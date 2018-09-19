#include "common/motor.h"
#include "common/error.h"
#include "common/time.h"
#include "common/uart.h"
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#define UART_TIMEOUT 500

// size of messages to and from the motor
// max length of a result (for non-MV commands) is
// address - 1 char, ack/nack - 1 char,
// an equal sign - 1 char
// a 32 bit int with possible negative sign - 11 chars
// a carriage return 1 - char + null char space - 1 char = 16 chars,
// this is why result is 16 characters long
#define MSG_LEN 16

#define CHARS_PER_READ 3

// minimimum gap between commands, in microseconds
#define MOTOR_CMD_MIN_GAP_US 50

// maximum number of motors supported by the module
#define MAX_MOTORS 4

/// time to wait, in ms for powerup
static const uint32_t POWERUP_WAIT_MS = 2000;

static const uint32_t MOTOR_GEAR_RATIO = 12; // motor turns to gearbox turns
static const uint32_t MOTOR_TICKS_PER_REV = 20000; // encoder ticks per rev
static const float MAX_GOTO_SPEED = 0.1f;

// the type of the argument
typedef enum {NO_ARG, INT32_ARG, FLOAT_ARG} arg_t;

/// motor control modes. these correspond to arguments for the CM command
enum motor_mode {POINT_TO_POINT = 21, CURRENT_CONTROL = 1};


struct motor_port
{
    // uart port on which to communicate with the motor
    const struct uart_port * uart_port;

    /// used to delay commands so they are not too fast
    /// for the motor to handle
    struct time_elapsed_us cmd_spacing;

    // if true a command has been sent and we are waiting for a response
    // otherwise we are free to send a new command
    bool awaiting_response;

    enum motor_mode mode;

    // length of the response buffer
    int len;

    /// buffer for holding partial responses
    char result[MSG_LEN];
};

/// @brief send a motor command with either 0 or 1 argument.
/// do not wait for a response
/// @param port - the motor port
/// @param cmd - the motor command, as a string
/// @param args - argument type specifier
/// @param arg - the argument to send
/// @param has_response - true  if the motor will respond, false otherwise
/// Commands that request data always respond, whereas commands that don't
/// request data have an optional response.
/// @pre to send a command we must not already be waiting for a response.
/// this sets the awaiting_response flag if has_response == true.
/// note that the awaiting_response is not strictly necessary with the motor
/// but it helps avoid bugs.  I specifically allow the SK stop command
/// through no matter what because it is important to stop the motor
/// even in the case of a send/receive matchup bug. I also allow a GC0 command
/// to set current to 0 in current control mode
static void motor_command_generic_send(struct motor_port * port,
                                       const char cmd[3],
                                       arg_t nargs,
                                       void * arg,
                                       bool has_response)
{
    if(!port)
    {
        error(FILE_LINE, "null ptr");
    }

    // even if awaiting response, still allow the stop command, 
    if(port->awaiting_response
       && !(cmd[0] == 'S' && cmd[1] == 'K')
       && !(cmd[0] == 'G' && cmd[1] == 'C' && 1 == nargs && 0 == *(int32_t*)arg)
        )
    {
        error(FILE_LINE, "must get response prior to sending");
    }

    if(nargs != NO_ARG && !arg)
    {
        error(FILE_LINE, "arg is NULL");
    }

    if(nargs == NO_ARG && arg)
    {
        error(FILE_LINE, "NO_ARG but arg != NULL");
    }

    // RS485 address of motor, then the command then \r
    char message[MSG_LEN] = "";
    int mesg_len = 0;

    switch(nargs)
    {
    case NO_ARG:
        message[0] = '0';
        message[1] = cmd[0];
        message[2] = cmd[1];
        message[3] = '\r';
        mesg_len = 4;
        break;
    case INT32_ARG:
        // subtract 1 byte from the length for the null character
        mesg_len = snprintf(message,
                            MSG_LEN,
                            "0%c%c%"PRIi32"\r",
                            cmd[0], cmd[1], *((int32_t *)arg)) ;
        break;
    case FLOAT_ARG:
        mesg_len = snprintf(message,
                            MSG_LEN,
                            "0%c%c%.4f\r",
                            cmd[0], cmd[1], (double)*((float*)arg));
        break;
    default:
        error(FILE_LINE,"nargs must be 0 or 1");
        break;
    }

    // if we don't want the ack, omit the 0 at the beginning of the command
    const int offset = has_response ? 0 : 1;
    // delay until the minimum gap between commands is done
    while(time_elapsed_us(&port->cmd_spacing) < MOTOR_CMD_MIN_GAP_US)
    {
        ;
    }

    uart_write_block(
        port->uart_port, message + offset, mesg_len - offset, UART_TIMEOUT);

    if(!has_response)
    {
        port->cmd_spacing = time_elapsed_us_init();
        port->awaiting_response = false;
    }
    else
    {
        port->awaiting_response = true;
    }
}

/// @brief check for a response from the motor and return the result
/// @param port - the motor port. the response is stored in the port's response
/// buffer, and its length is also stored in the port struct
/// @param cmd - command we are expecting the response to
///  this is used to validate the response
/// @param result - store the result in this buffer of MSG_LEN bytes
/// @return the length of the response, if complete,
///  or 0 if response is incomplete
/// @pre - must be awaiting a response to call this function. sets
//// awaiting_response flag to false after response is received
static int motor_command_generic_receive(struct motor_port * port,
                                          const char cmd[3],
                                          char result[MSG_LEN]
    )
{
    if(!port)
    {
        error(FILE_LINE, "null ptr");
    }

    if(!port->awaiting_response)
    {
        error(FILE_LINE, "noresp");
    }

    port->len += uart_read_nonblock(port->uart_port,
                                         port->result + port->len,
                                         MSG_LEN - port->len);
    if(0 == port->len)
    {
        return 0;
    }
    else if(port->result[port->len - 1] == '\r')
    {
        // packets end in \r so this is a complete packet
        int len = port->len;
        memcpy(result, port->result, len);

        // reset the temporary buffer
        port->len = 0;
        memset(port->result, 0 ,len);

        port->awaiting_response = false;
        // result now contains the response
        // re-initialize the motor sending timer
        port->cmd_spacing = time_elapsed_us_init();

        // check if packet is valid. Generally it should be prefixed
        // by the motor address '0', but sometimes the motor omits this
        // therefore we accept both forms
        int r = result[0] == '0' ? 1 : 0;
        if(((result[r] == '%' || result[r] == '*')
            || (result[r] == cmd[0]
                && result[r + 1] == cmd[1]
                && result[r + 2] == '=')))
        {
            return len;
        }
        else if('S' == cmd[0]  && 'K' == cmd[1])
        {
            // on a stop command don't worry if the response is wrong
            return len;
        }
        else
        {
            // there was an invalid response here
            error(FILE_LINE, "invalid response: ");
        }
    }
    else
    {
        // too many characters without a carriage return
        if(port->len > MSG_LEN)
        {
            error(FILE_LINE, "msg too long");
        }
        return 0;
    }
}


/// @brief  motor command with either 0 or 1 argument
/// @param port - port on which motor is connected
/// @param cmd - the two letter SCL command, as a c string
/// @param nargs - specifies the type of argument
/// @param arg - the argument if applicable (or NULL if there is no argument)
/// @param result[in/out] - the result, a MSG_LEN byte buffer.
///  should be set to all zeros prior to calling this function or NULL
/// if NULL then we assume that a response is not wanted.  NOTE that
/// commands that request data always send a response, so do not set this
/// to NULL when using such commands. this function will block until
/// the response is received, if one is requested. if a response is
/// not received on time, an error results
/// @return the length of the result, if a result is requested or 0 otherwise
static int motor_command_generic(struct motor_port * port,
                                 const char cmd[3],
                                 arg_t nargs,
                                 void * arg,
                                 char result[MSG_LEN])
{
    motor_command_generic_send(port, cmd, nargs, arg, NULL != result);
    if(result)
    {
        struct time_elapsed_ms elapsed = time_elapsed_ms_init();
        int len = 0;
        while(0 == len)
        {
            len = motor_command_generic_receive(port, cmd, result);

            if(time_elapsed_ms(&elapsed) > UART_TIMEOUT)
            {
                error(FILE_LINE, "motor response timeout");
            }
        }
        return len;
    }
    return 0;
}

/// @brief send an scl command to the motor that takes zero arguments
/// @param port - the port that is connected to the motor
/// @param cmd - the two-letter scl command
/// @param want_ack - if true we want to wait for an ack. only set
/// this to false if the command does not request data.  if the command
/// does request data then setting want_ack to false is potentially an error b/c
/// the motor will return data but the motor commands won't read that data
/// @return the result of the command. This value is only valid if the command
//   returns a result, want_ack is true, and the result is an integer
/// (some commands don't return integers, but most do)
/// if a NACK is received or a timeout occurs this is a fatal error
/// NOTE: this function does not work with commands that return something other
/// than an integer (such as MV)
static int32_t motor_command_arg0(struct motor_port * port,
                                  const char cmd[3], bool want_ack)
{
    char result[MSG_LEN] = ""; // important to initialize to all null characters
    motor_command_generic(port, cmd, NO_ARG, NULL, want_ack ? result : NULL);

    // note: result[] is already initialized to zero so we can always
    // check result[3] without worry
    if(result[3] == '=') // the motor has returned data
    {
        int32_t res = 0;
        if(1 == sscanf(result + 4, "%"PRIi32, &res))
        {
            return res;
        }
    }
    // there is no result
    return 0;
}

/// @brief send an scl command to the motor  that takes an integer argument
/// @param port - the port that is connected to the motor
/// @param cmd - the two-letter scl command
/// @param arg - argument for the command, an integer
/// @param await - if true an ack is desired, if false it is not. either way,
/// this function does not actually wait for the ack
static void motor_command_i32_nonblock(struct motor_port * port,
                               const char cmd[3], int32_t arg, bool await)
{
    motor_command_generic_send(port, cmd, INT32_ARG, &arg, await);
}

/// @brief send an scl command to the motor  that takes an integer argument.
/// wait for the response
/// @param port - the port that is connected to the motor
/// @param cmd - the two-letter scl command
/// @param arg - argument for the command, an integer
static void motor_command_i32_block(struct motor_port * port,
                               const char cmd[3], int32_t arg)
{
    char result[MSG_LEN] = "";
    motor_command_generic(port, cmd, INT32_ARG, &arg, result);
}

/// @brief send an scl command to the motor that takes a floating point argument
/// @param port - the port that is connected to the motor
/// @param cmd - the two-letter scl command
/// @param arg - argument for the command, a float
/// @param await - solicit a response from the motor if true, otherwise motor
/// will not respond
static void motor_command_f_nonblock(struct motor_port * port,
                                const char cmd[3], float arg, bool await)
{
    motor_command_generic_send(port, cmd, FLOAT_ARG, &arg, await);
}

/// @brief send an scl command to the motor that takes a floating point argument
/// and wait for the response
/// @param port - the port that is connected to the motor
/// @param cmd - the two-letter scl command
/// @param arg - argument for the command, a float
static void motor_command_f_block(struct motor_port * port,
                                  const char cmd[3], float arg) 
{
    char result[MSG_LEN] = "";
    motor_command_generic(port, cmd, FLOAT_ARG, &arg, result);
}

/// convert an angle at the gearbox, in radians, to encoder ticks
static int32_t motor_gear_radians_to_ticks(float radians)
{
    // the negative sign is because the
    // motor's encoder count increases with CW not CCW rotation
    return -radians*(float)MOTOR_GEAR_RATIO
        *((float)MOTOR_TICKS_PER_REV)/(2.0f*PI);
}

struct motor_port * motor_startup(const char name[],
                                  enum motor_baud baud,
                                  float curr_rad,
                                  float lower_rad,
                                  float upper_rad)
{
    static struct motor_port ports[MAX_MOTORS] = {{0}};
    static int portdex = 0;
    if(portdex == MAX_MOTORS)
    {
        error(FILE_LINE, "too many motors open");
    }
    struct  motor_port * port = &ports[portdex];
    ++portdex;
   
    port->uart_port =
        uart_open(name, 9600, UART_FLOW_NONE, UART_PARITY_NONE);
    port->cmd_spacing = time_elapsed_us_init();
    port->awaiting_response = false;
    port->len = 0;
    memset(port->result, 0, MSG_LEN);

    // motor was not just turned on
    if(!motor_powerup_wait(port))
    {
        port->uart_port
            = uart_open(name, baud, UART_FLOW_NONE, UART_PARITY_NONE);
        // reset the motor
        motor_command_arg0(port, "RE", true);
        uart_close(port->uart_port);
        port->uart_port
            = uart_open(name, 9600, UART_FLOW_NONE, UART_PARITY_NONE);
        if(!motor_powerup_wait(port))
        {
            error(FILE_LINE, "missed powerup packet");
        }
    }

    uart_close(port->uart_port);
    port->uart_port = uart_open(name, baud, UART_FLOW_NONE, UART_PARITY_NONE);
    // again wait for the motor to be ready, it takes 1 second after
    // the power up packet is received. during this time the motor
    // waits for a special signal to enter recovery mode, which we don't send
    time_delay_ms(1000);
    motor_disable(port);

    // set the encoder positions and encoder limits
    if(0.0f != curr_rad || 0.0f != upper_rad || 0.0f != lower_rad)
    {
        const int32_t curr_ticks = motor_gear_radians_to_ticks(curr_rad);
        // set motor current location
        motor_command_i32_block(port, "EP", curr_ticks);
        motor_command_i32_block(port, "SP", curr_ticks);

        // set motor limits
        motor_command_i32_block(port,
                                "LM", motor_gear_radians_to_ticks(upper_rad));
        motor_command_i32_block(port,
                                "LP", motor_gear_radians_to_ticks(lower_rad));
    }
    else if(upper_rad < lower_rad)
    {
        // lower rad must be smaller than upper rad
        error(FILE_LINE, "Invalid motor limits");
    }

    // set some settings. usually these are saved in nonvolatile memory
    // during construction of the robot, but we set them here just in case

    // enable point to point mode
    motor_mode_point_to_point(port);

    // set the motor to use limit switches
    motor_command_i32_block(port, "DL", 2);

    // point-to-point velocity, slow it down
    motor_command_f_block(port, "VE", MAX_GOTO_SPEED);

    // initial jog speed to zero
    motor_command_f_block(port, "JS", 0);
    return port;
}

const struct uart_port * motor_uart(const struct motor_port * port)
{
    if(!port)
    {
        error(FILE_LINE, "null ptr");
    }
    return port->uart_port;
}

bool motor_powerup_wait(struct motor_port * port)
{
    if(!port)
    {
        error(FILE_LINE, "null ptr");
    }
    uint8_t powerup[3] = {0};
    struct time_elapsed_ms elapsed = time_elapsed_ms_init();

    /// motor sends 0xFF<Version byte><model byte> when it is ready,
    /// at 9600 baud.
    /// then it switches to the programmed baud rate
    int read = 0;
    static const int rlen = 3;
    while(read != 3 && time_elapsed_ms(&elapsed) < POWERUP_WAIT_MS)
    {
        read +=
            uart_read_nonblock(port->uart_port, powerup + read, rlen - read);
    }

    if(3 == read && 0xFF == powerup[0])
    {
        return true;
    }
    else if(3 == read && 0xFF != powerup[0])
    {
        error(FILE_LINE, "invalid powerup");
    }
    else
    {
        return false;
    }
}

void motor_disable(struct motor_port * port)
{
    motor_command_arg0(port, "MD", true);
}

void motor_enable(struct motor_port * port)
{
    motor_command_arg0(port, "ME", true);
}


bool motor_goto_radians_nonblock(struct motor_port * port, float radians)
{
    static const char fp_cmd[] = "FP";
    char result[MSG_LEN] = "";
    if(port->awaiting_response)
    {
        const int len = motor_command_generic_receive(port, fp_cmd, result);
        if(0 == len)
        {
            return false;
        }
        else if(3 == len)
        {
            return true;
        }
        else
        {
            error(FILE_LINE, "invalid_resp");
        }
    }
    else
    {
        motor_command_i32_nonblock(port, fp_cmd,
                                   motor_gear_radians_to_ticks(radians), true);
    }
    return false;
}

void motor_stop(struct motor_port * port)
{
    // Use SK to stop and kill any queued commands
    if(POINT_TO_POINT == port->mode)
    {
        motor_command_arg0(port, "SK", true);
    }
    else
    {
        motor_command_i32_block(port, "GC", 0);
    }
}


void motor_status_block(struct motor_port * port, union motor_status * out)
{
    struct time_elapsed_ms elapsed = time_elapsed_ms_init();

    while(!motor_status_nonblock(port, out))
    {
        if(time_elapsed_ms(&elapsed) > UART_TIMEOUT)
        {
            error(FILE_LINE, "timeout");
        }
    }
}

bool motor_status_nonblock(struct motor_port * port, union motor_status * out)
{
    // TODO: use the SC command to make this more efficient
    char result[MSG_LEN] = "";
    static const char status_cmd[] = "SC";
    if(port->awaiting_response)
    {
        const int len = motor_command_generic_receive(port, status_cmd, result);
        if(0 == len)
        {
            return false;
        }
        else if(9 == len)
        {
            long res = strtol(result + 4, NULL, 16);

            // motor response is invalid
            if(0 != errno || res < 0 || res > 0xFFFF) 
            {
                error(FILE_LINE, "invalid resp");
            }

            // if out is null, caller does not care about the actual status
            if(out)
            {
                out->code = (uint16_t)res;
            }
            return true;
        }
        else
        {
            error(FILE_LINE,"invalid resp");
        }
    }
    else
    {
        motor_command_generic_send(port, status_cmd, NO_ARG, NULL, true);
    }
    return false;
}

void motor_jog_start(struct motor_port * port)
{
    // put the motor into jog mode. our motor is assumed
    // to have JS = 0 so it wont move right away
    motor_command_arg0(port, "CJ", false);
}


void motor_jog_speed(struct motor_port * port, float speed)
{
    if(speed < -MOTOR_MAX_REV_SEC)
    {
        speed = -MOTOR_MAX_REV_SEC;
    }
    else if(speed > MOTOR_MAX_REV_SEC)
    {
        speed = MOTOR_MAX_REV_SEC;
    }
    motor_command_f_nonblock(port, "CS", speed, false);
}


void motor_mode_point_to_point(struct motor_port * port)
{
    motor_command_i32_block(port, "CM", POINT_TO_POINT);
    port->mode = POINT_TO_POINT;
}

void motor_mode_current(struct motor_port * port)
{
    motor_command_i32_block(port, "CM", CURRENT_CONTROL);
    port->mode = CURRENT_CONTROL;
}



void motor_current_set(struct motor_port * port, float current)
{
    if(CURRENT_CONTROL != port->mode)
    {
        /// must be in current control mode to set the current
        error(FILE_LINE, "wrong mode");
    }

    if(current < -MOTOR_MAX_AMPS)
    {
        current = -MOTOR_MAX_AMPS;
    }
    else if(current > MOTOR_MAX_AMPS)
    {
        current = MOTOR_MAX_AMPS;
    }
    // current command is in 10 ma units
    motor_command_i32_nonblock(port, "GC", current * 100.0f, false);
}

void motor_torque_set(struct motor_port * port, float newton_meters)
{
    motor_current_set(port, newton_meters/(MOTOR_TORQUE_CONSTANT*(float)MOTOR_GEAR_RATIO));
}

#ifndef COMMON_MOTOR_H
#define COMMON_MOTOR_H
/// @brief file for controlling TSM-23Q using SCL commands
#include <stdint.h>
#include <stdbool.h>

struct motor_port;


/// @brief motor status as returned by the SC (status code) command
union motor_status
{
    uint16_t code;             // the full status code
    struct
    {
        unsigned enabled     : 1; // motor enabled if 1 disabled if 0
        unsigned sampling    : 1; // used by quick tuner software
        unsigned fault       : 1; // there is an alarm code
        unsigned in_position : 1; // motor in requested position
        unsigned moving      : 1; // motor is moving
        unsigned jogging     : 1; // motor is in jog mode
        unsigned stopping    : 1; // motor in the process of stopping
        unsigned wait_input  : 1; // motor waiting for an input (executing WI)
        unsigned saving      : 1; // motor is saving a parameter
        unsigned alarm       : 1; // an alarm code is present
        unsigned homing      : 1; // motor is homing (executing SH)
        unsigned wait_time   : 1; // motor is waiting for some time
        unsigned wizard      : 1; // timing wizard is running
        unsigned encoder     : 1; // timing wizard is checking the encoder
        unsigned qprog       : 1; // q program is running
        unsigned init        : 1; // motor is inititializing itself
    } bits;
};

/// @brief supported baud rates for the motor
enum motor_baud
{
    MOTOR_B9600 = 9600,
    MOTOR_B19200 = 19200,
    MOTOR_B38400 = 38400,
    MOTOR_B57600 = 57600,
    MOTOR_B115200 = 115200
};

/// @brief initiates the motor startup sequence, resetting it to
/// it's power-on state and disabling it, and returning a communication port
/// on which you can communicate with the motor. This should be called prior to
/// any other communication with the motor
/// @param name[] - name of the port to open
/// @param baud - baud rate of the motor.
/// @param curr_rad - absolute angle that the motor should consider its current
/// output position (after the gearbox)
/// @param lower_rad - min angle (radians) of motor output (after the gearbox)
/// @param upper_rad - max angle (radians) of motor output (after the gearbox)
/// If all limits and the curr_rad are == to 0.0f then no limits are set.
/// The motor will also be configured to use limit switches.
/// @return a port on which to communicate with the motor. if too many ports are
/// open, this causes an error.  The motor will be in the disabled state.
/// call motor_enable to enable the motor
struct motor_port * motor_startup(const char name[], enum motor_baud baud,
                                  float curr_rad, float lower_rad,
                                  float upper_rad);

/// @brief get the underlying uart from the motor port
const struct uart_port * motor_uart(const struct motor_port * port);

/// @brief wait to detect the powerup packet for a fixed period of time
/// @param port - motor port, set to 9600 baud
/// @return true if powerup packet was detected within the time frame,
///  false if no powerup packet was sent.  If an invalid powerup packet
/// is sent, trigger an error
bool motor_powerup_wait(struct motor_port * port);

/// @brief disable the motor and waits for motor acknowledgment before returning
void motor_disable(struct motor_port * port);

/// @brief enable the motor and waits for motor ack before returning
void motor_enable(struct motor_port * port);

/// @brief send the motor output (after the gearbox) to the given angle.
/// once this is called, you cannot send further commands to the motor
/// @param port - port on which to communicate with the motor
/// @param radians - angle (inr adians) to send the motor, in interval (PI, PI]
/// @return true when the motor has acknowledged receipt of the goto command.
/// false otherwise.  You must keep calling this function until it returns true
/// prior to using other motor functions (with the exception of motor_stop)
/// @pre motor_set_limits must have been called to set absolute angle reference
/// @pre motor_enable must be called for the motor to move
/// NOTE: this function may return prior to motor reaching its final position
/// use motor_status to determine when the final position is reached
bool motor_goto_radians_nonblock(struct motor_port * port, float radians);

/// @brief stop the motor
/// @param port - the port controlling the motor
/// this function waits for the reply from the command
void motor_stop(struct motor_port * port);


/// @brief get the motor's status and block until it returns it or times out
/// a timeout triggers an error
/// @param port - the port on which to communicate to the motor
/// @param out - the status of the motor, or null if status is ignored,
/// just the fact that status has been received is important
void motor_status_block(struct motor_port * port, union motor_status * out);

/// @brief query the motor's status but don't wait for it to return
/// Once this is called, you cannot send further commands to the motor until
/// this function returns true and returns that status
/// @param port - the port on which to communicate to the motor
/// @param out - the motor status, only valid when this function returns true
/// can be null if you wish to discard the status and simply check if it has
/// arrived
/// @param return true when the motor status has been received
bool motor_status_nonblock(struct motor_port * port, union motor_status * out);

/// @brief put the motor into jogging mode.
/// the initial jogging velocity will be zero so the motor
/// will not move.  You must set joint limits and enable the motor prior
/// to calling this function.
/// @param port - the port on which the motor is connected
void motor_jog_start(struct motor_port * port);

/// @brief set the motor jogging speed.  motor must be in jogging mode
/// @param port - the port on which the motor is connected
/// @param speed of the motor in rev/sec.  Speed is saturated 
void motor_jog_speed(struct motor_port * port, float speed);


/// @brief put motor in point-to-point mode. this is the default mode
/// and allows use of the higher-level motion commands
void motor_mode_point_to_point(struct motor_port * port);

/// @brief puts the motor into current control mode. The only commands
/// available are motor_current_set() and motor_mode_point_to_point to exit
/// current control mode. 
void motor_mode_current(struct motor_port * port);

/// @brief set the commanded current to the motor.  must be in current control
/// mode.
/// @param port - the motor port
/// @param current - signed motor current, in amps,  will be saturated at
/// MOTOR_MAX_AMPS.
void motor_current_set(struct motor_port * port, float current);

/// @brief set the commanded torque to the motor at the output of the gearbox.
/// must be in current control mode
/// @param port - the motor port
/// @param newton_meters - motor torque
void motor_torque_set(struct motor_port * port, float newton_meters);

// actual max speed is 60, 
#define MOTOR_MAX_REV_SEC  60.0f
/// actual motor max amps is 6
#define MOTOR_MAX_AMPS 6.0f

/// units are Nm/A
#define MOTOR_TORQUE_CONSTANT 0.4f
#endif

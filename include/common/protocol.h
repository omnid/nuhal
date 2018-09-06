#ifndef COMMON_PROTOCOL_H
#define COMMON_PROTOCOL_H
/// @brief implements the communication protocol
/// Uses request/response protocol.
/// Communication happens between D1 and D2
/// Either D1 or D2 can initiate a transaction by
/// sending a request.  If D1 sends a request to D2,
/// then D2 must reply with a response, and vice versa
#include<stdint.h>
#include<stdbool.h>
#include"common/type.h"
#include"common/bytestream.h"

#ifdef __cplusplus
extern "C" {
#endif

///@brief commands that are recognized
/// PROTOCOL_BOOTLOAD_xxxx commands are determined by the bootloader protocol
/// so their values must remain fixed
/// (@see protocol.c for details)
enum protocol_command
{
    /// sent on first run to initialize the eeprom
    /// REQUEST and RESPONSE .param is NONE
    PROTOCOL_EEPROM_INIT = 0x00

    /// passthrough from one uart to another
    /// REQUEST .param is processor_id, this is ignored
    /// if there are not multiple processors
    ///RESPONSE .param is NONE
    ,PROTOCOL_PASSTHROUGH = 0x01

    /// test the communication
    /// REQUEST and RESPONSE .param is NONE
    ,PROTOCOL_COMM_TEST = 0x02

    // request encoder data
    // RESPONSE param is type_joint_encoders
    ,PROTOCOL_ENCODERS = 0x03

    /// move the motor to the desired angle
    /// This uses the built-in motor position control
    /// and position is specified as the position of the
    /// output shaft of the gearbox
    /// REQUEST param is .radians
    /// RESPONSE param is NONE
    ,PROTOCOL_MOTOR_GOTO_RADIANS = 0x04

    /// jog the motor at the desired speed
    /// REQUEST param is .radsec - speed in radians/sec for motor
    /// RESPONSE Param is NONE
    ,PROTOCOL_MOTOR_JOG_RADSEC = 0x05

    /// stop all control, nothing moves
    /// REQUEST and RESPONSE param is NONE
    ,PROTOCOL_STOP = 0x06

    /// move the arm with the desired velocity
    /// REQUEST is a float that has rad_sec of desired arm velocity
    /// RESPONSE param is the pid_data that is returned for the first
    /// control loop cycle that is executed
    ,PROTOCOL_ARM_VELOCITY= 0x07

    /// set the pid gains for the ARM angle controller
    /// REQUEST param is .pid_gains
    /// RESPONSE param is NONE
    ,PROTOCOL_ARM_PID_GAINS_SET = 0x08

    /// get the pid gains from the ARM angle controller
    /// REQUEST param is NONE
    /// RESPONSE param is .pid_gains
    ,PROTOCOL_ARM_PID_GAINS_GET = 0x09

    /// get the control loop frequency of the arm in HZ
    /// REQUEST param is NONE
    /// RESPONSE param is .frequency
    ,PROTOCOL_CONTROL_HZ = 0x0A

    /// enter bootloading mode
    /// REQUEST and RESPONSE is NONE
    ,PROTOCOL_BOOT_ENTER = 0x0B

    /// an error has occurred
    /// This has no data.   An error message
    /// can subsequently be set using PROTOCOL_STRING
    ,PROTOCOL_ERROR = 0x0C

    // get the version of the firmware
    // REQUEST param is NONE
    // RESPONSE .param is version
    ,PROTOCOL_FIRMWARE_VERSION = 0x0D

    /// save arm pid gains to eeprom
    ,PROTOCOL_ARM_PID_GAINS_SAVE = 0x0E

    /// load arm pid gains from eeprom
    ,PROTOCOL_ARM_PID_GAINS_LOAD = 0x0F

    /// move the delta platform to the given position
    /// REQUEST param is linear_position followed by the type_control_mode type
    ,PROTOCOL_DELTA_POSITION_SET = 0x10

    /// get the arm state from a joint controller
    /// RESPONSE param is arm_state, joints before and after the spring
    ,PROTOCOL_ARM_STATE = 0x11

    /// bootload processors connected downstream
    ,PROTOCOL_BOOT_CHILDREN = 0x12

    /// get the delta full robot state
    /// RESPONSE param is type_delta_state
    ,PROTOCOL_DELTA_STATE =  0x13

    /// set the torque of a single motor
    /// REQUEST param is newton_meters, the torque in newton meters
    ,PROTOCOL_MOTOR_TORQUE = 0x14

    //// the emergency stop is active
    ,PROTOCOL_ESTOP = 0x15

    //// set the color of the leds
    /// REQUEST .param is type_led_color the lower 3 bits correspond to
    /// the RED GREEN and BLUE leds being on/off respectively (on = 1, off = 0)
    /// RESPONSE is the type_led_color that was previously set
    ,PROTOCOL_LED_SET = 0x16

    /// REQUEST/RESPONSE can optionally contain a string
    /// encoded with bytestream_inject_string
    ,PROTOCOL_STRING = 0x17

    /// trajectory control, details TODO
    ,PROTOCOL_DELTA_TRAJECTORY = 0x18

    /// enable or disable the motors remotely. true for enabled
    /// false for disabled
    ,PROTOCOL_MOTOR_SET_ENABLE_STATE = 0x19

    /// calibrate the delta robot.  Robot end effector should be placed at
    /// a known location.  type_linear_position with the currently specified
    /// robot location is sent to the robot
    ,PROTOCOL_DELTA_CALIBRATE = 0x20

    /// calibrate a joint.  sends an angle that should be the current angle of
    /// BOTH joint encoders
    ,PROTOCOL_JOINT_CALIBRATE = 0x21
};

/// default timeout to wait for a response when executing a request or broadcast
/// units of ms
#define PROTOCOL_TIMEOUT_DEFAULT 200u

/// repreresents a major.minor version
struct protocol_version
{
    char id[16];    // an identifier
    uint8_t major; // major version
    uint8_t minor; // minor version
};

/// @brief the communications packet that is sent between processors
/// To build a protocol_packet
/// 1. call protocol_packet_init
/// 2. use the stream member to inject data into the stream
/// To parse a protocol_packet
/// 1. call any protocol function that outputs a packet
/// 2. use the stream member to extract data from the stream
struct protocol_packet
{
    // used for serializing/deserializing the packet
    struct bytestream stream;

    // the actual data stored in the packet: do not access directly
    // instead packets should be build/parsed using the bytestream
    uint8_t _data[255];
};

struct uart_port;

/// @brief open a uart port for use with the protocol
/// @param uart_port_name - the name of the underlying uart port
/// @return a handle to the uart_port
const struct uart_port * protocol_open(const char uart_port_name[]);

/// @brief initialize a packet with the given command byte
/// @param out [out] - the packet to be initialized
/// @param command - the command byte to set.
/// @return pointer to the bytestream where the data payload can be placed
/// The bytestream is only valid for the lifetime of *out
void protocol_packet_init(struct protocol_packet * out, uint8_t command);

/// @brief get the command byte of the packet
/// @param in - the packet to parse
uint8_t protocol_packet_command(const struct protocol_packet * in);

/// @brief send a packet to the port
/// @param port - port on which to send the packet
/// @param packet [in/out] - the packet to send over the port.
/// the packet will be updated with the proper length and checksum.
/// @post the timeout is predetermined by the protocol and violating it
/// results in an error
/// @post the stream on the packet will be at its end position
/// and cannot be written to/read from again
void protocol_write_block(const struct uart_port * port,
                          struct protocol_packet * packet);

/// @brief wait for a protocol packet to be received
/// @param port - the protocol port
/// @param out [out] - contains the packet that was read, or NULL to ignore the
/// packet that is read. Data will still be read but will not be returned
/// The stream member will point to the beginning of the data payload,
/// after the header and the command byte
/// @param timeout - minimum timeout to wait for the packet (in ms)
/// if zero there is no timeout. There is also a per byte timeout
/// so longer packets receive slightly more time
void protocol_read_block(const struct uart_port * port,
                         struct protocol_packet * out,
                         uint32_t timeout);

/// @brief attempt to read a packet if it is available
/// @param port - the protocol port
/// @param out [out] - contains the packet that was read, if any
/// @return true if a packet was read, false otherwise
/// If there is any data available this function attempts to read
/// a full packet.  If the full packet is not ready within a timeout period
/// (dependent on packet length) an error occurs.
bool protocol_read_nonblock(const struct uart_port * port,
                            struct protocol_packet * out);

/// @brief send a request and wait for the matching response
/// @param port - the protocol port
/// @param in [in/out] - the request packet to send. packet will be modified
/// in the same way that it is in @see protocol_write_block
/// @param out - the response. if NULL the response is still checked internally
/// but the data is discarded
/// @post the response is validated using protocol_validate_response
/// and an error will occur if the response is invalid
/// @post an error also occurs if there is a timeout waiting for the response
void protocol_request(const struct uart_port * port,
                      struct protocol_packet * in,
                      struct protocol_packet * out);

/// @brief @see protocol_request
/// @param timeout_ms timeout in ms - base time to wait
void protocol_request_timeout(const struct uart_port * port,
                              struct protocol_packet * in,
                              struct protocol_packet * out,
                              uint32_t timeout_ms);

/// @brief execute the error message handshake
///  The handshake consists of sending a PROTOCOL_ERROR packet and waiting for
///  a response (this may timeout causing an additional error).
///  Upon receiving a response we send a
///  PROTOCOL_STRING message with the fileline and error msg
///  this is a two part process to avoid overflowing any RX buffers on the
/// receiving side. 
/// @param port - the protocol port
/// @param fileline - the file and line-number of the error message
/// @param message - the additional error information
void protocol_error_message(const struct uart_port * port,
                            const char * fileline,
                            const char * message);

/// @brief validate that a response is a valid reply to the given request
/// @param port - the port over which the communication is occurring
/// @param request - a packet that was sent
/// @param response - a packet that was recieved
/// @post trigger an error on an invalid response
///       a  response is invalid if its command does not match the request
///       if it is a PROTOCOL_ERROR command, then an error handshaking
///       sequence occurs (see protocol_error_message) 
void protocol_validate_response(const struct uart_port * port,
                                const struct protocol_packet * request,
                                const struct protocol_packet * response);

/// @brief broadcast a request to 3 ports and get the responses
/// (if response is NULL, then the response is retrieved but not returned)
/// @param ports - the three ports to which the packet should be broadcast.
/// @param pkt [in/out] - the packet to send.  either 1 packet sent to all
/// 3 ports or 3 packets, each sent to its corresponding ports
/// packets must have same length.  packets are modified in the same way that
/// @see protocol_write_block modifies them
/// @param response - response buffer from all 3 ports
/// if NULL then this still waits for the response but the data is discarded
/// @param all - if true, all the messages are the same. if false, each message
/// is different
/// @param timeout_ms - timeout in ms. Use PROTOCOL_TIMEOUT_DEFAULT
/// for default value
/// NOTE: this interleaves the sending and receiving of all the data
/// Rather than sending each packet in sequence and receiving three packets in
/// sequence

void protocol_broadcast(const struct uart_port * const ports[],
                        struct protocol_packet  pkt[],
                        struct protocol_packet response[],
                        bool all,
                        uint32_t timeout);

/// @brief serialize the protocol version
/// @param bs - the bytestream
/// @param vs - the version
void protocol_inject_version(struct bytestream * bs,
                             const struct protocol_version * vs);

/// @brief deserialize the protocol version
/// @param bs - the bytestream
/// @param vs - the version
void protocol_extract_version(struct bytestream * bs,
                              struct protocol_version * vs);
#ifdef __cplusplus
}
#endif

#endif

#ifndef NUHAL_PROTOCOL_H
#define NUHAL_PROTOCOL_H
/// @file
/// @brief implements the basics of the communication protocol
/// The protocol uses the same  format as the tiva bootloader protocol
/// @see TI Application Report SPMA074A "Implementation of Programmer
/// for Serial Bootloaders on TM4C12x Microcontroller'
///
/// A packet has the following structure
/// byte 0 - length (N) in bytes, including the length byte
///          FOR THE BOOTLOADER ONLY: If there is no data payload
///          (as in an ack/nack) this might be zero.
///          NOTE: in practice bootloader sends 0x00 0xCC as an ack, not 0xCC
/// byte 1 - Checksum:
///      if N != 0, 0xFF & \f$\sum_{i=2}^{N}byte[i]\f$,
///      if N == 0, 0xCC
/// byte 2..N: the raw data
///
/// bytes 0 and 1 constitute the header
/// bytes 2 .. N: constitute the data
///
/// Format of the data
///
/// data[0] - the command byte
/// data[1]...data[N] the data
///   data can serialized/deserialized using  bytestream.
///   For basic types, use the provided bytestream_inject/bytestream_extract functions
///   custom types can implement their own inject/extract functions that use the built-in bytestream inject/extrac
///   functions as a base and a guide

#include<stdint.h>
#include<stdbool.h>
#include"nuhal/bytestream.h"
#include"nuhal/utilities.h"

/// @brief the maximum length of a protocol packet
#define PROTOCOL_PACKET_MAX_LENGTH 255

/// @brief the communications packet that is sent between processors
/// The packet must be initialized prior to use, either by
/// 1. Call protocol_packet_init
/// 2. Use the stream member to inject data into the stream
/// OR
/// 1. Call any protocol function that outputs a packet
/// 2. Use the stream member to extract data from the stream
struct protocol_packet
{
    /// \brief Used for serializing/deserializing the packet.
    ///
    /// The initialization functions set up the stream so that it
    /// manages the _data member.
    struct bytestream stream;

    /// \brief Actual data stored in the packet: do not access directly.
    ///
    /// Instead packets should be built/parsed using the bytestream
    uint8_t _data[PROTOCOL_PACKET_MAX_LENGTH];
};

struct uart_port;

#ifdef __cplusplus
extern "C" {
#endif

/// indicate that there was an error processing the request.
#define PROTOCOL_ERROR 0xFF

/// @brief type of relationship used for sending data to joints
enum protocol_broadcast_type
{
    // send 1 packet to 3 destinations
    PROTOCOL_BROADCAST,

    /// send 3 packets to 3 destinations
    PROTOCOL_ANYCAST,
};

/// default timeout to wait for a response when executing a request or broadcast
/// units of ms
#define PROTOCOL_TIMEOUT_DEFAULT  200u

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

/// @brief @see protocol_read_block,
/// @param[in] port The uart port
/// @param[out] out The packet that is to be read.
/// @param timeout The time, in ms, to wait before timing out. 0 is an infinite timeout
/// @param timeout_error  If true a timeout casues an error()
///             if false, the function returns false if it times out
/// @return true unless a timeout occurs and timeout_error == false
bool protocol_read_block_error(const struct uart_port * port,
                               struct protocol_packet * out,
                               uint32_t timeout,
                               bool timeout_error);

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

/// @brief Send a protocol packet and wait for a response
/// @param port The port over which to send the packet
/// @param[in] in The packet to send
/// @param[out] out The packet with the received data
/// @param timeout_ms timeout in ms - base time to wait
/// @param timeout_error - if true, an error occurs on timeout
/// if false, this function returns false on timeout
/// @return false if the request timed out
bool protocol_request_timeout(const struct uart_port * port,
                              struct protocol_packet * in,
                              struct protocol_packet * out,
                              uint32_t timeout_ms,
                              bool timeout_error);


/// @brief Send a request to N ports (interleaving the transmissions)
/// Then waits for the response for each port (interleaving the waiting)
/// @param ports - the ports to which the packet should be broadcast,
/// @param num_ports - the number of ports to send data to. max is 3
/// @param pkt [in/out] - the packet to send.  either 1 packet sent to all
/// ports or num_ports packets, each sent to its corresponding port.
/// each packet must be the same length
/// @param response - response buffer from all the ports
/// if NULL then wait for the response but discard the data
/// @param btype -PROTOCOL_BROADCAST - pkt[0] is sent to all ports
///               PROTOCOL_ANYCAST - pkt[i] is sent to ports[i],
/// @param timeout - timeout in ms.
///
/// This function interleaves the sending and receiving of all the data
/// Rather than sending each packet in sequence and receiving three packets in
/// sequence.
void protocol_broadcast_timeout(const struct uart_port * const ports[],
                                unsigned int num_ports,
                                struct protocol_packet  pkt[],
                                struct protocol_packet response[],
                                enum protocol_broadcast_type btype,
                                uint32_t timeout);

/// @brief same as @see protocol_broadcast-timeout except uses the default value
void protocol_broadcast(const struct uart_port * const ports[],
                        unsigned int num_ports,
                        struct protocol_packet  pkt[],
                        struct protocol_packet response[],
                        enum protocol_broadcast_type btype);

/// @brief send an error response to a packet with the given command
/// @param port - the port on which to send the packet
///
/// This function sends a packet with a bitwise-inverted command byte
/// When validating the response, the initiator of the request detects
/// this as an error.  This should be called if the processor cannot properly
/// respond to a request to indicate to the parent that an error has occurred.
void protocol_error_response(const struct uart_port * port);

#ifdef __cplusplus
}
#endif

#endif

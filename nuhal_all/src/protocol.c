#include <string.h>
#include "nuhal/protocol.h"
#include "nuhal/error.h"
#include "nuhal/uart.h"
#include "nuhal/time.h"

/// @file
/// @brief Implements the protocol
///
/// @desc Protocol description:
/// The protocol uses the same  format as the tiva bootloader protocol
/// described in TI application report SPMA074A "Implementation of Programmer
/// for Serial Bootloaders on TM4C12x Microcontroller
/// A packet has the following structure
/// byte 0 - length (N) including itself.  Special case is for the bootloader
/// The length byte may be 0 if there is no data payload
/// byte 1 - checksum
/// byte 2 .. N - data
/// multi byte values are stored in Big Endian format
/// in general, byte 2 contains the command byte
// the checksum is computed as 0xFF & \sum_{i=2}^{N}byte[i],
// unless N = 0 in which case the checksum is 0xCC
// THus ack and nack are treated as checksum passing/errors on
// packets of zero bytes (note in practice the bootloader sends
// 00 CC as an ack, not just CC as documented.

/// number of bytes in the length
#define LENGTH_BYTES 1

/// number of bytes in the checksum
#define CHECKSUM_BYTES 1

/// number of command bytes
#define COMMAND_BYTES 1

/// number of bytes in the header
#define HEADER_BYTES (LENGTH_BYTES + CHECKSUM_BYTES)

/// max length of an error message
#define ERROR_MESSAGE_MAX_BYTES \
    (ARRAY_LEN(((struct protocol_packet*)0)->_data)   \
     - HEADER_BYTES - COMMAND_BYTES)

/// max number of broadcasting ports
#define MAX_BROADCAST_PORTS 3

// index of the length byte
static const uint32_t LENGTH_INDEX = 0;

// index of the checksum byte
static const uint32_t CHECKSUM_INDEX = 1;

/// baud for the uart
static const uint32_t BAUD = 1000000u;

// timeout for the uart to read a single byte, in MS
static const uint32_t TIMEOUT_MS_PER_BYTE = 10u;

// base timeout for commands:
static const uint32_t TIMEOUT_MS_BASE = 100u;


/// @brief compute the checksum of a packet
static uint8_t protocol_checksum(struct protocol_packet * packet)
{
    // checksum is sume of all bytes mod 255 so take advantage
    // of unsigned integer overflow
    uint8_t chsum = 0;

    // a data length of zero (including no command byte) is a special
    // case for the checksum, for compatibility with the bootloader protocol
    // the checksum is 0xCC
    if(0 == packet->_data[LENGTH_INDEX])
    {
        return 0xCC;
    }

    // header is not part of the checksum
    for(uint8_t i = HEADER_BYTES; i != packet->_data[LENGTH_INDEX]; ++i)
    {
        chsum += packet->_data[i];
    }
    return chsum;
}

/// @brief initialize the packet header and return its length
/// @param packet [out] - the packet to initialize
/// @return the length of the packet, in bytes, including the header
static uint8_t protocol_header_init(struct protocol_packet * packet)
{
    if(!packet)
    {
        error(FILE_LINE, "NULL ptr");
    }
    // the length of the packet
    packet->_data[LENGTH_INDEX] = packet->stream.size + HEADER_BYTES;
    packet->_data[CHECKSUM_INDEX] = protocol_checksum(packet);

    // update the stream capacity to its actual size, so that nothing
    // more can be written to it without error
    packet->stream.capacity = packet->stream.size;
    return packet->_data[LENGTH_INDEX];
}

/// @brief check that a packet with complete data has a valid
/// checksum and set its stream to the beginning of its data section
/// @param out - the packet
/// @param data_length - the length of the data payload of the packet
static
void protocol_verify_checksum(struct protocol_packet * out, uint8_t data_length)
{
    if(!out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    // compute the checksum
    uint8_t checksum = protocol_checksum(out);
    if(checksum != out->_data[CHECKSUM_INDEX])
    {
        error(FILE_LINE, "invalid checksum");
    }

    // set the stream to the proper position
    bytestream_init(&out->stream, &out->_data[HEADER_BYTES], data_length);
    // skip over the command byte (the only time there is no command byte
    // is with a bootloader ACK
    if(out->_data[LENGTH_INDEX] > 0)
    {
        (void)bytestream_extract_u8(&out->stream);
    }
}

/// @brief validate that a response is a valid reply to the given request
/// @param request - a packet that was sent
/// @param response - a packet that was recieved
/// @post trigger an error on an invalid response
///       a  response is invalid if its command does not match the request
static void protocol_validate_response(const struct protocol_packet * request,
                                       const struct protocol_packet * response)
{
    if(!request || !response || !response->_data)
    {
        error(FILE_LINE, "NULL ptr");
    }

    const uint8_t req_cmd = protocol_packet_command(request);
    const uint8_t resp_cmd = protocol_packet_command(response);

    if(PROTOCOL_ERROR == resp_cmd)
    {
        error(FILE_LINE, "Received an ERROR from downstream.");
    }
    else if(req_cmd != resp_cmd)
    {
        error(FILE_LINE, "Request/response mismatch.");
    }
}


uint8_t protocol_packet_module(const struct protocol_packet * pkt)
{
    // exract the 2 most significant bits
    return protocol_packet_command(pkt) & 0xC0;
}

/// @brief open a uart port for use with the protocol
/// @param uart_port_name - the name of the underlying uart port
/// @return a handle to the uart_port
const struct uart_port * protocol_open(const char uart_port_name[])
{
    return uart_open(uart_port_name, BAUD, UART_FLOW_NONE, UART_PARITY_NONE);
}

// initialize the stream portion of the packet without setting a command
static void protocol_packet_stream_init(struct protocol_packet * out)
{
    bytestream_init(&out->stream,
                    out->_data + HEADER_BYTES,
                    ARRAY_LEN(out->_data) - HEADER_BYTES);
}
void protocol_packet_init(struct protocol_packet * out,
                          uint8_t command)
{
    if(!out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    protocol_packet_stream_init(out);
    bytestream_inject_u8(&out->stream, command);
}

uint8_t protocol_packet_command(const struct protocol_packet * pkt)
{
    if(!pkt)
    {
        error(FILE_LINE, "NULL ptr");
    }
    // the command byte is the first byte after the header
    return pkt->_data[HEADER_BYTES];
}

void protocol_write_block(const struct uart_port * port,
                          struct protocol_packet * packet)
{
    if(!port || !packet)
    {
        error(FILE_LINE, "NULL ptr");
    }

    // write the packet header
    const uint8_t length = protocol_header_init(packet);

    const uint32_t timeout = length * TIMEOUT_MS_PER_BYTE + TIMEOUT_MS_BASE;
    (void)uart_write_block(port, packet->_data, length, timeout);
}

bool protocol_read_block_error(const struct uart_port * port,
                               struct protocol_packet * out,
                               uint32_t timeout,
                               bool timeout_error)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }

    struct protocol_packet temp = {0};
    if(!out)
    {
        out = &temp;
    }
    protocol_packet_stream_init(out);

    // read the header
    uart_read_block(port, &out->_data[0], HEADER_BYTES,
                    0 == timeout ? 0
                    : timeout + TIMEOUT_MS_PER_BYTE * HEADER_BYTES ,
                    UART_TERM_NONE);

    // ACK packets from the bootloader have a length of 0, so that length
    // does not include the header bytes. In all other packets, the length
    // does include the header bytes
    const uint32_t length = 0 == out->_data[LENGTH_INDEX] ?
        HEADER_BYTES : out->_data[LENGTH_INDEX];

    const uint32_t data_length = length - HEADER_BYTES;

    // read the rest of the packet
    const int read =
        uart_read_block_error(port,
                              &out->_data[HEADER_BYTES],
                              data_length,
                              TIMEOUT_MS_PER_BYTE * data_length + timeout,
                              UART_TERM_NONE,
                              timeout_error);

    if(read == 0)
    {
        return false;
    }
    else
    {
        protocol_verify_checksum(out, data_length);
        return true;
    }
}

void protocol_read_block(const struct uart_port * port,
                         struct protocol_packet * out,
                         uint32_t timeout)
{
    (void)protocol_read_block_error(port, out, timeout, true);
}

bool protocol_read_nonblock(const struct uart_port * port,
                            struct protocol_packet * out)
{
    if(!port || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    if(!uart_data_available(port))
    {
        return false;
    }

    protocol_read_block(port, out, TIMEOUT_MS_BASE);
    return true;
}

bool protocol_request_timeout(const struct uart_port * port,
                              struct protocol_packet * in,
                              struct protocol_packet * out,
                              uint32_t timeout_ms,
                              bool timeout_error)
{
    if(!port || !in)
    {
        error(FILE_LINE, "NULL ptr");
    }

    // used to store discarded packets
    static struct protocol_packet devnull = {0};
    if(!out)
    {
        out = &devnull;
    }

    protocol_write_block(port, in);

    const bool success =
        protocol_read_block_error(port, out, timeout_ms, timeout_error);

    if(success)
    {
        protocol_validate_response(in, out);
        return true;
    }
    else
    {
        if(timeout_error)
        {
            error(FILE_LINE, "timeout");
        }
        return false;
    }
}


void protocol_request(const struct uart_port * port,
                      struct protocol_packet * in,
                      struct protocol_packet * out)
{
    (void)protocol_request_timeout(port, in, out, TIMEOUT_MS_BASE, true);
}


void protocol_broadcast_timeout(const struct uart_port * const ports[],
                                unsigned int num_ports,
                                struct protocol_packet  pkt[],
                                struct protocol_packet response[],
                                enum protocol_broadcast_type btype,
                                uint32_t timeout)
{
    if(!pkt || !ports)
    {
        error(FILE_LINE, "NULL ptr");
    }

    if(num_ports > MAX_BROADCAST_PORTS || num_ports == 0)
    {
        error(FILE_LINE, "invalid number of ports");
    }

    // get the leng
    uint32_t out_length[MAX_BROADCAST_PORTS] = {0};
    const struct protocol_packet * outpack[MAX_BROADCAST_PORTS] = {NULL};

    out_length[0] = protocol_header_init(&pkt[0]);
    outpack[0] = &pkt[0];

    for(unsigned int i = 1; i < num_ports; ++i)
    {
        switch(btype)
        {
        case PROTOCOL_ANYCAST: // each packet sent is different
            out_length[i] = protocol_header_init(&pkt[i]);
            outpack[i] = &pkt[i];
            if(out_length[i] != out_length[0])
            {
                // packets must have the same length
                error(FILE_LINE, "packet length mismatch");
            }
            break;
        case PROTOCOL_BROADCAST: // each packet sent is the same
            out_length[i] = out_length[0];
            outpack[i] = outpack[0];
            break;
        default:
            error(FILE_LINE, "Unknown btype");
        }
    }

    if(out_length[0] > ARRAY_LEN(outpack[0]->_data) - HEADER_BYTES)
    {
        error(FILE_LINE, "packet data too long");
    }


    // TODO: there is an optimal point between how much data to send
    // per interleaved. for now interleave every byte
    for(uint32_t i = 0; i != out_length[0]; ++i)
    {
        for(unsigned int port = 0; port != num_ports; ++port)
        {
            uart_write_nonblock(ports[port], &outpack[port]->_data[i], 1);
        }
    }

    // garbage buffer if caller does not want a response
    struct protocol_packet in_packets[MAX_BROADCAST_PORTS];
    struct protocol_packet * in_packet = response ? response : in_packets;
    // initialize the packet streams
    for(unsigned int i = 0; i != MAX_BROADCAST_PORTS; ++i)
    {
        protocol_packet_stream_init(&in_packet[i]);
    }

    // length is temporarily 1 until we read the actual packet length
    uint8_t length[MAX_BROADCAST_PORTS] = {1, 1, 1};
    uint8_t index[MAX_BROADCAST_PORTS] = {0};

    // set to true once the true length was read
    bool length_set[MAX_BROADCAST_PORTS] = {false};

    // timeout in ms
    struct time_elapsed_ms elapsed = time_elapsed_ms_init();
    bool keep_looping = true;
    while(keep_looping)
    {
        // read up to 12 bytes at a time because the uart rx fifo is
        // 16 bytes.  This loop must always complete before the FIFO on the
        // other ports is full. Thus it is a balance between reading
        // as much as possible from one fifo before the other fifo fills up.
        // NOTE: this loop may need to be manually unrolled for performance
        // reasons
        for(unsigned int port = 0; port != num_ports; ++port)
        {
            index[port] += uart_read_nonblock(ports[port],
                                     &in_packet[port]._data[index[port]], 12);
        }

        // if we have read the length byte from the packet, then
        // set the packet's length
        for(unsigned int port = 0; port != num_ports; ++port)
        {
            if(!length_set[port] && index[port] > 0)
            {
                length[port] = in_packet[port]._data[LENGTH_INDEX];
                length_set[port] = true;
            }
        }

        if(time_elapsed_ms(&elapsed) > timeout)
        {
            error(FILE_LINE, "timeout");
        }

        // determine if we still must load more data
        keep_looping = false;
        for(unsigned int port = 0; port != num_ports; ++port)
        {
            keep_looping |= index[port] < length[port];
        }
    }

    // verify and validate the response
    for(unsigned int port = 0; port != num_ports; ++port)
    {
        if(length[port] < HEADER_BYTES)
        {
            // NOTE: for simplicity, broadcasting is incompatible with bootloader
            // therefore a length of 0 is never permissible, unlike when sending
            // a single packet.
            error(FILE_LINE, "invalid length");
        }

        // setup the packet data from the raw data and
        // verify that the checksum is correct
        protocol_verify_checksum(&in_packet[port], length[port] - HEADER_BYTES);
        protocol_validate_response(outpack[port], &in_packet[port]);
    }
}

void protocol_broadcast(const struct uart_port * const ports[],
                        unsigned int num_ports,
                        struct protocol_packet  pkt[],
                        struct protocol_packet response[],
                        enum protocol_broadcast_type btype)
{
    protocol_broadcast_timeout(ports, num_ports,
                               pkt, response, btype, PROTOCOL_TIMEOUT_DEFAULT);
}

void protocol_error_response(const struct uart_port * port)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }
    struct protocol_packet out = {0};
    protocol_packet_init(&out, PROTOCOL_ERROR);
    protocol_write_block(port, &out);
}

#include <string.h>
#include "common/protocol.h"
#include "common/error.h"
#include "common/uart.h"
#include "common/time.h"

/// protocol description:
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
#define LENGTH_BYTES 1

#define CHECKSUM_BYTES 1

#define COMMAND_BYTES 1

#define HEADER_BYTES (LENGTH_BYTES + CHECKSUM_BYTES)

/// max length of an error message
#define ERROR_MESSAGE_MAX_BYTES \
    (ARRAY_LEN(((struct protocol_packet*)0)->_data)   \
     - HEADER_BYTES - COMMAND_BYTES)

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
static void protocol_verify_checksum(struct protocol_packet * out, uint8_t data_length)
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

/// @brief open a uart port for use with the protocol
/// @param uart_port_name - the name of the underlying uart port
/// @return a handle to the uart_port
const struct uart_port * protocol_open(const char uart_port_name[])
{
    return uart_open(uart_port_name, BAUD, UART_FLOW_NONE, UART_PARITY_NONE);
}


void protocol_packet_init(struct protocol_packet * out,
                          uint8_t command)
{
    if(!out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    bytestream_init(&out->stream,
                    out->_data + HEADER_BYTES,
                    ARRAY_LEN(out->_data) - HEADER_BYTES);
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

void protocol_read_block(const struct uart_port * port,
                         struct protocol_packet * out,
                         uint32_t timeout)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }

    struct protocol_packet temp;
    if(!out)
    {
        out = &temp;
    }

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
    uart_read_block(port, &out->_data[HEADER_BYTES], data_length,
                    TIMEOUT_MS_PER_BYTE * data_length + timeout,
                    UART_TERM_NONE);
    protocol_verify_checksum(out, data_length);
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

void protocol_request_timeout(const struct uart_port * port,
                              struct protocol_packet * in,
                              struct protocol_packet * out,
                              uint32_t timeout_ms)
{
    if(!port || !in)
    {
        error(FILE_LINE, "NULL ptr");
    }

    // used to store discarded packets
    static struct protocol_packet devnull;
    if(!out)
    {
        out = &devnull;
    }

    protocol_write_block(port, in);
    protocol_read_block(port, out, timeout_ms);
    protocol_validate_response(port, in, out);
}


void protocol_request(const struct uart_port * port,
                      struct protocol_packet * in,
                      struct protocol_packet * out)
{
    protocol_request_timeout(port, in, out, TIMEOUT_MS_BASE);
}

void protocol_error_message(const struct uart_port * port,
                            const char * fileline,
                            const char * message)
{
    // send the error packet and wait for the response
    {
        struct protocol_packet error_indicate;
        protocol_packet_init(&error_indicate, PROTOCOL_ERROR);

        // send the error signal and ready for the response
        protocol_request(port, &error_indicate, NULL);
    }

    // upon receiving the response, send the string

    // concatenate the fileline and the message, subject to the
    // length constraint on the error_message buffer.
    char error_buf[ERROR_MESSAGE_MAX_BYTES] = ""; 
    const size_t max_len = ARRAY_LEN(error_buf) - 1; // minus 1 is for '\0'

    // copy to the buffer, ensuring the length is not exceeded
    size_t used_len = 0;
    strncpy(error_buf, fileline, max_len);
    used_len += strlen(fileline);
    if(used_len < max_len)
    {
        // add the space. note the rest of the buffer is already null
        // so no need to write the null character
        error_buf[used_len] = ' ';
        ++used_len;
        if(used_len < max_len)
        {
            strncpy(&error_buf[used_len], message, max_len - used_len);
        }
    }

    struct protocol_packet pkt;
    protocol_packet_init(&pkt, PROTOCOL_STRING);
    // store the message in the packet
    bytestream_inject_string(&pkt.stream, error_buf);

    // send the error message
    protocol_write_block(port, &pkt);
}

void protocol_validate_response(const struct uart_port * port,
                                const struct protocol_packet * request,
                                const struct protocol_packet * response)
{
    if(!port || !request || !response)
    {
        error(FILE_LINE, "NULL ptr");
    }
    if(protocol_packet_command(request) != protocol_packet_command(response))
    {
        if(PROTOCOL_ERROR == protocol_packet_command(response))
        {
            // we have received an error message.
            // acknowledge the error message
            {
                struct protocol_packet resp;
                protocol_packet_init(&resp, PROTOCOL_ERROR);
                protocol_write_block(port, &resp);
            }

            // wait to receive the actual error string
            struct protocol_packet msg_pkt;
            protocol_read_block(port, &msg_pkt, TIMEOUT_MS_BASE);
            if(protocol_packet_command(&msg_pkt) == PROTOCOL_STRING)
            {
                // the error message is stored LENGTHafter the command byte
                // as a NULL terminated string
                char msg[ERROR_MESSAGE_MAX_BYTES] = "";
                bytestream_extract_string(&msg_pkt.stream, msg, ARRAY_LEN(msg));
                error(FILE_LINE, msg);
            }
            else
            {
                error(FILE_LINE, "could not get error msg");
            }
        }
        else
        {
            error(FILE_LINE, "request/response mismatch");
        }
    }
}

void protocol_broadcast(const struct uart_port * const ports[],
                        struct protocol_packet  pkt[],
                        struct protocol_packet response[],
                        bool all,
                        uint32_t timeout)
{
    if(!pkt || !ports)
    {
        error(FILE_LINE, "NULL ptr");
    }

    // get the length and initialize the packet header
    uint32_t out_length = protocol_header_init(&pkt[0]);
    const struct protocol_packet * outpack[] = {pkt, pkt, pkt};
    if(!all)
    {
        const uint32_t length1 = protocol_header_init(&pkt[1]);
        const uint32_t length2 = protocol_header_init(&pkt[2]);
        if(out_length != length1 || length1 != length2)
        {
            // packets must be the same length
            error(FILE_LINE, "mismatched length");
        }
        outpack[1] = &pkt[1];
        outpack[2] = &pkt[2];
    }

    if(out_length > ARRAY_LEN(outpack[0]->_data) - HEADER_BYTES)
    {
        error(FILE_LINE, "packet data too long");
    }


    // TODO: there is an optimal point between interleaving each byte and
    // sending the bytes in a block
    for(uint32_t i = 0; i != out_length; ++i)
    {
        uart_write_nonblock(ports[0], &outpack[0]->_data[i], 1);
        uart_write_nonblock(ports[1], &outpack[1]->_data[i], 1);
        uart_write_nonblock(ports[2], &outpack[2]->_data[i], 1);
    }

    // garbage buffer if caller does not want a response
    struct protocol_packet in_packets[3];
    struct protocol_packet * in_packet = response ? response : in_packets;

    // length is temporarily 1 until we read the actual packet length
    uint8_t length0 = 1;
    uint8_t length1 = 1;
    uint8_t length2 = 1;

    // the index into each packet
    uint8_t index0 = 0;
    uint8_t index1 = 0;
    uint8_t index2 = 0;

    // set to true once the true length was read
    bool length0_set = false;
    bool length1_set = false;
    bool length2_set = false;

    // timeout in ms
    struct time_elapsed_ms elapsed = time_elapsed_ms_init();
    while(index0 < length0 || index1 < length1 || index2 < length2)
    {
        // read up to 8 bytes at a time because the uart rx fifo is
        // 16 bytes
        index0 += uart_read_nonblock(ports[0], &in_packet[0]._data[index0], 8);
        index1 += uart_read_nonblock(ports[1], &in_packet[1]._data[index1], 8);
        index2 += uart_read_nonblock(ports[2], &in_packet[2]._data[index2], 8);

        if(!length0_set && index0 > 0)
        {
            length0 = in_packet[0]._data[LENGTH_INDEX];
            length0_set = true;
        }
        if(!length1_set && index1 > 0)
        {
            length1 = in_packet[1]._data[LENGTH_INDEX];
            length1_set = true;
        }
        if(!length2_set && index2 > 0)
        {
            length2 = in_packet[2]._data[LENGTH_INDEX];
            length2_set = true;
        }

        if(time_elapsed_ms(&elapsed) > timeout)
        {
            error(FILE_LINE, "timeout");
        }
    }

    if(length0 < HEADER_BYTES
       || length1 < HEADER_BYTES
       || length2 < HEADER_BYTES)
    {
        // NOTE: for simplicity, broadcasting is incompatible with bootloader
        // therefore a length of 0 is never permissible, unlike when sending
        // a single packet.
        error(FILE_LINE, "invalid length");
    }

    // setup the packet data from the raw data and
    // verify that the checksum is correct
    protocol_verify_checksum(&in_packet[0], length0 - HEADER_BYTES);
    protocol_verify_checksum(&in_packet[1], length1 - HEADER_BYTES);
    protocol_verify_checksum(&in_packet[2], length2 - HEADER_BYTES);

    protocol_validate_response(ports[0], outpack[0], &in_packet[0]);
    protocol_validate_response(ports[1], outpack[1], &in_packet[1]);
    protocol_validate_response(ports[2], outpack[2], &in_packet[2]);

}

void protocol_inject_version(struct bytestream * bs,
                             const struct protocol_version * vs)
{
    if(!bs || !vs)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_string(bs, vs->id);
    bytestream_inject_u8(bs, vs->major);
    bytestream_inject_u8(bs, vs->minor);
}
/// @brief deserialize the protocol version
/// @param bs - the bytestream
/// @param vs - the version
void protocol_extract_version(struct bytestream * bs,
                              struct protocol_version * vs)
{
    if(!bs || !vs)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_extract_string(bs, vs->id, ARRAY_LEN(vs->id));
    vs->major = bytestream_extract_u8(bs);
    vs->minor = bytestream_extract_u8(bs);
}

/******************************************************************************
* ACNodeReader Firmware - Comms Protocol                                      *
*                                                                             *
* (c) 2020 London Hackspace                                                   *
* Released under a 3-clause BSD license. See included LICENSE file.           *
*                                                                             *
******************************************************************************/

#include "commsprotocol.h"
#include "tickcounter.h"

#include <string.h>
#include <stdbool.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#endif

static const unsigned int max_tx_payload = COMMS_TXBUFFER_SIZE - 5;

static unsigned char calculate_checksum(const unsigned char* data);

// Weak implementations of the message processors
void comms_default_message_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength)
{
    // This saves quite a bit of space in the bootloader!
//#ifndef BUILD_BOOTLOADER
    comms_send_unknown_message_reply(comms, code);
//#endif
}

#define DEFAULT_MESSAGE_HANDLER(M) void M (comms_context_t*, unsigned char, unsigned char*, size_t) __attribute__ ((weak, alias ("comms_default_message_handler")))

DEFAULT_MESSAGE_HANDLER(comms_query_reader_version_handler);
DEFAULT_MESSAGE_HANDLER(comms_reader_version_response_handler);
DEFAULT_MESSAGE_HANDLER(comms_log_message_handler);
DEFAULT_MESSAGE_HANDLER(comms_card_message_handler);
DEFAULT_MESSAGE_HANDLER(comms_reset_reader_handler);
DEFAULT_MESSAGE_HANDLER(comms_query_temperature_handler);
DEFAULT_MESSAGE_HANDLER(comms_temperature_response_handler);
DEFAULT_MESSAGE_HANDLER(comms_set_led_handler);
DEFAULT_MESSAGE_HANDLER(comms_query_bootloader_status_handler);
DEFAULT_MESSAGE_HANDLER(comms_bootloader_status_response_handler);
DEFAULT_MESSAGE_HANDLER(comms_bootloader_write_data_handler);

// default for this explicitly defined since we don't want to send "unknown" replies to unknown reply messages!
__attribute__ ((weak))
void comms_unknown_message_reply_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength)
{

}

enum comms_state
{
    STATE_IDLE = 0,
    STATE_WAIT_ACK = 1
};

enum message_nums
{
    MSG_QUERY_READER_VERSION = 0x01,
    MSG_QUERY_TEMPERATURE = 0x03,
    MSG_RESET_READER = 0x04,
    MSG_SET_LEDS = 0x05,
    MSG_QUERY_BOOTLOADER_STATUS = 0x20,
    MSG_WRITE_BOOTLOADER_DATA = 0x21,
    MSG_READER_VERSION_RESPONSE = 0x81,
    MSG_LOG = 0x82,
    MSG_TEMPERATURE_RESPONSE = 0x83,
    MSG_CARD = 0x86,
    MSG_BOOTLOADER_STATUS_RESPONSE = 0xa0,

    MSG_UNKNOWN_MESSAGE_REPLY_H2R = 0x7f,
    MSG_UNKNOWN_MESSAGE_REPLY_R2H = 0xff,
};

void comms_init(comms_context_t* comms)
{
    memset(comms, 0, sizeof(comms_context_t));
}

void comms_set_handlers(comms_context_t* comms, sendfunc send, recvfunc receive)
{
    comms->send = send;
    comms->receive = receive;
}

static void process_receive(comms_context_t* comms)
{
    if(comms->buffer_level < COMMS_RXBUFFER_SIZE)
    {
        unsigned int got = comms->receive(comms->rxbuffer+comms->buffer_level, COMMS_RXBUFFER_SIZE-comms->buffer_level);
        if(got > 0)
        {
            comms->buffer_level += got;
        }
    }
}

// remove some bytes from the middle (or end) of the rx buffer
static void close_gap(comms_context_t* comms, unsigned int start, unsigned int count)
{
    for(unsigned int i = start; i < comms->buffer_level-count; ++i)
    {
        comms->rxbuffer[i] = comms->rxbuffer[i+count];
    }
    comms->buffer_level -= count;
}

static void process_message(comms_context_t* comms, unsigned int start, unsigned int length)
{
    unsigned char checksum = calculate_checksum(comms->rxbuffer+start);

    unsigned char scratch[2];
    scratch[0] = 0xfd;
    if(checksum != comms->rxbuffer[start+length-1])
    {
        // send nak
        scratch[1] = 0x03;
    }
    else
    {
        scratch[1] = 0x02;
    }

    comms->send(scratch, 2);

    // If we just sent a NAK, there's no point continuing
    if(scratch[1] == 0x03)
    {
        return;
    }

    unsigned char code = comms->rxbuffer[start+2];
    unsigned char* payload = comms->rxbuffer+start+4;
    size_t payloadLength = length-5;

    switch(code)
    {
        case MSG_RESET_READER:
        {
            comms_reset_reader_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_QUERY_BOOTLOADER_STATUS:
        {   
            comms_query_bootloader_status_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_WRITE_BOOTLOADER_DATA:
        {
            comms_bootloader_write_data_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_SET_LEDS:
        {
            comms_set_led_handler(comms, code, payload, payloadLength);
        } break;
// If we're building the bootloader, skip some options to save a bit of code space
#ifndef BUILD_BOOTLOADER
        case MSG_QUERY_READER_VERSION:
        {
            comms_query_reader_version_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_QUERY_TEMPERATURE:
        {
            comms_query_temperature_handler(comms, code, payload, payloadLength);
        } break;      
// Similarly, if we're not building the host side, skip messages that are sent that way  
#ifdef COMMS_HOST_MODE
        case MSG_BOOTLOADER_STATUS_RESPONSE:
        {
            comms_bootloader_status_response_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_UNKNOWN_MESSAGE_REPLY_R2H:
        {

            comms_unknown_message_reply_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_TEMPERATURE_RESPONSE:
        {
            comms_temperature_response_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_READER_VERSION_RESPONSE:
        {
            comms_reader_version_response_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_LOG:
        {
            comms_log_message_handler(comms, code, payload, payloadLength);
        } break;
        case MSG_CARD:
        {
            comms_card_message_handler(comms, code, payload, payloadLength);
        } break;
#endif // COMMS_HOST_MODE
        case MSG_UNKNOWN_MESSAGE_REPLY_H2R:
        {
#ifdef COMMS_HOST_MODE
            // this is never valid in this direction
            comms_default_message_handler(comms, code, payload, payloadLength);
#else
            comms_unknown_message_reply_handler(comms, code, payload, payloadLength);
#endif
        } break;
#endif // ifndef BUILD_BOOTLOADER
        default:
        {
            comms_default_message_handler(comms, code, payload, payloadLength);
        }
    }
}

//void lights_set(int,int,int);

void comms_poll(comms_context_t* comms)
{
    process_receive(comms);

    // do we need to attempt a retransmission?
    if(comms->state == STATE_WAIT_ACK)
    {
        if(compare_timer_values(tickcounter_get(), comms->ackWaitCounter) >= 1000)
        {
            comms->send(comms->txbuffer, comms->lastmsglength);
            comms->ackWaitCounter = tickcounter_get();
            ++comms->retryCounter;
        }
    }

    // a single byte isn't much good to anyone
    if(comms->buffer_level < 2)
    {
        return;
    }

    unsigned int processed_to = 0;
    for(unsigned int i = 0; i < comms->buffer_level-1; ++i)
    {
        // Not a valid message start, Ack or Nak -> skip it
        if(comms->rxbuffer[i] != 0xff && comms->rxbuffer[i] != 0xfd)
        {
            processed_to = i+1;
            continue;
        }
        if(comms->rxbuffer[i] == 0xfd && comms->rxbuffer[i+1] == 0x02)
        {
            // Woohoo this is an ack!
            comms->state = STATE_IDLE;
            i += 1; // the loop will increment by one but we grabbed two bytes
            processed_to = i+2;
            comms->retryCounter = 0;
            continue;
        }
        else if(comms->rxbuffer[i] == 0xfd && comms->rxbuffer[i+1] == 0x03)
        {
            // Ah a NAK. Remove it from the buffer and resend the last message
            comms->send(comms->txbuffer, comms->lastmsglength);
            comms->ackWaitCounter = tickcounter_get();
            ++comms->retryCounter;
            i += 1; // the loop will increment by one but we grabbed two bytes
            processed_to = i+1;
            continue;
        }
        else if(comms->rxbuffer[i] == 0xff && comms->rxbuffer[i+1] == 0xdd)
        {
            // do we have enough to be a plausible message?
            if((comms->buffer_level-i) >= 5) // 5 is the shortest possible message
            {
                unsigned int length = 4+comms->rxbuffer[i+3];
                if((comms->buffer_level-i) >= length)
                {
                    process_message(comms, i, length);
                    processed_to = i + length;
                    // i gets incremented, so add length-1
                    i += length-1;
                    continue;
                }
                else
                {
                    // partial message
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    //Now we have processed as much as we can, remove anything already processed
    // shortcut for the case where we ate the entire buffer
    if(processed_to+1 >= comms->buffer_level)
    {
        comms->buffer_level = 0;
    }
    else
    {
        // remove the processed part from the buffer
        close_gap(comms, 0, processed_to);
    }
}

static void prepare_tx_buffer(comms_context_t* comms, unsigned char messagenum, unsigned int payloadLength)
{
    comms->txbuffer[0] = 0xff;
    comms->txbuffer[1] = 0xdd;
    comms->txbuffer[2] = messagenum;
    comms->txbuffer[3] = payloadLength+1; // include checksum in length
}

static unsigned char calculate_checksum(const unsigned char* data)
{
    unsigned char result = 0;

    for(unsigned int i = 0; i < data[3]+1; ++i)
    {
        result += data[2+i];
    }

    return (0xff-result)+1;
}

static void send_message(comms_context_t* comms)
{
    comms->lastmsglength = comms->txbuffer[3]+4;
    comms->send(comms->txbuffer, comms->lastmsglength);
    comms->state = STATE_WAIT_ACK;
    comms->ackWaitCounter = tickcounter_get();
    comms->retryCounter = 0;
}

static void comms_wait_for_idle(comms_context_t* comms)
{
    // make sure the buffer is ready - if not, keep trying
    while(comms->state != STATE_IDLE)
    {
        #ifdef __AVR__
        wdt_reset();
        #endif
        comms_poll(comms);
    }
}

void comms_send_log(comms_context_t* comms, int level, const char* context, const char* message, int length)
{
    #ifdef __AVR__
    size_t context_length = strlen_P(context);
    #else
    size_t context_length = strlen(context);
    #endif

    const int max_length = max_tx_payload - 3 - context_length; // leave space for size and level fields
    
    comms_wait_for_idle(comms);
    
    if(length > max_length)
    {
        length = max_length;
    }

    prepare_tx_buffer(comms, MSG_LOG, length+3+context_length);

    comms->txbuffer[4] = level;
    comms->txbuffer[5] = context_length;
    #ifdef __AVR__
    memcpy_P(comms->txbuffer+6, context, context_length);
    #else
    memcpy(comms->txbuffer+6, context, context_length);
    #endif
    comms->txbuffer[6+context_length] = length;
    memcpy(comms->txbuffer+7+context_length, message, length);
    comms->txbuffer[7+context_length+length] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_logz(comms_context_t* comms, int level, const char* context, const char* message)
{
    comms_send_log(comms, level, context, message, strlen(message));
}

#ifdef __AVR__
void comms_send_log_flash(comms_context_t* comms, int level, const char* context, const char* flashmessage, int length)
{
    size_t context_length = strlen_P(context);

    const int max_length = max_tx_payload - 3 - context_length; // leave space for size and level fields
    // make sure the buffer is ready - if not, keep trying
    comms_wait_for_idle(comms);
    
    if(length > max_length)
    {
        length = max_length;
    }

    prepare_tx_buffer(comms, MSG_LOG, length+3+context_length);

    comms->txbuffer[4] = level;
    comms->txbuffer[5] = context_length;
    memcpy_P(comms->txbuffer+6, context, context_length);
    comms->txbuffer[6+context_length] = length;
    memcpy_P(comms->txbuffer+7+context_length, flashmessage, length);
    comms->txbuffer[7+length+context_length] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_logz_flash(comms_context_t* comms, int level, const char* context, const char* flashmessage)
{
    comms_send_log_flash(comms, level, context, flashmessage, strlen_P(flashmessage));
}
#endif

void comms_send_card_message(comms_context_t* comms, unsigned char length, const unsigned char* uid)
{
    comms_wait_for_idle(comms);

    prepare_tx_buffer(comms, MSG_CARD, 1+length);
    comms->txbuffer[4] = length;
    memcpy(comms->txbuffer+5, uid, length);
    comms->txbuffer[5+length] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_temperature_query(comms_context_t* comms)
{
    comms_wait_for_idle(comms);

    prepare_tx_buffer(comms, MSG_QUERY_TEMPERATURE, 0);
    comms->txbuffer[4] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_set_leds(comms_context_t* comms, unsigned char r, unsigned char g, unsigned char b)
{
    comms_wait_for_idle(comms);

    prepare_tx_buffer(comms, MSG_SET_LEDS, 3);
    comms->txbuffer[4] = r;
    comms->txbuffer[5] = g;
    comms->txbuffer[6] = b;
    comms->txbuffer[7] = calculate_checksum(comms->txbuffer);
}

void comms_reset_reader(comms_context_t* comms)
{
    comms_wait_for_idle(comms);

    prepare_tx_buffer(comms, MSG_RESET_READER, 0);
    comms->txbuffer[4] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_temperature_response(comms_context_t* comms, unsigned char high, unsigned char low)
{
    comms_wait_for_idle(comms);
    prepare_tx_buffer(comms, MSG_TEMPERATURE_RESPONSE, 2);

    comms->txbuffer[4] = high;
    comms->txbuffer[5] = low;
    comms->txbuffer[6] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_bootloader_status_query(comms_context_t* comms)
{
    comms_wait_for_idle(comms);

    prepare_tx_buffer(comms, MSG_QUERY_BOOTLOADER_STATUS, 0);
    comms->txbuffer[4] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_bootloader_status_response(comms_context_t* comms, enum bootloaderstatus status)
{
    comms_wait_for_idle(comms);

    prepare_tx_buffer(comms, MSG_BOOTLOADER_STATUS_RESPONSE, 1);
    comms->txbuffer[4] = status;
    comms->txbuffer[5] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_unknown_message_reply(comms_context_t* comms, unsigned char messageCode)
{
    comms_wait_for_idle(comms);
    #ifdef COMMS_HOST_MODE
    prepare_tx_buffer(comms, MSG_UNKNOWN_MESSAGE_REPLY_H2R, 1);
    #else
    prepare_tx_buffer(comms, MSG_UNKNOWN_MESSAGE_REPLY_R2H, 1);
    #endif

    comms->txbuffer[4] = messageCode;
    comms->txbuffer[5] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

void comms_send_reader_version_response(comms_context_t* comms, int majorVersion, int minorVersion, const char* buildDate, const char* githash)
{
    comms_wait_for_idle(comms);
    #ifdef __AVR__
    int dateLength = strlen_P(buildDate);
    int hashLength = strlen_P(githash);
    #else
    int dateLength = strlen(buildDate);
    int hashLength = strlen(githash);
    #endif
    int length = 4 + dateLength + hashLength;
    prepare_tx_buffer(comms, MSG_READER_VERSION_RESPONSE, length);

    comms->txbuffer[4] = majorVersion;
    comms->txbuffer[5] = minorVersion;
    comms->txbuffer[6] = dateLength;
    comms->txbuffer[7+dateLength] = hashLength;

    #ifdef __AVR__
    memcpy_P(comms->txbuffer+7, buildDate, dateLength);
    memcpy_P(comms->txbuffer+8+dateLength, githash, hashLength);
    #else
    memcpy(comms->txbuffer+7, buildDate, dateLength);
    memcpy(comms->txbuffer+8+dateLength, githash, hashLength);
    #endif

    comms->txbuffer[8+dateLength+hashLength] = calculate_checksum(comms->txbuffer);
    send_message(comms);
}

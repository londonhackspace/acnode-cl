/******************************************************************************
* ACNodeReader Firmware - Comms Protocol                                      *
*                                                                             *
* (c) 2020 London Hackspace                                                   *
* Released under a 3-clause BSD license. See included LICENSE file.           *
*                                                                             *
******************************************************************************/

#ifndef COMMSPROTOCOL_H
#define COMMSPROTOCOL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Send/Receive function pointer definitions
typedef void(*sendfunc)(const unsigned char*,unsigned int);
typedef unsigned int(*recvfunc)(unsigned char*,unsigned int);


// Uncomment this to switch to host mode
//#define COMMS_HOST_MODE

#ifdef COMMS_HOST_MODE
#define COMMS_TXBUFFER_SIZE 192
#define COMMS_RXBUFFER_SIZE 256
#else
#define COMMS_TXBUFFER_SIZE 192
#ifdef BUILD_BOOTLOADER
#define COMMS_RXBUFFER_SIZE 256
#else
#define COMMS_RXBUFFER_SIZE 64
#endif
#endif

typedef struct
{
    sendfunc send;
    recvfunc receive;

    int state;

    // For handling outgoing message assembly
    unsigned char txbuffer[COMMS_TXBUFFER_SIZE];
    unsigned int lastmsglength;

    // For handling incoming messages
    unsigned char rxbuffer[COMMS_RXBUFFER_SIZE];
    unsigned int buffer_level;

    // counter to handle how many polls we've been waiting for a response
    unsigned long int ackWaitCounter;
    unsigned int retryCounter;
} comms_context_t;

enum loglevels
{
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARN = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_DEBUG  = 3,
};

enum bootloaderstatus
{
    BOOTLOADER_STATUS_NOT_BOOTLOADER = 0,
    BOOTLOADER_STATUS_IDLE = 1,
    BOOTLOADER_STATUS_BUSY = 2,
};

void comms_init(comms_context_t* comms);
void comms_set_handlers(comms_context_t* comms, sendfunc send, recvfunc receive);
void comms_poll(comms_context_t* comms);

void comms_send_unknown_message_reply(comms_context_t* comms, unsigned char messageCode);

// on AVR, buildDate and githash are assumed to be in PROGMEM
void comms_send_reader_version_response(comms_context_t* comms, int majorVersion, int minorVersion, const char* buildDate, const char* githash);

// On AVR, context is assumed to always be in PROGMEM as used by the logging macros
void comms_send_log(comms_context_t* comms, int level, const char* context, const char* message, int length);
void comms_send_logz(comms_context_t* comms, int level, const char* context, const char* message);

#ifdef __AVR__
void comms_send_log_flash(comms_context_t* comms, int level, const char* context, const char* flashmessage, int length);
void comms_send_logz_flash(comms_context_t* comms, int level, const char* context, const char* flashmessage);
#endif

void comms_send_card_message(comms_context_t* comms, unsigned char length, const unsigned char* uid);

void comms_send_temperature_query(comms_context_t* comms);
void comms_send_temperature_response(comms_context_t* comms, unsigned char high, unsigned char low);

void comms_send_set_leds(comms_context_t* comms, unsigned char r, unsigned char g, unsigned char b);

void comms_send_bootloader_status_query(comms_context_t* comms);
void comms_send_bootloader_status_response(comms_context_t* comms, enum bootloaderstatus status);

// Implement any of these you care about handling - weak implementations are provided by default
void comms_query_reader_version_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);
void comms_reader_version_response_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_query_temperature_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);
void comms_temperature_response_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_set_led_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_reset_reader(comms_context_t* comms);
void comms_reset_reader_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_log_message_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_card_message_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_unknown_message_reply_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);

void comms_query_bootloader_status_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);
void comms_bootloader_status_response_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);
void comms_bootloader_write_data_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength);


#ifdef __cplusplus
}
#endif

#endif

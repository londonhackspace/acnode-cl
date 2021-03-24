#include "serialreader.h"

#include "commsprotocol.h"

#include <Energia.h>

static comms_context_t ctx;

static SerialReader* serialReader = nullptr;

extern "C" void scream(const char* msg)
{
	Serial.println(msg);
}

static String readString(uint8_t* ptr)
{
	// the String class of Arduino has no length-based constructor
	// so we need to make a c string first
	uint8_t len = ptr[0];
	char* tmp = (char*)calloc(len+1, 1);

	memcpy(tmp, ptr+1, len);
	String ret(tmp);
	free(tmp);

	return ret;
}

void comms_log_message_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength)
{
	uint8_t level = payload[0];

	String context = readString(payload+1);
	String msg = readString(payload+payload[1]+2);
	Serial.print("Reader: ");
	switch(level)
	{
		case 0:
			Serial.print("[ERROR] ");
			break;
		case 1:
			Serial.print("[WARNING] ");
			break;
		case 2:
			Serial.print("[INFO] ");
			break;
		case 3:
			Serial.print("[DEBUG] ");
			break;
		default:
			Serial.print("[?????] ");
	}

	Serial.print(context);
	Serial.print(" :: ");
	Serial.println(msg);
}

void comms_card_message_handler(comms_context_t* comms, unsigned char code, unsigned char* payload, size_t payloadLength)
{
	if(payload[0] == 8)
	{
		// ignore 8-byte card for now
		serialReader->onCard(CardId());
	}
	else if(payload[0] == 0)
	{
		serialReader->onCard(CardId());
	}
	else
	{
		CardId c(payload+1, payload[0]);
		serialReader->onCard(c);
	}
}

SerialReader::SerialReader()
{
	comms_init(&ctx);
	comms_set_handlers(&ctx, SerialReader::sendData, SerialReader::receiveData);
	serialReader = this;
}

SerialReader::~SerialReader()
{

}

CardId SerialReader::getCard()
{
	return lastCard;
}

void SerialReader::init()
{
	Serial6.setBufferSize(2048, 2048);
	Serial6.begin(115200);

	//comms_reset_reader(&ctx);
}

void SerialReader::set(uint8_t r, uint8_t g, uint8_t b)
{
	Serial.println("UPDATING REMOTE LED");
	comms_send_set_leds(&ctx, r, g, b);
}

void SerialReader::poll()
{
	comms_poll(&ctx);
	return;
	static int lastPoll = millis();
	static int pollCount = 0;
	++pollCount;
	if(pollCount == 5000)
	{
		Serial.print("Serial polled every ");
		Serial.print((millis()-lastPoll)/5000);
		Serial.println(" ms");
		Serial.print("Buffer level is ");
		Serial.print(ctx.buffer_level);
		Serial.print(" bytes and ");
		Serial.print(Serial6.available());
		Serial.print(" available. State is ");
		Serial.println(ctx.state);
		Serial.flush();
		pollCount = 1;
		lastPoll = millis();
	}
}

void SerialReader::sendData(const unsigned char* buf,unsigned int len)
{
	Serial6.write(buf, len);
	Serial6.flush();
}

unsigned int SerialReader::receiveData(unsigned char* buf,unsigned int len)
{
	Serial6.flush();

	unsigned int outlen = 0;

	while(Serial6.available() && len > 0)
	{
		*buf = Serial6.read();
		++buf;
		++outlen;
		--len;
	}

	/*unsigned int available = Serial6.available();
	len = min(len, available);

	if(len == 0)
	{
		return 0;
	}

	Serial6.readBytes(buf, len);*/

	return outlen;
}
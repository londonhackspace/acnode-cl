
#include "PN532_HSU.h"
#include "PN532_debug.h"

volatile int was_intr;
volatile int low_count;
volatile int high_count;
int intr_pin;

PN532_HSU::PN532_HSU(HardwareSerial &serial)
{
    _serial = &serial;
    _pin = -1;
    intr_pin = _pin;
    command = 0;
    was_intr = 0;
}

// we were using PD_2
PN532_HSU::PN532_HSU(HardwareSerial &serial, int pin)
{
    _serial = &serial;
    _pin = pin;
    intr_pin = pin;
    command = 0;
    was_intr = 0;
}

/*
 * on the Cooqrobot board the IRQ pin is the P70_IRQ one on the PN532
 * IRQ goes low?
 *
 */
void isr(void) {
    was_intr = 1;
    
    if (digitalRead(intr_pin)) {
      high_count++;
    } else {
      low_count++;
    }
}

void PN532_HSU::intr_check() {
  Serial.println("checking...");
  if (was_intr) {
    Serial.print("was inturupted: ");
    Serial.print(high_count);
    Serial.print(" / ");
    Serial.println(low_count);
    was_intr = 0;
  }
}

void PN532_HSU::begin()
{
    _serial->begin((long)115200);
    low_count = 0;
    high_count = 0;
    was_intr = 1;
    if (_pin != -1) {
      // or not a pullup?
      pinMode(_pin, INPUT_PULLUP);
      // should be falling (high -> low).
      attachInterrupt(_pin, isr, CHANGE);
    }
}

void PN532_HSU::zz_wakeup()
{
    _serial->write((uint8_t)0x55);
    _serial->write((uint8_t)0x55);
    _serial->write((uint8_t)0x00);
    _serial->write((uint8_t)0x00);
    _serial->write((uint8_t)0x00);

    /** dump serial buffer */
    if(_serial->available()){
        DMSG("Dump serial buffer: ");
    }
    while(_serial->available()){
        uint8_t ret = _serial->read();
        DMSG_HEX(ret);
    }

}

int8_t PN532_HSU::writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{

    /** dump serial buffer */
    if(_serial->available()){
        DMSG("Dump serial buffer: ");
    }
    while(_serial->available()){
        uint8_t ret = _serial->read();
        DMSG_HEX(ret);
    }

    DMSG_STR("");

    // for some reason this is what fixes it:
    // maybe there is a setting to stop it sleeping or something?
    zz_wakeup();

    command = header[0];
    
    _serial->write((uint8_t)PN532_PREAMBLE);
    DMSG_HEX(PN532_PREAMBLE);
    _serial->write((uint8_t)PN532_STARTCODE1);
    DMSG_HEX(PN532_STARTCODE1);
    _serial->write((uint8_t)PN532_STARTCODE2);
    DMSG_HEX(PN532_STARTCODE2);
    
    uint8_t length = hlen + blen + 1;   // length of data field: TFI + DATA
    _serial->write(length);
    DMSG_HEX(length);
    _serial->write((uint8_t)~length + 1);         // checksum of length
    DMSG_HEX((uint8_t)~length + 1);
    
    _serial->write((uint8_t)PN532_HOSTTOPN532);
    DMSG_HEX(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;    // sum of TFI + DATA
    
    _serial->write(header, hlen);
    for (uint8_t i = 0; i < hlen; i++) {
        sum += header[i];
        DMSG_HEX(header[i]);
    }

    _serial->write(body, blen);
    for (uint8_t i = 0; i < blen; i++) {
        sum += body[i];
        DMSG_HEX(body[i]);
    }
    
    uint8_t checksum = ~sum + 1;            // checksum of TFI + DATA
    _serial->write(checksum);
    DMSG_HEX(checksum);
    _serial->write((uint8_t)PN532_POSTAMBLE);
    DMSG_HEX((uint8_t)PN532_POSTAMBLE);

    DMSG_STR("");

    DMSG_STR("about to readackframe");

    return readAckFrame();
}

int16_t PN532_HSU::readResponse(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    uint8_t tmp[3];
    
    DMSG_STR("Read response");
    
    /** Frame Preamble and Start Code */
    if(receive(tmp, 3, timeout)<=0){
        return PN532_TIMEOUT;
    }
    if(0 != tmp[0] || 0 != tmp[1] || 0xFF != tmp[2]){
        DMSG("Preamble error");
        return PN532_INVALID_FRAME;
    }
    
    /** receive length and check */
    uint8_t length[2];
    if(receive(length, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(length[0] + length[1]) ){
        DMSG("Length error");
        return PN532_INVALID_FRAME;
    }
    length[0] -= 2;
    if( length[0] > len){
        return PN532_NO_SPACE;
    }
    
    /** receive command byte */
    uint8_t cmd = command + 1;               // response command
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( PN532_PN532TOHOST != tmp[0] || cmd != tmp[1]){
        DMSG("Command error");
        return PN532_INVALID_FRAME;
    }
    
    if(receive(buf, length[0], timeout) != length[0]){
        return PN532_TIMEOUT;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for(uint8_t i=0; i<length[0]; i++){
        sum += buf[i];
    }
    
    /** checksum and postamble */
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(sum + tmp[0]) || 0 != tmp[1] ){
        DMSG("Checksum error");
        return PN532_INVALID_FRAME;
    }

    return length[0];
}

int8_t PN532_HSU::readAckFrame()
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];
    
    DMSG_STR("Read Ack");
    
    if( receive(ackBuf, sizeof(PN532_ACK)) <= 0 ){
        DMSG("Read ACK Timeout\n");
        return PN532_TIMEOUT;
    }
    
    if( memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) ){
        DMSG("Invalid ACK\n");
        return PN532_INVALID_ACK;
    }
    return 0;
}

/**
    @brief receive data .
    @param buf --> return value buffer.
           len --> length expect to receive.
           timeout --> time of reveiving
    @retval number of received bytes, 0 means no data received.
*/
int8_t PN532_HSU::receive(uint8_t *buf, int len, uint16_t timeout)
{
  int read_bytes = 0;
  int ret = -1;
  unsigned long start_millis;

  while (read_bytes < len) {
    start_millis = millis();
    ret = -1;
    do {
      if (_serial->available() > 0) {
        ret = _serial->read();
      }
      if (ret >= 0) {
        break;
      }
      delay(1);
    } while( (millis()- start_millis ) < timeout);

    if (ret >= 0) {
      DMSG_HEX(ret);
    }

    if (ret < 0) {
        if (read_bytes) {
            return read_bytes;
        } else {
            return PN532_TIMEOUT;
        }
    }
    buf[read_bytes] = (uint8_t)ret;
    read_bytes++;
  }
  DMSG_STR("");
  return read_bytes;
}

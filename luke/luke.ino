/*
  Toobit interface firmware for Arudino
  HID API is based on Advanced RawHID example released by NicoHood
*/

#include "HID-Project.h"
#include "attribute.h"

//#define DEBUG_SERIAL_LOG

const int pinLed = LED_BUILTIN;
const int analogIn = 8;

uint8_t rawhidData[64];

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(analogIn, INPUT);

  Serial.begin(115200);

  RawHID.begin(rawhidData, sizeof(rawhidData));

  digitalWrite(pinLed, LOW);
}

void loop() {

  unsigned char txDataBuffer[63];

  // Check if there is new data from host
  auto bytesAvailable = RawHID.available();
  if (bytesAvailable)
  {
#ifdef DEBUG_SERIAL_LOG
    Serial.print("bytesAvailable: ");
    Serial.print(bytesAvailable);
#endif

    // Receive a packet
    uint8_t header = RawHID.read();
    uint8_t pcktVer = header & 0xC0;
    uint8_t pcktLen = header & 0x2F;
    if (pcktVer == PROTOCOL_VERSION && pcktLen > 1) {
#ifdef DEBUG_SERIAL_LOG
      Serial.print(", header: ");
      Serial.print(header);
#endif

      uint8_t opcode = RawHID.read();
      txDataBuffer[1] = opcode;  // echo back operation code
#ifdef DEBUG_SERIAL_LOG
      Serial.print(", opcode: ");
      Serial.print(opcode);
#endif

      if (opcode == OP_ATT_VALUE_GET) {

        ATTID id = (RawHID.read() << 8) + RawHID.read();
        txDataBuffer[2] = RC_OK;  // Return OK code
        txDataBuffer[0] = PROTOCOL_VERSION;
#ifdef DEBUG_SERIAL_LOG
        Serial.print(", id: ");
        Serial.print(id);
#endif

        if (id == ATT_PRODUCT_NAME) {

          // Set sending packet
          txDataBuffer[0] |= strcopy_from_PROGMEM(&(txDataBuffer[3]), PRODUCT_NAME) + 3; // packet length

        } else if (id == ATT_PRODUCT_REVISION) {

          // Set sending packet
          txDataBuffer[0] |= strcopy_from_PROGMEM(&txDataBuffer[3], PRODUCT_REVISION) + 3; // packet length

        } else if (id == ATT_PRODUCT_SERIAL) {

          // Set sending packet
          txDataBuffer[0] |= strcopy_from_PROGMEM(&txDataBuffer[3], PRODUCT_SERIAL) + 3; // packet length

        } else if (id == ATT_FIRM_VERSION) {

          // Set sending packet
          txDataBuffer[0] |= strcopy_from_PROGMEM(&txDataBuffer[3], FIRM_VERSION) + 3; // packet length

        } else if (id == ATT_VOLTAGE) {

          txDataBuffer[0] |= 4 + 3; // packet length
          float volt = get_voltage();
          memcpy(&txDataBuffer[3], &volt, 4);

        } else if (id == ATT_CURRENT) {

          txDataBuffer[0] |= 4 + 3; // packet length
          float curr = get_current();
          memcpy(&txDataBuffer[3], &curr, 4);

        } else {
          txDataBuffer[0] |= 3;  // packet length
          txDataBuffer[2] = RC_FAIL;  // Return error code9
        }

        // end of if (opcode == OP_ATT_VALUE_GET)
      } else if (opcode == OP_ATT_VALUE_SET) {

        ATTID id = (RawHID.read() << 8) + RawHID.read();
        txDataBuffer[2] = RC_OK;  // Return OK code
        txDataBuffer[0] = PROTOCOL_VERSION;
#ifdef DEBUG_SERIAL_LOG
        Serial.print(", id: ");
        Serial.print(id);
#endif

      } else {

        txDataBuffer[2] = RC_FAIL;  // Return error code

      }
    }

    bytesAvailable = RawHID.available();
    while (bytesAvailable--) {
      RawHID.read();
    }
#ifdef DEBUG_SERIAL_LOG
    Serial.println("");
#endif

    // Send response
    RawHID.write(txDataBuffer, sizeof(txDataBuffer));

  }
}

float get_voltage()
{
  float volt = analogRead(analogIn);
  return volt / 1024.0 * 5.0;
}

float get_current()
{
  return 0.0;  // Not support
}




/*
  Toobit interface firmware for Arudino
  HID API is based on Advanced RawHID example released by NicoHood
*/

#include "HID-Project.h"
#include "attribute.h"

//#define DEBUG_SERIAL_LOG

const int pinLed = LED_BUILTIN;

uint8_t rawhidData[64];
uint8_t portCtrl[4];

void setup() {
  pinMode(pinLed, OUTPUT);

  Serial.begin(115200);

  RawHID.begin(rawhidData, sizeof(rawhidData));

  portCtrl[0] = 0x01;
  portCtrl[1] = 0x00;
  portCtrl[2] = 0x00;
  portCtrl[3] = 0x00;
  digitalWrite(pinLed, HIGH);
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
          txDataBuffer[0] |= strcopy_from_PROGMEM(&(txDataBuffer[3]), PRODUCT_REVISION) + 3; // packet length
            
        } else if (id == ATT_PRODUCT_SERIAL) {

          // Set sending packet
          txDataBuffer[0] |= strcopy_from_PROGMEM(&(txDataBuffer[3]), PRODUCT_SERIAL) + 3; // packet length

        } else if (id == ATT_FIRM_VERSION) {

          // Set sending packet
          txDataBuffer[0] |= strcopy_from_PROGMEM(&(txDataBuffer[3]), FIRM_VERSION) + 3; // packet length

        } else if (id == ATT_USB_PORT_CTRL) {

          txDataBuffer[0] |= 4 + 3; // packet length
          txDataBuffer[3] = portCtrl[0];
          txDataBuffer[4] = portCtrl[1];
          txDataBuffer[5] = portCtrl[2];
          txDataBuffer[6] = portCtrl[3];

        }

        else {
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

        if (id == ATT_USB_PORT_CTRL) {

          portCtrl[0] = RawHID.read();
          if (portCtrl[0] & 0x01)
            digitalWrite(pinLed, HIGH);
          else
            digitalWrite(pinLed, LOW);
          //portCtrl[1] = RxDataBuffer[5];
          //portCtrl[2] = RxDataBuffer[6];
          //portCtrl[3] = RxDataBuffer[7];

        }

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


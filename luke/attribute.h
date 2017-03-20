#ifndef ATTRIBUTE_H__
#define  ATTRIBUTE_H__

#include <avr/pgmspace.h>

#define VALUE_LEN 32

#define PROTOCOL_VERSION 0x00  // 7-6bits is effective

#define ATTID uint16_t

typedef enum
{
  // Operation Code
  OP_METADATA_GET = 0x10,
  OP_CONFIG_SET,
  OP_ATT_VALUE_SET,
  OP_ATT_VALUE_GET,
  // Event Code
  EVT_NOTIFY = 0xA0
} OperationCode;

typedef enum
{
  // Return Code
  RC_FAIL = 0x0,
  RC_OK   = 0x1,
} ReturnCode;

typedef enum
{
  // Toolbit common attribute ID
  ATT_PRODUCT_NAME     = 0x0000,
  ATT_PRODUCT_REVISION = 0x0001,
  ATT_PRODUCT_SERIAL   = 0x0002,
  ATT_FIRM_VERSION     = 0x0003,

  // Platform commom attribute ID
  ATT_I2C0_ADDR        = 0x1030,
  ATT_I2C0_RW_1BYTE    = 0x1031,
  ATT_I2C0_RW_2BYTE    = 0x1032,

  // Product specific attribute ID
  ATT_VOLTAGE_RANGE = 0x8100,
  ATT_VOLTAGE       = 0x8101,
  ATT_CURRENT_RANGE = 0x8102,
  ATT_CURRENT       = 0x8103
} AttributionID;

/*
  struct Attribution{
    uint16_t id;
    uint8_t permission;
    uint8_t property;
    uint8_t *value;
  };
*/

const char PRODUCT_NAME[] PROGMEM     = {"Luke"};
const char PRODUCT_REVISION[] PROGMEM = {"0.1"};
const char PRODUCT_SERIAL[] PROGMEM   = {"SN0001"};
const char FIRM_VERSION[] PROGMEM     = {"0.1"};

uint8_t strcopy_from_PROGMEM(unsigned char *dst, const char *str)
{
  uint8_t len = strlen_P(str) + 1;  // +1 for NULL
  for (uint8_t k = 0; k < len; k++)
    *dst++ = pgm_read_byte_near(str + k);
  return len;
}


#endif  /* ATTRIBUTE_H__ */


/*!
 *  @file Adafruit_LC709203F.h
 *
 * 	I2C Driver for the Adafruit LC709203F Battery Monitor
 *
 * 	This is a library for the Adafruit LC709203F breakout:
 * 	https://www.adafruit.com/products/xxxx
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_LC709203F_H
#define _ADAFRUIT_LC709203F_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define LC709203F_I2CADDR_DEFAULT 0x0B   ///< LC709203F default i2c address
#define LC709203F_CMD_THERMISTORB   0x06   ///< Read/write thermistor B
#define LC709203F_CMD_CELLVOLTAGE   0x09   ///< Read batt voltage
#define LC709203F_CMD_CELLITE   0x0F   ///< Read batt indicator to empty
#define LC709203F_CMD_ICVERSION   0x11   ///< Read IC version

static uint8_t crc8(uint8_t *data, int len);


typedef enum {
  LC709203F_CURRENTDIRECTION_AUT0 = 0x0000,
  LC709203F_CURRENTDIRECTION_CHARGE = 0x0001,
  LC709203F_CURRENTDIRECTION_DISCHARGE = 0xFFFF,
} lc709203_currentdirection_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the LC709203F I2C battery monitor
 */
class Adafruit_LC709203F {
public:
  Adafruit_LC709203F();
  ~Adafruit_LC709203F();

  bool begin(TwoWire *wire = &Wire);
  uint16_t getICversion(void);
  float cellVoltage(void);
  float cellPercent(void);
  uint16_t getThermistorB(void);
  bool setThermistorB(uint16_t b);

protected:
  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  bool readWord(uint8_t address, uint16_t *data);
  bool writeWord(uint8_t command, uint16_t data);

};

#endif

/*!
 *  @file Adafruit_LC709203F.cpp
 *
 *  @mainpage Adafruit LC709203F Battery Monitor library
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Adafruit LC709203F Battery Monitor library
 *
 * 	This is a library for the Adafruit LC709203F breakout:
 * 	https://www.adafruit.com/product/4712
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"

#include "Adafruit_LC709203F.h"

/*!
 *    @brief  Instantiates a new LC709203F class
 */
Adafruit_LC709203F::Adafruit_LC709203F(void) {}

Adafruit_LC709203F::~Adafruit_LC709203F(void) {}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @param  sensor_id
 *            The unique ID to differentiate the sensors from others
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_LC709203F::begin(TwoWire *wire) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(LC709203F_I2CADDR_DEFAULT, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  getICversion();
  return true;
}


uint16_t Adafruit_LC709203F::getICversion(void) {
  uint16_t vers = 0;
  readWord(LC709203F_CMD_ICVERSION, &vers);
  return vers;
}


float Adafruit_LC709203F::cellVoltage(void) {
  uint16_t voltage = 0;
  readWord(LC709203F_CMD_CELLVOLTAGE, &voltage);
  return voltage / 1000.0;
}


float Adafruit_LC709203F::cellPercent(void) {
  uint16_t percent = 0;
  readWord(LC709203F_CMD_CELLITE, &percent);
  return 100 - (percent / 10.0);
}

uint16_t Adafruit_LC709203F::getThermistorB(void) {
  uint16_t val = 0;
  readWord(LC709203F_CMD_THERMISTORB, &val);
  return val;
}


bool Adafruit_LC709203F::setThermistorB(uint16_t b) {
  return writeWord(LC709203F_CMD_THERMISTORB, b);
}


/*!
 *    @brief  Helper that reads 16 bits of CRC data from the chip. Note
 *            this function performs a CRC on data that includes the I2C
 *            write address, command, read address and 2 bytes of response
 */
bool Adafruit_LC709203F::readWord(uint8_t command, uint16_t *data) {
  uint8_t reply[6];
  reply[0] = LC709203F_I2CADDR_DEFAULT * 2;  // write byte
  reply[1] = command;         // command / register
  reply[2] = reply[0] | 0x1;  // read byte

  if (! i2c_dev->write_then_read(&command, 1, reply+3, 3)) {
    return false;
  }

  uint8_t crc = crc8(reply, 5);
  // CRC failure?
  if (crc != reply[5]) return false;

  *data = reply[4];
  *data <<= 8;
  *data |= reply[3];

  return true;
}


/*!
 *    @brief  Helper that writes 16 bits of CRC data to the chip. Note
 *            this function performs a CRC on data that includes the I2C
 *            write address, command, and 2 bytes of response
 */
bool Adafruit_LC709203F::writeWord(uint8_t command, uint16_t data) {
  uint8_t send[5];
  send[0] = LC709203F_I2CADDR_DEFAULT * 2;  // write byte
  send[1] = command;         // command / register
  send[2] = data & 0xFF;
  send[3] = data >> 8;
  send[4] = crc8(send, 4);

  return i2c_dev->write(send+1, 5);
}


/**
 * Performs a CRC8 calculation on the supplied values.
 *
 * @param data  Pointer to the data to use when calculating the CRC8.
 * @param len   The number of bytes in 'data'.
 *
 * @return The computed CRC8 value.
 */
static uint8_t crc8(uint8_t *data, int len) {
  const uint8_t POLYNOMIAL(0x07);
  uint8_t crc(0x00);

  for (int j = len; j; --j) {
    crc ^= *data++;

    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

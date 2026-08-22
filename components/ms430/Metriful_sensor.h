/*
  Metriful_sensor.h

  This file declares functions and settings which are used in the code
  examples. The function definitions are in file Metriful_sensor.cpp
  Refactored for esp-idf.

  Copyright 2020-2023 Metriful Ltd.
  Licensed under the MIT License - for further details see LICENSE.txt

  For code examples, datasheet and user guide, visit
  https://github.com/metriful/sensor
*/

#ifndef METRIFUL_SENSOR_H
#define METRIFUL_SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "sensor_constants.h"
#include "esphome/components/i2c/i2c.h"

// Un-comment the following line to display temperatures in Fahrenheit
// else they will be in Celsius
//#define USE_FAHRENHEIT

// Specify which particle sensor is connected:
#define PARTICLE_SENSOR PARTICLE_SENSOR_OFF
// Define PARTICLE_SENSOR as:
//    PARTICLE_SENSOR_PPD42    for the Shinyei PPD42
//    PARTICLE_SENSOR_SDS011   for the Nova SDS011
//    PARTICLE_SENSOR_OFF      if no sensor is connected

// The I2C address of the MS430 board. 
#define I2C_ADDRESS I2C_ADDR_7BIT_SB_OPEN
// The default is I2C_ADDR_7BIT_SB_OPEN and must be changed to 
// I2C_ADDR_7BIT_SB_CLOSED if the solder bridge SB1 on the board 
// is soldered closed

/////////////////////////////////////////////////////////////////////

// The Arduino Wire library has a limited internal buffer size:
#define ARDUINO_WIRE_BUFFER_LIMIT_BYTES 32

#define I2C_CLK_FREQ_HZ 100000
#define SERIAL_BAUD_RATE 9600

// Unicode symbol strings
#define CELSIUS_SYMBOL "°C"
#define FAHRENHEIT_SYMBOL "°F"
#define SDS011_UNIT_SYMBOL "µg/m³"
#define SUBSCRIPT_2 "₂"
#define OHM_SYMBOL "Ω"

extern volatile bool ready_assertion_event; 

typedef struct
{
  float SPL_dBA;
  float SPL_bands_dB[SOUND_FREQ_BANDS];
  float peakAmp_mPa;
  bool  stable;
} SoundData_F_t;

typedef struct
{
  float    T_C;
  uint32_t P_Pa;
  float    H_pc;
  uint32_t G_Ohm;
} AirData_F_t;

typedef struct
{
  float   AQI;
  float   CO2e;
  float   bVOC;
  uint8_t AQI_accuracy;
} AirQualityData_F_t;

typedef struct
{
  float    illum_lux;
  uint16_t white;
} LightData_F_t;

typedef struct
{
  float duty_cycle_pc;
  float concentration;
  bool valid;
} ParticleData_F_t;

typedef enum
{
  OFF    = PARTICLE_SENSOR_OFF,
  PPD42  = PARTICLE_SENSOR_PPD42,
  SDS011 = PARTICLE_SENSOR_SDS011
} ParticleSensor_t;

typedef struct
{
  const char * variableName;
  const char * measurementUnit;
  int32_t thresHigh;
  int32_t thresLow;  
  uint16_t inactiveCount;
  const char * adviceHigh;
  const char * adviceLow;
} ThresholdSetting_t;

typedef struct
{
  const char * name;
  const char * unit;
  const char * icon;
  uint8_t decimalPlaces;
} HA_Attributes_t;

bool TransmitI2C(esphome::i2c::I2CDevice *device, uint8_t commandRegister,
                 const uint8_t * data, uint8_t data_length);
bool ReceiveI2C(esphome::i2c::I2CDevice *device, uint8_t commandRegister,
                uint8_t data[], uint8_t data_length);

const char * interpret_AQI_accuracy(uint8_t AQI_accuracy_code);
const char * interpret_AQI_accuracy_brief(uint8_t AQI_accuracy_code);
const char * interpret_AQI_value(uint16_t AQI);

void convertAirDataF(const AirData_t * airData_in, AirData_F_t * airDataF_out);
void convertAirQualityDataF(const AirQualityData_t * airQualityData_in, 
                            AirQualityData_F_t * airQualityDataF_out);
void convertLightDataF(const LightData_t * lightData_in,
                       LightData_F_t * lightDataF_out);
void convertSoundDataF(const SoundData_t * soundData_in,
                       SoundData_F_t * soundDataF_out);
void convertParticleDataF(const ParticleData_t * particleData_in,
                          ParticleData_F_t * particleDataF_out);

bool setSoundInterruptThreshold(esphome::i2c::I2CDevice *device, uint16_t threshold_mPa);
bool setLightInterruptThreshold(esphome::i2c::I2CDevice *device, uint16_t thres_lux_int,
                                uint8_t thres_lux_fr_2dp);

SoundData_t getSoundData(esphome::i2c::I2CDevice *device);
AirData_t getAirData(esphome::i2c::I2CDevice *device);
LightData_t getLightData(esphome::i2c::I2CDevice *device);
AirQualityData_t getAirQualityData(esphome::i2c::I2CDevice *device);
ParticleData_t getParticleData(esphome::i2c::I2CDevice *device);

SoundData_F_t getSoundDataF(esphome::i2c::I2CDevice *device);
AirData_F_t getAirDataF(esphome::i2c::I2CDevice *device);
LightData_F_t getLightDataF(esphome::i2c::I2CDevice *device);
AirQualityData_F_t getAirQualityDataF(esphome::i2c::I2CDevice *device);
ParticleData_F_t getParticleDataF(esphome::i2c::I2CDevice *device);

float convertCtoF(float C);
void convertCtoF_int(float C, uint8_t * F_int, uint8_t * F_fr_1dp,
                     bool * isPositive);
float convertEncodedTemperatureToFloat(uint8_t T_C_int_with_sign,
                                       uint8_t T_C_fr_1dp);
const char * getTemperature(const AirData_t * pAirData, uint8_t * T_intPart, 
                            uint8_t * T_fractionalPart, bool * isPositive);
#endif

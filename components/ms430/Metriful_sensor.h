/*
  Metriful_sensor.h

  Defines the MS430 I2C helpers and data decoding functions.

  Copyright 2020-2023 Metriful Ltd.
  Licensed under the MIT License - for further details see LICENSE.txt
*/

#ifndef METRIFUL_SENSOR_H
#define METRIFUL_SENSOR_H

#include <stdint.h>
#include "sensor_constants.h"
#include "esphome/components/i2c/i2c.h"

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

bool TransmitI2C(esphome::i2c::I2CDevice *device, uint8_t commandRegister,
                 const uint8_t * data, uint8_t data_length);
bool ReceiveI2C(esphome::i2c::I2CDevice *device, uint8_t commandRegister,
                uint8_t data[], uint8_t data_length);

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

float convertEncodedTemperatureToFloat(uint8_t T_C_int_with_sign,
                                       uint8_t T_C_fr_1dp);
#endif

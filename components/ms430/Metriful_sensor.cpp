/*
  Metriful_sensor.cpp

  This file defines functions which are used in the code examples.
  Refactored for esp-idf.

  Copyright 2020-2023 Metriful Ltd.
  Licensed under the MIT License - for further details see LICENSE.txt

  For code examples, datasheet and user guide, visit
  https://github.com/metriful/sensor
*/

#include "Metriful_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/log.h"

volatile bool ready_assertion_event = false;

void convertAirDataF(const AirData_t * airData_in, AirData_F_t * airDataF_out)
{
  // Decode the signed value for T (in Celsius)
  airDataF_out->T_C = convertEncodedTemperatureToFloat(
                        airData_in->T_C_int_with_sign,
                        airData_in->T_C_fr_1dp);
  airDataF_out->P_Pa = airData_in->P_Pa;
  airDataF_out->H_pc = ((float) airData_in->H_pc_int)
                        + (((float) airData_in->H_pc_fr_1dp) / 10.0f);
  airDataF_out->G_Ohm = airData_in->G_ohm;
}

void convertAirQualityDataF(const AirQualityData_t * airQualityData_in, 
                                AirQualityData_F_t * airQualityDataF_out)
{
  airQualityDataF_out->AQI =  ((float) airQualityData_in->AQI_int) + 
                             (((float) airQualityData_in->AQI_fr_1dp) / 10.0f);
  airQualityDataF_out->CO2e = ((float) airQualityData_in->CO2e_int) + 
                             (((float) airQualityData_in->CO2e_fr_1dp) / 10.0f);
  airQualityDataF_out->bVOC = ((float) airQualityData_in->bVOC_int) + 
                             (((float) airQualityData_in->bVOC_fr_2dp) / 100.0f);
  airQualityDataF_out->AQI_accuracy = airQualityData_in->AQI_accuracy;
}

void convertLightDataF(const LightData_t * lightData_in,
                       LightData_F_t * lightDataF_out)
{
  lightDataF_out->illum_lux = ((float) lightData_in->illum_lux_int)
                          + (((float) lightData_in->illum_lux_fr_2dp) / 100.0f);
  lightDataF_out->white = lightData_in->white;
}

void convertSoundDataF(const SoundData_t * soundData_in,
                       SoundData_F_t * soundDataF_out)
{
  soundDataF_out->SPL_dBA = ((float) soundData_in->SPL_dBA_int)
                      + (((float) soundData_in->SPL_dBA_fr_1dp) / 10.0f);
  for (uint16_t i = 0; i < SOUND_FREQ_BANDS; i++)
  {
    soundDataF_out->SPL_bands_dB[i] = ((float) soundData_in->SPL_bands_dB_int[i])
                       + (((float) soundData_in->SPL_bands_dB_fr_1dp[i]) / 10.0f);
  }
  soundDataF_out->peakAmp_mPa = ((float) soundData_in->peak_amp_mPa_int)
                      + (((float) soundData_in->peak_amp_mPa_fr_2dp) / 100.0f);
  soundDataF_out->stable = (soundData_in->stable == 1);
}

void convertParticleDataF(const ParticleData_t * particleData_in,
                          ParticleData_F_t * particleDataF_out)
{
  particleDataF_out->duty_cycle_pc = ((float) particleData_in->duty_cycle_pc_int)
                      + (((float) particleData_in->duty_cycle_pc_fr_2dp) / 100.0f);
  particleDataF_out->concentration = ((float) particleData_in->concentration_int)
                      + (((float) particleData_in->concentration_fr_2dp) / 100.0f);
  particleDataF_out->valid = (particleData_in->valid == 1);
}

bool TransmitI2C(esphome::i2c::I2CDevice *device, uint8_t commandRegister,
                 const uint8_t * data, uint8_t data_length)
{
  if (data_length > 0)
  {
    return device->write_register(commandRegister, data, data_length);
  }
  else
  {
    return device->write(&commandRegister, 1);
  }
}

bool ReceiveI2C(esphome::i2c::I2CDevice *device, uint8_t commandRegister,
                uint8_t data[], uint8_t data_length)
{
  if (data_length == 0)
  {
    return false;
  }

  bool result = device->read_register(commandRegister, data, data_length);

  if (!result)
  {
    ESP_LOGE("ms430", "I2C read failed: register 0x%02X, length %u",
             commandRegister, data_length);
  }

  return result;
}

const char * interpret_AQI_accuracy(uint8_t AQI_accuracy_code)
{
  switch (AQI_accuracy_code)
  {
    default:
    case 0:
      return "Not yet valid, self-calibration incomplete";
    case 1:
      return "Low accuracy, self-calibration ongoing";
    case 2:
      return "Medium accuracy, self-calibration ongoing";
    case 3:
      return "High accuracy";
  }
}

const char * interpret_AQI_accuracy_brief(uint8_t AQI_accuracy_code)
{
  switch (AQI_accuracy_code)
  {
    default:
    case 0:
      return "Not yet valid";
    case 1:
      return "Low";
    case 2:
      return "Medium";
    case 3:
      return "High";
  }
}

const char * interpret_AQI_value(uint16_t AQI)
{
  if (AQI < 50) {
    return "Good";
  }
  else if (AQI < 100) {
    return "Acceptable";
  }
  else if (AQI < 150) {
    return "Substandard";
  }
  else if (AQI < 200) {
    return "Poor";
  }
  else if (AQI < 300) {
    return "Bad";
  }
  else {
    return "Very bad";
  }
}

bool setSoundInterruptThreshold(esphome::i2c::I2CDevice *device,
                                uint16_t threshold_mPa)
{
  uint8_t TXdata[SOUND_INTERRUPT_THRESHOLD_BYTES] = {0};
  TXdata[0] = (uint8_t) (threshold_mPa & 0x00FF);
  TXdata[1] = (uint8_t) (threshold_mPa >> 8);
  return TransmitI2C(device, SOUND_INTERRUPT_THRESHOLD_REG,
                     TXdata, SOUND_INTERRUPT_THRESHOLD_BYTES);
}

bool setLightInterruptThreshold(esphome::i2c::I2CDevice *device,
                                uint16_t thres_lux_int,
                                uint8_t thres_lux_fr_2dp)
{
  uint8_t TXdata[LIGHT_INTERRUPT_THRESHOLD_BYTES] = {0};
  TXdata[0] = (uint8_t) (thres_lux_int & 0x00FF);
  TXdata[1] = (uint8_t) (thres_lux_int >> 8);
  TXdata[2] = thres_lux_fr_2dp;
  return TransmitI2C(device, LIGHT_INTERRUPT_THRESHOLD_REG,
                     TXdata, LIGHT_INTERRUPT_THRESHOLD_BYTES);
}

SoundData_t getSoundData(esphome::i2c::I2CDevice *device)
{
  SoundData_t soundData = {0};
  ReceiveI2C(device, SOUND_DATA_READ,
             (uint8_t *) &soundData, SOUND_DATA_BYTES);
  return soundData;
}

AirData_t getAirData(esphome::i2c::I2CDevice *device)
{
  AirData_t airData = {0};
  ReceiveI2C(device, AIR_DATA_READ,
             (uint8_t *) &airData, AIR_DATA_BYTES);
  return airData;
}

LightData_t getLightData(esphome::i2c::I2CDevice *device)
{
  LightData_t lightData = {0};
  ReceiveI2C(device, LIGHT_DATA_READ,
             (uint8_t *) &lightData, LIGHT_DATA_BYTES);
  return lightData;
}

AirQualityData_t getAirQualityData(esphome::i2c::I2CDevice *device)
{
  AirQualityData_t airQualityData = {0};
  ReceiveI2C(device, AIR_QUALITY_DATA_READ,
             (uint8_t *) &airQualityData, AIR_QUALITY_DATA_BYTES);
  return airQualityData;
}

ParticleData_t getParticleData(esphome::i2c::I2CDevice *device)
{
  ParticleData_t particleData = {0};
  ReceiveI2C(device, PARTICLE_DATA_READ,
             (uint8_t *) &particleData, PARTICLE_DATA_BYTES);
  return particleData;
}

SoundData_F_t getSoundDataF(esphome::i2c::I2CDevice *device)
{
  SoundData_F_t soundDataF = {0};
  SoundData_t soundData = getSoundData(device);
  convertSoundDataF(&soundData, &soundDataF);
  return soundDataF;
}

AirData_F_t getAirDataF(esphome::i2c::I2CDevice *device)
{
  AirData_F_t airDataF = {0};
  AirData_t airData = getAirData(device);
  convertAirDataF(&airData, &airDataF);
  return airDataF;
}

LightData_F_t getLightDataF(esphome::i2c::I2CDevice *device)
{
  LightData_F_t lightDataF = {0};
  LightData_t lightData = getLightData(device);
  convertLightDataF(&lightData, &lightDataF);
  return lightDataF;
}

AirQualityData_F_t getAirQualityDataF(esphome::i2c::I2CDevice *device)
{
  AirQualityData_F_t airQualityDataF = {0};
  AirQualityData_t airQualityData = getAirQualityData(device);
  convertAirQualityDataF(&airQualityData, &airQualityDataF);
  return airQualityDataF;
}

ParticleData_F_t getParticleDataF(esphome::i2c::I2CDevice *device)
{
  ParticleData_F_t particleDataF = {0};
  ParticleData_t particleData = getParticleData(device);
  convertParticleDataF(&particleData, &particleDataF);
  return particleDataF;
}

float convertCtoF(float C)
{
  return ((C * 1.8f) + 32.0f);
}

void convertCtoF_int(float C, uint8_t * F_int, uint8_t * F_fr_1dp,
                     bool * isPositive)
{
  float F = convertCtoF(C);
  bool isNegative = (F < 0.0f);
  if (isNegative)
  {
    F = -F;
  }
  F += 0.05f;
  F_int[0] = (uint8_t) F;
  F -= (float) F_int[0];
  F_fr_1dp[0] = (uint8_t) (F * 10.0f);
  isPositive[0] = (!isNegative);
}

float convertEncodedTemperatureToFloat(uint8_t T_C_int_with_sign,
                                       uint8_t T_C_fr_1dp)
{
  float temperature_C = ((float) (T_C_int_with_sign & TEMPERATURE_VALUE_MASK))
                          + (((float) T_C_fr_1dp) / 10.0f);
  if ((T_C_int_with_sign & TEMPERATURE_SIGN_MASK) != 0)
  {
    // The most-significant bit is set, which indicates that
    // the temperature is negative
    temperature_C = -temperature_C;
  }
  return temperature_C;
}

const char * getTemperature(const AirData_t * pAirData, uint8_t * T_intPart, 
                            uint8_t * T_fractionalPart, bool * isPositive)
{
  #ifdef USE_FAHRENHEIT
    float temperature_C = convertEncodedTemperatureToFloat(
                              pAirData->T_C_int_with_sign, 
                              pAirData->T_C_fr_1dp);
    convertCtoF_int(temperature_C, T_intPart, T_fractionalPart, isPositive);
    return FAHRENHEIT_SYMBOL;
  #else
    isPositive[0] = ((pAirData->T_C_int_with_sign
                      & TEMPERATURE_SIGN_MASK) == 0);
    T_intPart[0] = pAirData->T_C_int_with_sign & TEMPERATURE_VALUE_MASK;
    T_fractionalPart[0] = pAirData->T_C_fr_1dp;
    return CELSIUS_SYMBOL;
  #endif
}

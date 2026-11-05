/*
  ms430_esphome.cpp

  ESPHome external component for the Metriful MS430 environment sensor.

  Copyright 2025 Metriful Ltd.
  Licensed under the MIT License - for further details see LICENSE.txt

  For code examples, datasheet and user guide, visit
  https://github.com/metriful/sensor
*/

#include "ms430_esphome.h"
#include "sensor_constants.h"

namespace esphome {
namespace ms430 {

void MS430::setup()
{

    if (this->ready_pin_ != nullptr) {
        this->ready_pin_->setup();
    }
  
    uint32_t start_time = millis();
    while ((this->ready_pin_ != nullptr) && this->ready_pin_->digital_read()
           && (millis() - start_time < 2000)) {
        delay(10);
    }
  
    uint8_t reset_cmd = RESET_CMD;
    this->write(&reset_cmd, 1);
    delay(10); // Give the sensor a moment to process the reset
    start_time = millis();
    while ((this->ready_pin_ != nullptr) && this->ready_pin_->digital_read()
           && (millis() - start_time < 2000)) {
        delay(10);
    } 
  
    TransmitI2C(this, PARTICLE_SENSOR_SELECT_REG, &this->particle_sensor_, 1);
    TransmitI2C(this, CYCLE_TIME_PERIOD_REG, &this->cycle_period_, 1);

    uint8_t cycle_mode = CYCLE_MODE_CMD;
    this->write(&cycle_mode, 1);
}

void MS430::loop()
{

    if (this->ready_pin_ != nullptr) {
        bool current_ready = this->ready_pin_->digital_read();
        // If the pin was HIGH on the last loop and is LOW now, data is ready
        bool data_ready = this->last_ready_state_ && !current_ready;
        this->last_ready_state_ = current_ready;

        if (data_ready && (this->stage_ == 0)) {
            this->stage_ = 1;
        }
    }

    this->stage_ = this->output(this->stage_);
}

float MS430::get_setup_priority() const
{
    return setup_priority::BUS;
}

uint8_t MS430::output(uint8_t stage)
{
    if (stage == 0)
    {
        return 0;
    }
    if (stage == 1)
    {
        this->airDataF_ = getAirDataF(this);
    }
    if (stage == 2)
    {
        this->airQualityDataF_ = getAirQualityDataF(this);
    }
    if ((stage == 3) && (this->particle_sensor_ != PARTICLE_SENSOR_OFF))
    {
        this->particleDataF_ = getParticleDataF(this);
    }
    if (stage == 4)
    {
        this->lightDataF_ = getLightDataF(this);
    }
    if (stage == 5)
    {
        this->soundDataF_ = getSoundDataF(this);
    }
    if (stage == 6)
    {
        if (this->temperature_sensor != nullptr) {
            this->temperature_sensor->publish_state(this->airDataF_.T_C);
        }
        if (this->pressure_sensor != nullptr) {
            this->pressure_sensor->publish_state(this->airDataF_.P_Pa);
        }
        if (this->humidity_sensor != nullptr) {
            this->humidity_sensor->publish_state(this->airDataF_.H_pc);
        }
        if (this->gas_sensor != nullptr) {
            this->gas_sensor->publish_state(this->airDataF_.G_Ohm);
        }
    }
    if (stage == 7)
    {
        if (this->aqi_acc_sensor != nullptr) {
            this->aqi_acc_sensor->publish_state(this->airQualityDataF_.AQI_accuracy);
        }
        if (this->aqi_acc_text_sensor != nullptr) {
            this->aqi_acc_text_sensor->publish_state(
                interpret_AQI_accuracy_brief(this->airQualityDataF_.AQI_accuracy));
        }
        if (this->airQualityDataF_.AQI_accuracy > 0)
        {
            AQIinitialized = true;
        }

        if (AQIinitialized)
        {
            if (this->aqi_sensor != nullptr) {
                this->aqi_sensor->publish_state(this->airQualityDataF_.AQI);
            }
            if (this->CO2e_sensor != nullptr) {
                this->CO2e_sensor->publish_state(this->airQualityDataF_.CO2e);
            }
            if (this->bVOC_sensor != nullptr) {
                this->bVOC_sensor->publish_state(this->airQualityDataF_.bVOC);
            }
            if (this->aqi_text_sensor != nullptr) {
                this->aqi_text_sensor->publish_state(
                    interpret_AQI_value((uint16_t) this->airQualityDataF_.AQI));
            }
        }
    }
    if ((stage == 8) && (this->particle_sensor_ != PARTICLE_SENSOR_OFF))
    {
        if (this->particle_duty_sensor != nullptr) {
            this->particle_duty_sensor->publish_state(this->particleDataF_.duty_cycle_pc);
        }
        if (this->particle_conc_sensor != nullptr) {
            this->particle_conc_sensor->publish_state(this->particleDataF_.concentration);
        }
    }
    if (stage == 9)
    {
        if (this->w_light_sensor != nullptr) {
            this->w_light_sensor->publish_state(this->lightDataF_.white);
        }
        if (this->illum_sensor != nullptr) {
            this->illum_sensor->publish_state(this->lightDataF_.illum_lux);
        }
    }
    if (stage == 10)
    {
        if (this->sound_spl_sensor != nullptr) {
            this->sound_spl_sensor->publish_state(this->soundDataF_.SPL_dBA);
        }
        if (this->sound_peak_sensor != nullptr) {
            this->sound_peak_sensor->publish_state(this->soundDataF_.peakAmp_mPa);
        }
        if (this->sound_band0_sensor != nullptr) {
            this->sound_band0_sensor->publish_state(this->soundDataF_.SPL_bands_dB[0]);
        }
        if (this->sound_band1_sensor != nullptr) {
            this->sound_band1_sensor->publish_state(this->soundDataF_.SPL_bands_dB[1]);
        }
    }
    if (stage == 11)
    {
        if (this->sound_band2_sensor != nullptr) {
            this->sound_band2_sensor->publish_state(this->soundDataF_.SPL_bands_dB[2]);
        }
        if (this->sound_band3_sensor != nullptr) {
            this->sound_band3_sensor->publish_state(this->soundDataF_.SPL_bands_dB[3]);
        }
        if (this->sound_band4_sensor != nullptr) {
            this->sound_band4_sensor->publish_state(this->soundDataF_.SPL_bands_dB[4]);
        }
        if (this->sound_band5_sensor != nullptr) {
            this->sound_band5_sensor->publish_state(this->soundDataF_.SPL_bands_dB[5]);
        }
    }
    stage++;
    if (stage > 11) {
        stage = 0;
    }
    this->status_clear_warning();
    return stage;
}

}
}

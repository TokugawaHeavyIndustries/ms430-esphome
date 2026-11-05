/*
  ms430_esphome.h

  ESPHome external component for the Metriful MS430 environment sensor.

  Copyright 2025 Metriful Ltd.
  Licensed under the MIT License - for further details see LICENSE.txt

  For code examples, datasheet and user guide, visit
  https://github.com/metriful/sensor
*/

#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/gpio.h"
#include "sensor_constants.h"
#include "Metriful_sensor.h"

namespace esphome {
namespace ms430 {

class MS430 : public Component, public i2c::I2CDevice {
    public:
        void set_ready_pin(GPIOPin *pin) { ready_pin_ = pin; }
        // Particle sensor type and cycle period are set from YAML
        void set_particle_sensor(uint8_t type) { particle_sensor_ = type; }
        void set_cycle_period(uint8_t period) { cycle_period_ = period; }

        void set_temperature_s(sensor::Sensor *s) { temperature_sensor = s; }
        void set_pressure_s(sensor::Sensor *s) { pressure_sensor = s; }
        void set_humidity_s(sensor::Sensor *s) { humidity_sensor = s; }
        void set_gas_s(sensor::Sensor *s) { gas_sensor = s; }
        void set_w_light_s(sensor::Sensor *s) { w_light_sensor = s; }
        void set_illum_s(sensor::Sensor *s) { illum_sensor = s; }
        void set_aqi_acc_s(sensor::Sensor *s) { aqi_acc_sensor = s; }
        void set_aqi_s(sensor::Sensor *s) { aqi_sensor = s; }
        void set_CO2e_s(sensor::Sensor *s) { CO2e_sensor = s; }
        void set_bVOC_s(sensor::Sensor *s) { bVOC_sensor = s; }
        void set_particle_duty_s(sensor::Sensor *s) { particle_duty_sensor = s; }
        void set_particle_conc_s(sensor::Sensor *s) { particle_conc_sensor = s; }
        void set_sound_spl_s(sensor::Sensor *s) { sound_spl_sensor = s; }
        void set_sound_peak_s(sensor::Sensor *s) { sound_peak_sensor = s; }
        void set_sound_band0_s(sensor::Sensor *s) { sound_band0_sensor = s; }
        void set_sound_band1_s(sensor::Sensor *s) { sound_band1_sensor = s; }
        void set_sound_band2_s(sensor::Sensor *s) { sound_band2_sensor = s; }
        void set_sound_band3_s(sensor::Sensor *s) { sound_band3_sensor = s; }
        void set_sound_band4_s(sensor::Sensor *s) { sound_band4_sensor = s; }
        void set_sound_band5_s(sensor::Sensor *s) { sound_band5_sensor = s; }
        void set_aqi_text_s(text_sensor::TextSensor *s) { aqi_text_sensor = s; }
        void set_aqi_acc_text_s(text_sensor::TextSensor *s) { aqi_acc_text_sensor = s; }

        void setup() override;
        void loop() override;
        float get_setup_priority() const override;

    protected:
        GPIOPin *ready_pin_{nullptr};
        bool last_ready_state_{true};
        uint8_t stage_{0};

        AirData_F_t airDataF_{};
        AirQualityData_F_t airQualityDataF_{};
        ParticleData_F_t particleDataF_{};
        LightData_F_t lightDataF_{};
        SoundData_F_t soundDataF_{};
        uint8_t particle_sensor_{PARTICLE_SENSOR_PPD42};
        uint8_t cycle_period_{CYCLE_PERIOD_3_S};

        uint8_t output(uint8_t stage);
        sensor::Sensor *temperature_sensor{nullptr};
        sensor::Sensor *pressure_sensor{nullptr};
        sensor::Sensor *humidity_sensor{nullptr};
        sensor::Sensor *gas_sensor{nullptr};
        sensor::Sensor *w_light_sensor{nullptr};
        sensor::Sensor *illum_sensor{nullptr};
        sensor::Sensor *aqi_acc_sensor{nullptr};
        sensor::Sensor *aqi_sensor{nullptr};
        sensor::Sensor *CO2e_sensor{nullptr};
        sensor::Sensor *bVOC_sensor{nullptr};
        sensor::Sensor *particle_duty_sensor{nullptr};
        sensor::Sensor *particle_conc_sensor{nullptr};
        sensor::Sensor *sound_spl_sensor{nullptr};
        sensor::Sensor *sound_peak_sensor{nullptr};
        sensor::Sensor *sound_band0_sensor{nullptr};
        sensor::Sensor *sound_band1_sensor{nullptr};
        sensor::Sensor *sound_band2_sensor{nullptr};
        sensor::Sensor *sound_band3_sensor{nullptr};
        sensor::Sensor *sound_band4_sensor{nullptr};
        sensor::Sensor *sound_band5_sensor{nullptr};
        text_sensor::TextSensor *aqi_text_sensor{nullptr};
        text_sensor::TextSensor *aqi_acc_text_sensor{nullptr};
        bool AQIinitialized = false;
};

}
}

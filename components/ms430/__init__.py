"""
  __init__.py

  ESPHome external component for the Metriful MS430 environment sensor.

  Copyright 2025 Metriful Ltd.
  Licensed under the MIT License - for further details see LICENSE.txt

"""

from esphome import codegen
import esphome.config_validation as cv
from esphome.components import sensor, text_sensor, i2c
from esphome import const as c
from esphome import pins

AUTO_LOAD = ["sensor", "text_sensor"]
DEPENDENCIES = ["i2c"]

CONF_READY_PIN = "ready_pin"
CONF_PARTICLE_SENSOR = "particle_sensor"
CONF_CYCLE_PERIOD = "cycle_period"
CONF_WHITE_LIGHT = "white_light"
CONF_AQI = "aqi"
CONF_AQI_TEXT = "aqi_text"
CONF_AQI_ACCURACY_TEXT = "aqi_accuracy_text"
CONF_E_CO2 = "e_co2"
CONF_B_VOC = "b_voc"
CONF_PARTICLE_DUTY = "particle_duty"
CONF_PARTICLE_CONC = "particle_concentration"
CONF_SOUND_SPL = "sound_spl"
CONF_SOUND_PEAK = "sound_peak"
CONF_SOUND_BAND_0 = "sound_band_0"
CONF_SOUND_BAND_1 = "sound_band_1"
CONF_SOUND_BAND_2 = "sound_band_2"
CONF_SOUND_BAND_3 = "sound_band_3"
CONF_SOUND_BAND_4 = "sound_band_4"
CONF_SOUND_BAND_5 = "sound_band_5"

UNIT_DECIBEL_A = "dBA"
UNIT_MILLIPASCAL = "mPa"
# The PPD42 reports particles per litre; the SDS011 reports ug/m3
UNIT_PARTICLES_PER_LITRE = "ppL"

# Values match PARTICLE_SENSOR_* and CYCLE_PERIOD_* in sensor_constants.h
PARTICLE_SENSORS = {"NONE": 0, "PPD42": 1, "SDS011": 2}
CYCLE_PERIODS = {"3s": 0, "100s": 1, "300s": 2}

ns = codegen.esphome_ns.namespace("ms430")
MS430 = ns.class_("MS430", codegen.Component, i2c.I2CDevice)


def measurement(icon, unit=cv.UNDEFINED, decimals=1, device_class=cv.UNDEFINED):
    return sensor.sensor_schema(
        unit_of_measurement=unit,
        icon=icon,
        accuracy_decimals=decimals,
        device_class=device_class,
        state_class=c.STATE_CLASS_MEASUREMENT,
    )


def sound_band(icon="mdi:sine-wave"):
    return measurement(icon, c.UNIT_DECIBEL, 1, c.DEVICE_CLASS_SOUND_PRESSURE)


# Each entry: (config key, C++ setter, default name, schema, extra defaults)
SENSORS = [
    (c.CONF_TEMPERATURE, "set_temperature_s", "Temperature",
     measurement("mdi:thermometer", c.UNIT_CELSIUS, 1, c.DEVICE_CLASS_TEMPERATURE), {}),
    (c.CONF_HUMIDITY, "set_humidity_s", "Humidity",
     measurement("mdi:cloud-percent", c.UNIT_PERCENT, 1, c.DEVICE_CLASS_HUMIDITY), {}),
    (c.CONF_PRESSURE, "set_pressure_s", "Air pressure",
     measurement("mdi:weather-partly-rainy", c.UNIT_PASCAL, 0,
                 c.DEVICE_CLASS_ATMOSPHERIC_PRESSURE), {}),
    (c.CONF_GAS_RESISTANCE, "set_gas_s", "Gas sensor resistance",
     measurement("mdi:scent", c.UNIT_OHM, 0), {}),
    (CONF_WHITE_LIGHT, "set_w_light_s", "White light level",
     measurement("mdi:circle-outline", decimals=0), {}),
    (c.CONF_ILLUMINANCE, "set_illum_s", "Illuminance",
     measurement("mdi:white-balance-sunny", c.UNIT_LUX, 2, c.DEVICE_CLASS_ILLUMINANCE), {}),
    (c.CONF_IAQ_ACCURACY, "set_aqi_acc_s", "Air quality accuracy",
     measurement("mdi:check-decagram", decimals=0), {c.CONF_INTERNAL: True}),
    (CONF_AQI, "set_aqi_s", "Air quality index",
     measurement("mdi:flower-tulip-outline", decimals=1, device_class=c.DEVICE_CLASS_AQI), {}),
    (CONF_E_CO2, "set_CO2e_s", "Estimated CO₂",
     measurement("mdi:molecule-co2", c.UNIT_PARTS_PER_MILLION, 1,
                 c.DEVICE_CLASS_CARBON_DIOXIDE), {}),
    (CONF_B_VOC, "set_bVOC_s", "Equivalent breath VOC",
     measurement("mdi:account-voice", c.UNIT_PARTS_PER_MILLION, 2,
                 c.DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS_PARTS), {}),
    (CONF_PARTICLE_DUTY, "set_particle_duty_s", "Particle sensor duty cycle",
     measurement("mdi:square-wave", c.UNIT_PERCENT, 2), {}),
    # Unit and device class are filled in by apply_particle_sensor()
    (CONF_PARTICLE_CONC, "set_particle_conc_s", "Particle concentration",
     measurement("mdi:chart-bubble", decimals=2), {}),
    (CONF_SOUND_SPL, "set_sound_spl_s", "Sound pressure level",
     measurement("mdi:microphone", UNIT_DECIBEL_A, 1, c.DEVICE_CLASS_SOUND_PRESSURE), {}),
    (CONF_SOUND_PEAK, "set_sound_peak_s", "Peak sound amplitude",
     measurement("mdi:waveform", UNIT_MILLIPASCAL, 2), {}),
    (CONF_SOUND_BAND_0, "set_sound_band0_s", "SPL at 125 Hz", sound_band(), {}),
    (CONF_SOUND_BAND_1, "set_sound_band1_s", "SPL at 250 Hz", sound_band(), {}),
    (CONF_SOUND_BAND_2, "set_sound_band2_s", "SPL at 500 Hz", sound_band(), {}),
    (CONF_SOUND_BAND_3, "set_sound_band3_s", "SPL at 1000 Hz", sound_band(), {}),
    (CONF_SOUND_BAND_4, "set_sound_band4_s", "SPL at 2000 Hz", sound_band(), {}),
    (CONF_SOUND_BAND_5, "set_sound_band5_s", "SPL at 4000 Hz", sound_band(), {}),
]

TEXT_SENSORS = [
    (CONF_AQI_TEXT, "set_aqi_text_s", "Air quality",
     text_sensor.text_sensor_schema(icon="mdi:flower-tulip-outline")),
    (CONF_AQI_ACCURACY_TEXT, "set_aqi_acc_text_s", "Air quality accuracy",
     text_sensor.text_sensor_schema(icon="mdi:check-decagram")),
]

PARTICLE_KEYS = (CONF_PARTICLE_DUTY, CONF_PARTICLE_CONC)


def with_defaults(sensor_schema, name, extra=None):
    """Merge the user's settings for a sensor over its defaults, so that
    e.g. "internal: true" alone keeps the default name."""
    defaults = {c.CONF_NAME: name, **(extra or {})}

    def validator(value):
        if value is None:  # key listed with nothing under it
            value = {}
        if not isinstance(value, dict):
            raise cv.Invalid("Expected a mapping of sensor settings")
        return sensor_schema({**defaults, **value})

    return validator


def particle_sensor_type(value):
    if value is False or str(value).upper() == "OFF":
        value = "NONE"
    return cv.enum(PARTICLE_SENSORS, upper=True)(value)


def apply_particle_sensor(config):
    """
    NONE:   self-explanatory
    PPD42:  concentration in particles per litre, no HA device class
    SDS011: concentration in ug/m3, HA device class PM10
    """
    sensor_type = config[CONF_PARTICLE_SENSOR]
    if sensor_type == "NONE":
        for key in PARTICLE_KEYS:
            config.pop(key, None)
        return config

    conc = config.get(CONF_PARTICLE_CONC)
    if conc is None:
        return config

    if sensor_type == "PPD42":
        conc.setdefault(c.CONF_UNIT_OF_MEASUREMENT, UNIT_PARTICLES_PER_LITRE)
    else:
        conc.setdefault(c.CONF_UNIT_OF_MEASUREMENT, c.UNIT_MICROGRAMS_PER_CUBIC_METER)
        conc.setdefault(c.CONF_DEVICE_CLASS, c.DEVICE_CLASS_PM10)
    return config


schema = {
    cv.GenerateID(): cv.declare_id(MS430),
    cv.Required(CONF_READY_PIN): pins.gpio_input_pin_schema,
    cv.Optional(CONF_PARTICLE_SENSOR, default="NONE"): particle_sensor_type,
    cv.Optional(CONF_CYCLE_PERIOD, default="100s"):
        cv.enum(CYCLE_PERIODS, lower=True),
}
for key, _, name, sensor_schema, extra in SENSORS:
    schema[cv.Optional(key, default={})] = with_defaults(sensor_schema, name, extra)
for key, _, name, sensor_schema in TEXT_SENSORS:
    schema[cv.Optional(key, default={})] = with_defaults(sensor_schema, name)

CONFIG_SCHEMA = cv.All(
    cv.Schema(schema)
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x71)),
    apply_particle_sensor,
)


async def to_code(config):
    var = codegen.new_Pvariable(config[c.CONF_ID])
    await codegen.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    pin = await codegen.gpio_pin_expression(config[CONF_READY_PIN])
    codegen.add(var.set_ready_pin(pin))
    codegen.add(var.set_particle_sensor(config[CONF_PARTICLE_SENSOR]))
    codegen.add(var.set_cycle_period(config[CONF_CYCLE_PERIOD]))

    for key, setter, _, _, _ in SENSORS:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            codegen.add(getattr(var, setter)(sens))

    for key, setter, _, _ in TEXT_SENSORS:
        if key in config:
            sens = await text_sensor.new_text_sensor(config[key])
            codegen.add(getattr(var, setter)(sens))

# Metriful MS430 ESPHome Libraries

The Metriful MS430 is a low power, high accuracy, smart sensor cluster for indoor environment monitoring. It operates via a simple I2C-compatible interface and measures eighteen variables including air quality, light and sound levels.

This repository includes refactored code ported to the `esp-idf` framework, allowing for a simple seven line configuration in ESPHome.  Tested extensively over Thread on the C6 series boards!!!

## Wiring

The Wemos D1 Mini (ESP8266) and ESP32-C6 Super Mini (ESP32) have exactly the same pinouts for this simple three-GPIO project, but make sure to adjust to your exact configuration.

|    MS430 pin    |        ESP         |                                    Notes                                    |
|:---------------:|:------------------:|:---------------------------------------------------------------------------:|
|        PRT      |        -           |                           Used for particle sensor.                         |
|        RDY      |        D3          |       Bog-standard digital output pin, so it can go on any safe GPIO.       |
|        SIT      |        -           |            Sound interrupt output.  Not yet implemented for ESPHome.        |
|        LIT      |        -           |            Light interrupt output.  Not yet implemented for ESPHome.        |
|        SDA      |        D2          |                                                                             |
|        SCL      |        D1          |                                                                             |
|        VPU      |        3V3         |                         Drives SCL/SDA/RDY pin voltage.**†**                |
|        GND      |        GND         |                                                                             |
|        VDD      |        3V3         |                            Power input, 3.3v.**‡**                          |
|        VIN      |        5V          |                            Power input, 5v.**‡**                            |

**† Must be connected to 3.3v!!** If you're powering your MS430 using VDD, simply jumper it to VDD.  If you're powering via VIN, you must run a second jumper to your ESP32 and connect to a 3.3v output.  If using a particle sensor, see below for wiring recommendations.

**<ins>_‡ Only use one power input!!_</ins>**


### Connecting a particle sensor

The MS430 is compatible with the `Shinyei PPD42` and the `Nova SDS011`. A particle sensor is optional and only a single sensor can be connected at any time.  The neatest and easiest wiring method is to connect **VIN** to ESP 5v and **VPU** to ESP 3.3v.  You can then run all three particle sensor wires to the MS430 and simply tie into **VIN** and **GND** alongside your **PRT** connection.

|       | PPD42 pin number | SDS011 pin label |
|:-----:|:----------------:|:----------------:|
|   5V  |        3         |        5V        |
|  GND  |        1         |        GND       |
|  PRT  |        4         |        25um      | 


## ESPHome Configuration

Add the following to your device's ESPHome YAML, set the I2C pins, and your RDY pin:

```yaml
external_components:
- source: github://TokugawaHeavyIndustries/ms430-esphome

i2c:
  sda: 2
  scl: 1

ms430:
  ready_pin: 3
```

### Options

* **particle_sensor**: selects the connected particle sensor. This tells the MS430 which sensor is attached and sets up the particle sensors in Home Assistant to match.  Defaults to NONE.

* **cycle_period**: how often the MS430 takes a measurement. 100s-300s are recommended to avoid self-heating of the board, which raises the temperature reading.  Defaults to 100s if not set.

* **address**: I2C address.  If SB1 is bridged on the MS430, set to 0x70.  Defaults to 0x71 if not set. 

### Customizing sensors

All sensors are created and enumerated to Home Assistant automatically.  However, explicit declaration in your YAML allows you to customize values, apply filters, or hide from Home Assistant completely (`internal: true`).

| Key | Default name |
|:----|:-------------|
| temperature | Temperature |
| humidity | Humidity |
| pressure | Air pressure |
| gas_resistance | Gas sensor resistance |
| aqi | Air quality index |
| aqi_text | Air quality (Good, Acceptable, Substandard, Poor, Bad, Very bad) |
| aqi_accuracy_text | Air quality accuracy (Not yet valid, Low, Medium, High) |
| iaq_accuracy | Air quality accuracy (numeric, internal by default) |
| e_co2 | Estimated CO₂ |
| b_voc | Equivalent breath VOC |
| white_light | White light level |
| illuminance | Illuminance |
| sound_spl | Sound pressure level |
| sound_peak | Peak sound amplitude |
| sound_band_0 ... sound_band_5 | SPL at 125 / 250 / 500 / 1000 / 2000 / 4000 Hz |
| particle_duty | Particle sensor duty cycle |
| particle_concentration | Particle concentration |

* Air quality values are published once the sensor's self-calibration has completed (accuracy above "Not yet valid").
* Particle concentration units differ by model.  PPD42 outputs `particles per litre (ppL)` and SDS011 outputs `micrograms per cubic meter (µg/m³)`
* Internally, temperature is output as Celsius, but Home Assistant will auto-convert to Fahrenheit if your Home Assistant is set to US Customary units.  You can manually override if you really want to by setting `unit_of_measurement: °F` and a `lambda: return x * 1.8 + 32` filter under your **Temperature** sensor.
  

## Example Full-Featured YAML

In this example, we're using an ESP32C6 over Thread, with a PPD42 particle sensor.  We bridged SB1 on the MS430, requiring an I2C address change.  We've also increased the cycle time to 300s.  Finally, **Gas Resistance** is hidden, **Temperature** has an overridden name, and a smoothing filter is applied to **Humidity**.

```yaml
esphome:
  name: master-bedroom-environment
  friendly_name: Master Bedroom Environment

esp32:
  board: esp32-c6-devkitm-1
  framework:
    type: esp-idf

network:
  enable_ipv6: true

openthread:
  tlv: 000300000b010278dd0210f71cd78e8f52c8e7a7fadb5c3798cf01

api:
  encryption:
    key: "LA1iU76YJ3Rq12Wwl5O68ehOdGjwyvpIP7AXlbVFWK0="

ota:
  - platform: esphome
    encryption:

i2c:
  sda: 3
  scl: 4

external_components:
- source: github://TokugawaHeavyIndustries/ms430-esphome
  
ms430:
  ready_pin: 0
  particle_sensor: PPD42
  cycle_period: 300s
  address: 0x70
  temperature:
    name: "Living room temperature"
  humidity:
    filters:
    - sliding_window_moving_average:
        window_size: 15
        send_every: 15
  gas_resistance:
    internal: true
```


## License

See the [LICENSE](LICENSE.txt) file for software license rights and limitations (MIT).

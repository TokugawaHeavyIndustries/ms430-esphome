# Metriful MS430: Environment Sensor

The Metriful MS430 is a low power, high accuracy, smart sensor cluster for indoor environment monitoring. It operates via a simple I2C-compatible interface and measures eighteen variables including air quality, light and sound levels.

### Contents

#### Hardware setup
- **[ESP8266](#esp8266)**<br>
- **[ESP32](#esp32)**<br>
- **[Particle sensor](#connecting-and-enabling-a-particle-sensor)**<br>
#### Code example setup
- **[ESPHome for Home Assistant](#esphome-for-home-assistant)**<br>
#### Other information
- **[License](#license)**<br>



### Wiring for ESP8266/ESP32

|    MS430 pin    |       ESP8266        |     ESP32     |
|:---------------:|:--------------------:|:-------------:|
|        VIN      |        5V            |      5V       |
|        GND      |        GND           |      GND      | 
|        VPU      |        5V            |      5V       | 
|        SCL      |    D1 (GPIO 5)       |      D22      |
|        SDA      |    D2 (GPIO 4)       |      D21      |
|        RDY      |    D6 (GPIO 12)      |      D23      |

* VPU is a pull-up pin.  Feel free to jumper it to VIN at the MS430.
* PRT is used with particle sensors.
* SIT/LIT are interrupt lines and are not implemented for ESPHome.
* If you don't have 5V, leave VIN disconnected and connect VDD/VPU to 3V3.


## Connecting and enabling a particle sensor

The MS430 is compatible with `Shinyei PPD42` and  `Nova SDS011`. The particle sensor is optional and only a single sensor can be connected at any time.

|       | PPD42 pin number | SDS011 pin label |
|:-----:|:----------------:|:----------------:|
|   5V  |        3         |        5V        |
|  GND  |        1         |        GND       |
|  PRT  |        4         |        25um      | 

* It's easiest to connect all three pins to the MS430 rather than run two separate jumpers back to your ESP board.
* Particle sensors require 5V and are therefore incompatible if using 3V3 via the VDD pin.

## Configuration

Add the following to your device's ESPHome YAML, set the I2C pins, and your RDY pin:

```yaml
external_components:
  - source: github://TokugawaHeavyIndustries/ms430-esphome
    components: [ms430]

i2c:
  sda: 3
  scl: 4

ms430:
  ready_pin: 0
```

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
| particle_duty | Particle sensor duty cycle (if enabled) |
| particle_concentration | Particle concentration (if enabled) |

The air quality values are only published once the sensor's self-calibration has started (accuracy above "Not yet valid").

### Options

* **particle_sensor**: selects the connected particle sensor. This tells the MS430 which sensor is attached and sets up the particle sensors in Home Assistant to match.  Defaults to NONE.

  | Setting | Concentration unit | HA device class | Particle sensors created |
  |:-------:|:------------------:|:---------------:|:------------------------:|
  | PPD42 (default) | particles per litre (ppL) | none | yes |
  | SDS011 | µg/m³ | PM10 | yes |
  | NONE | - | - | no |

  A unit or device class you set yourself on **particle_concentration** is kept.
  
* **cycle_period**: how often the MS430 takes a measurement. 100s-300s are recommended to avoid self-heating of the board, which raises the temperature reading.  Defaults to 100s if not set.

* **address**: I2C address.  If SB1 is bridged on the MS430, set to 0x70.  Defaults to 0x71 if not set. 

### Customizing sensors

To hide a measurement from Home Assistant, set **internal: true** on it.  Additionally, any setting of any sensor can be overridden by listing it and defining options/filters.  For example:

```yaml
ms430:
  ready_pin: 0
  address: 0x70
  particle_sensor: PPD42
  cycle_period: 5s
  temperature:
    name: "Living room temperature"
    filters:
      - offset: -1.5
  sound_band_0:
    internal: true   # hide from Home Assistant
```



## License

See the [LICENSE](LICENSE.txt) file for software license rights and limitations (MIT).

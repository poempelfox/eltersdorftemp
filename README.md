
# eltersdorftemp

## Introduction

2023 edition (the first).

This is WIP.

## hardware

* ESP32 DevKitC V4 (from AZ delivery) for control and WiFi
* SHT41 temperature and humidity sensor. This could be replaced by a SHT45, preferably the version with integrated filter membrane, once proper breakout boards become available, without requiring any software changes.
* LPS35HW pressure sensor
* SEN50 particulate matter sensor
* (probably) RG15 rain sensor
* (perhaps) DS18B20 temperature sensor in moulded metal case to measure ground temperature 50 cm below ground
* the following sensors will definitely not be included:
  - wind sensor - too many buildings in the surroundings, this would not measure anything useful
  - light / UV sensor - too many things giving shadows at random times around
* power will be coming from the mains grid, we have no reason to preserve power, except of course keeping our heat dissapation low to not spoil our own measurements

## cabling

* there is a power distribution board that also distributes I2C bus 0
* I2C bus 0
  - as always, SCL is green and SDA is yellow
  - SCL on GPIO18, SDA on GPIO19
  - SHT41
  - LPS35HW
* I2C bus 1
  - SCL on GPIO33, SDA on GPIO32
  - SEN50


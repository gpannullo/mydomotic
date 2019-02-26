# mydomotic
Firmware for controlling your home
# How to compile and run
For compile it, use PlatformIO

# Requirements

Dependency Graph:
```
|-- <MyDomotic Library> 1.2   (inside)
|   |-- <ELClient>            (inside)
|   |-- <Ethernet> 1.1.2      (inside)
|   |   |-- <SPI> 1.0         (inside)
|   |-- <PubSubClient> 2.6    (to add)
|   |-- <ArduinoJson> 5.13.2  (to add)
|   |-- <AsyncDelay> 1.0.2    (to add)
|   |-- <EEPROM> 2.0          (inside)
```

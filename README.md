# MyDomotic

MyDomotic firmware è una collezione di automatisti già pronti per

# How to compile and run
For compile it, use PlatformIO. Scarica il software Atom da https://atom.io/  e successivamente aggiungere il modulo PlatformIO.


# Requirements
Le librerie necessarie per la compilazione da aggiungere alla vostra installazione sono:
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



# Howto install esplink firmware

I have used for my work a esp01-s with 1M of memory but standard version (512K) is also good got it. The different is as you programming it: For example for my esp01s I have used last stable version (3.0.14 at http://s3.voneicken.com/esp-link/esp-link-v3.0.14-g963ffbb.tgz). After downloaded this archive and extract all file, use this command for programming:

```
esptool.py --port /dev/tty.usbserial-A50285BI \
            --baud 115200 erase_flash #for erase all data on esp01

esptool.py --port /dev/tty.usbserial-A50285BI \
            --baud 115200 write_flash \
            -fs 1MB -ff 40m \
            0x00000 boot_v1.6.bin \
            0x1000 user1.bin \
            0xFC000 esp_init_data_default.bin \
            0xFE000 blank.bin
```

For esp01 model 512K use this command:

```
esptool.py --port /dev/tty.usbserial-A50285BI \
            --baud 115200 erase_flash #for erase all data on esp01

esptool.py --port /dev/tty.usbserial-A50285BI \
            --baud 460800 write_flash \
            -fs 512KB -ff 40m \
            0x00000 boot_v1.5.bin \
            0x1000 user1.bin \
            0x7C000 esp_init_data_default.bin \
            0x7E000 blank.bin
```

For install esptool.py you must install python and then run a command
```
pip install esptool
```

# Howto connect esp01 with esplink firmware installed on to arduino controller

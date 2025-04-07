# arduino-playground


Best practices for files:
https://x.com/i/grok/share/0Xk9lrw38vMlnB26sEeREkOmc


## partitions

C:\Users\PSWIDERSKI\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.2.0\tools\partitions\huge_app.csv

## Libs sources

C:\Users\PSWIDERSKI\Desktop\ARDUINO\Sketches\libraries


## Enabling 192.168.4.1 access in Chrome

chrome://flags/#unsafely-treat-insecure-origin-as-secure

ws://192.168.4.1:443, ws://192.168.1.39:443

## Elemnts:

- Main board: ESP32-CAM
- Motion sensor: HC-SR501

## IMPORTANT

Failed to connect to ESP32: Wrong boot mode detected (0xb)

> SOLUTION -> GPIO2 must also be either left unconnected/floating, or driven Low, in order to enter the serial bootloader.


## ESP TOOLS OPTIONS 

usage: esptool [-h]
               [--chip {auto,esp8266,esp32,esp32s2,esp32s3,esp32c3,esp32c2,esp32c6,esp32c61,esp32c5,esp32h2,esp32h21,esp32p4,esp32h4}]
               [--port PORT] [--baud BAUD] [--port-filter PORT_FILTER]
               [--before {default_reset,usb_reset,no_reset,no_reset_no_sync}]
               [--after {hard_reset,soft_reset,no_reset,no_reset_stub,watchdog_reset}]
               [--no-stub] [--trace] [--override-vddsdio [{1.8V,1.9V,OFF}]]
               [--connect-attempts CONNECT_ATTEMPTS]
               {load_ram,dump_mem,read_mem,write_mem,write_flash,run,image_info,make_image,elf2image,read_mac,chip_id,flash_id,read_flash_status,write_flash_status,read_flash,verify_flash,erase_flash,erase_region,read_flash_sfdp,merge_bin,get_security_info,version} ...

esptool.py v4.8.9 - Espressif chips ROM Bootloader Utility

positional arguments:
  {load_ram,dump_mem,read_mem,write_mem,write_flash,run,image_info,make_image,elf2image,read_mac,chip_id,flash_id,read_flash_status,write_flash_status,read_flash,verify_flash,erase_flash,erase_region,read_flash_sfdp,merge_bin,get_security_info,version}
                        Run esptool.py {command} -h for additional help
    load_ram            Download an image to RAM and execute
    dump_mem            Dump arbitrary memory to disk
    read_mem            Read arbitrary memory location
    write_mem           Read-modify-write to arbitrary memory location
    write_flash         Write a binary blob to flash
    run                 Run application code in flash
    image_info          Dump headers from a binary file (bootloader or
                        application)
    make_image          Create an application image from binary files
    elf2image           Create an application image from ELF file
    read_mac            Read MAC address from OTP ROM
    chip_id             Read Chip ID from OTP ROM
    flash_id            Read SPI flash manufacturer and device ID
    read_flash_status   Read SPI flash status register
    write_flash_status  Write SPI flash status register
    read_flash          Read SPI flash content
    verify_flash        Verify a binary blob against flash
    erase_flash         Perform Chip Erase on SPI flash
    erase_region        Erase a region of the flash
    read_flash_sfdp     Read SPI flash SFDP (Serial Flash Discoverable
                        Parameters)
    merge_bin           Merge multiple raw binary files into a single file for
                        later flashing
    get_security_info   Get some security-related data
    version             Print esptool version

options:
  -h, --help            show this help message and exit
  --chip, -c {auto,esp8266,esp32,esp32s2,esp32s3,esp32c3,esp32c2,esp32c6,esp32c61,esp32c5,esp32h2,esp32h21,esp32p4,esp32h4}
                        Target chip type
  --port, -p PORT       Serial port device
  --baud, -b BAUD       Serial port baud rate used when flashing/reading
  --port-filter PORT_FILTER
                        Serial port device filter, can be vid=NUMBER,
                        pid=NUMBER, name=SUBSTRING, serial=SUBSTRING
  --before {default_reset,usb_reset,no_reset,no_reset_no_sync}
                        What to do before connecting to the chip
  --after, -a {hard_reset,soft_reset,no_reset,no_reset_stub,watchdog_reset}
                        What to do after esptool.py is finished
  --no-stub             Disable launching the flasher stub, only talk to ROM
                        bootloader. Some features will not be available.
  --trace, -t           Enable trace-level output of esptool.py interactions.
  --override-vddsdio [{1.8V,1.9V,OFF}]
                        Override ESP32 VDDSDIO internal voltage regulator (use
                        with care)
  --connect-attempts CONNECT_ATTEMPTS
                        Number of attempts to connect, negative or 0 for
                        infinite. Default: 7.

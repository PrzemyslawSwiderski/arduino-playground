@ECHO OFF

C:\Users\PSWIDERSKI\AppData\Local\Arduino15\packages\esp32\tools\mkspiffs\0.2.3\mkspiffs.exe -c^
 P:\GITHUBREPOS\arduino-playground\orzel7\data^
 -b 4096 -p 256 -s 0xE0000 P:\GITHUBREPOS\arduino-playground\spiffs\spiffs.bin

C:\Users\PSWIDERSKI\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\4.9.dev3\esptool.exe --chip esp32^
 --port COM3 --baud 115200 write_flash^
 -z 0x310000^
 P:\GITHUBREPOS\arduino-playground\spiffs\spiffs.bin

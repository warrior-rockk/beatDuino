###############################################################################
# Makefile for Arduino core & libraries
###############################################################################

## General options
MCU = atmega328p
CLOCK = 8000000
COMPILER = "Arduino"
ARDUINO = 105
SHELL = C:\Windows\system32\cmd.exe

## Tools general options
CCFLAGS = -w -ffunction-sections -fdata-sections -DARDUINO_ARCH_AVR -DF_CPU=$(CLOCK) -DARDUINO=$(ARDUINO) -mmcu=$(MCU) -I "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino" -I "C:/Archivos de programa/Arduino/hardware/arduino/variants/standard" -Os
ARFLAGS = ru

## Processing Tools
CC=avr-gcc
AR=ar

## Build tree:
all:	arduino/arduino.a

arduino/arduino.a:	arduino/CDC.o arduino/HardwareSerial.o arduino/HID.o arduino/IPAddress.o arduino/main.o arduino/new.o arduino/Print.o arduino/Stream.o arduino/Tone.o arduino/USBCore.o arduino/WInterrupts.o arduino/wiring.o arduino/wiring_analog.o arduino/wiring_digital.o arduino/wiring_pulse.o arduino/wiring_shift.o arduino/WMath.o arduino/WString.o arduino/avr-libc/malloc.o arduino/avr-libc/realloc.o arduino/EEPROM/EEPROM.o arduino/SPI/SPI.o arduino/Wire/Wire.o arduino/Wire/utility/twi.o 

arduino/CDC.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/CDC.cpp
	 $(CC) $(CCFLAGS) -o arduino/CDC.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/CDC.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/CDC.o

arduino/HardwareSerial.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp
	 $(CC) $(CCFLAGS) -o arduino/HardwareSerial.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HardwareSerial.o

arduino/HID.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/HID.cpp
	 $(CC) $(CCFLAGS) -o arduino/HID.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/HID.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HID.o

arduino/IPAddress.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/IPAddress.cpp
	 $(CC) $(CCFLAGS) -o arduino/IPAddress.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/IPAddress.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/IPAddress.o

arduino/main.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/main.cpp
	 $(CC) $(CCFLAGS) -o arduino/main.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/main.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/main.o

arduino/new.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/new.cpp
	 $(CC) $(CCFLAGS) -o arduino/new.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/new.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/new.o

arduino/Print.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/Print.cpp
	 $(CC) $(CCFLAGS) -o arduino/Print.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/Print.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Print.o

arduino/Stream.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/Stream.cpp
	 $(CC) $(CCFLAGS) -o arduino/Stream.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/Stream.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Stream.o

arduino/Tone.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/Tone.cpp
	 $(CC) $(CCFLAGS) -o arduino/Tone.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/Tone.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Tone.o

arduino/USBCore.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/USBCore.cpp
	 $(CC) $(CCFLAGS) -o arduino/USBCore.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/USBCore.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/USBCore.o

arduino/WInterrupts.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/WInterrupts.c
	 $(CC) $(CCFLAGS) -o arduino/WInterrupts.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/WInterrupts.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/WInterrupts.o

arduino/wiring.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/wiring.c
	 $(CC) $(CCFLAGS) -o arduino/wiring.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/wiring.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring.o

arduino/wiring_analog.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/wiring_analog.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_analog.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/wiring_analog.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_analog.o

arduino/wiring_digital.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/wiring_digital.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_digital.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/wiring_digital.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_digital.o

arduino/wiring_pulse.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/wiring_pulse.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_pulse.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/wiring_pulse.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_pulse.o

arduino/wiring_shift.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/wiring_shift.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_shift.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/wiring_shift.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_shift.o

arduino/WMath.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/WMath.cpp
	 $(CC) $(CCFLAGS) -o arduino/WMath.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/WMath.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/WMath.o

arduino/WString.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/WString.cpp
	 $(CC) $(CCFLAGS) -o arduino/WString.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/WString.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/WString.o

arduino/avr-libc/malloc.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/avr-libc/malloc.c
	 $(CC) $(CCFLAGS) -o arduino/avr-libc/malloc.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/avr-libc/malloc.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/avr-libc/malloc.o

arduino/avr-libc/realloc.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/cores/arduino/avr-libc/realloc.c
	 $(CC) $(CCFLAGS) -o arduino/avr-libc/realloc.o -c "C:/Archivos de programa/Arduino/hardware/arduino/cores/arduino/avr-libc/realloc.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/avr-libc/realloc.o

arduino/EEPROM/EEPROM.o:	C:/Archivos\ de\ programa/Arduino/libraries/EEPROM/EEPROM.cpp
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/libraries/EEPROM/utility" -I "C:/Archivos de programa/Arduino/libraries/EEPROM/avr" -I "C:/Archivos de programa/Arduino/libraries/EEPROM/utility/avr" -o arduino/EEPROM/EEPROM.o -c "C:/Archivos de programa/Arduino/libraries/EEPROM/EEPROM.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/EEPROM/EEPROM.o

arduino/SPI/SPI.o:	C:/Archivos\ de\ programa/Arduino/libraries/SPI/SPI.cpp
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/libraries/SPI/utility" -I "C:/Archivos de programa/Arduino/libraries/SPI/avr" -I "C:/Archivos de programa/Arduino/libraries/SPI/utility/avr" -o arduino/SPI/SPI.o -c "C:/Archivos de programa/Arduino/libraries/SPI/SPI.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/SPI/SPI.o

arduino/Wire/Wire.o:	C:/Archivos\ de\ programa/Arduino/libraries/Wire/Wire.cpp
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/libraries/Wire/utility" -I "C:/Archivos de programa/Arduino/libraries/Wire/avr" -I "C:/Archivos de programa/Arduino/libraries/Wire/utility/avr" -o arduino/Wire/Wire.o -c "C:/Archivos de programa/Arduino/libraries/Wire/Wire.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Wire/Wire.o

arduino/Wire/utility/twi.o:	C:/Archivos\ de\ programa/Arduino/libraries/Wire/utility/twi.c
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/libraries/Wire/utility" -I "C:/Archivos de programa/Arduino/libraries/Wire/avr" -I "C:/Archivos de programa/Arduino/libraries/Wire/utility/avr" -o arduino/Wire/utility/twi.o -c "C:/Archivos de programa/Arduino/libraries/Wire/utility/twi.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Wire/utility/twi.o

clean:	tidy
	 rm -f arduino/arduino.a

tidy:
	 rm -f arduino/CDC.o
	 rm -f arduino/HardwareSerial.o
	 rm -f arduino/HID.o
	 rm -f arduino/IPAddress.o
	 rm -f arduino/main.o
	 rm -f arduino/new.o
	 rm -f arduino/Print.o
	 rm -f arduino/Stream.o
	 rm -f arduino/Tone.o
	 rm -f arduino/USBCore.o
	 rm -f arduino/WInterrupts.o
	 rm -f arduino/wiring.o
	 rm -f arduino/wiring_analog.o
	 rm -f arduino/wiring_digital.o
	 rm -f arduino/wiring_pulse.o
	 rm -f arduino/wiring_shift.o
	 rm -f arduino/WMath.o
	 rm -f arduino/WString.o
	 rm -f arduino/avr-libc/malloc.o
	 rm -f arduino/avr-libc/realloc.o
	 rm -f arduino/EEPROM/EEPROM.o
	 rm -f arduino/SPI/SPI.o
	 rm -f arduino/Wire/Wire.o
	 rm -f arduino/Wire/utility/twi.o


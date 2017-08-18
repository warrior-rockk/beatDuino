###############################################################################
# Makefile for Arduino core & libraries
###############################################################################

## General options
MCU = atmega328p
CLOCK = 8000000
COMPILER = "Arduino"
ARDUINO = 183
SHELL = C:\Windows\system32\cmd.exe

## Tools general options
CCFLAGS = -w -ffunction-sections -fdata-sections -DARDUINO_ARCH_AVR -DF_CPU=$(CLOCK) -DARDUINO=$(ARDUINO) -mmcu=$(MCU) -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/variants/eightanaloginputs" -Os
ARFLAGS = ru

## Processing Tools
CC=avr-gcc
AR=ar

## Build tree:
all:	arduino/arduino.a

arduino/arduino.a:	arduino/abi.o arduino/CDC.o arduino/HardwareSerial.o arduino/HardwareSerial0.o arduino/HardwareSerial1.o arduino/HardwareSerial2.o arduino/HardwareSerial3.o arduino/hooks.o arduino/IPAddress.o arduino/main.o arduino/new.o arduino/PluggableUSB.o arduino/Print.o arduino/Stream.o arduino/Tone.o arduino/USBCore.o arduino/WInterrupts.o arduino/wiring.o arduino/wiring_analog.o arduino/wiring_digital.o arduino/wiring_pulse.o arduino/wiring_shift.o arduino/WMath.o arduino/WString.o arduino/SPI/SPI.o arduino/Wire/Wire.o arduino/Wire/utility/twi.o arduino/SoftwareSerial/SoftwareSerial.o 

arduino/abi.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/abi.cpp
	 $(CC) $(CCFLAGS) -o arduino/abi.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/abi.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/abi.o

arduino/CDC.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/CDC.cpp
	 $(CC) $(CCFLAGS) -o arduino/CDC.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/CDC.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/CDC.o

arduino/HardwareSerial.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial.cpp
	 $(CC) $(CCFLAGS) -o arduino/HardwareSerial.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HardwareSerial.o

arduino/HardwareSerial0.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial0.cpp
	 $(CC) $(CCFLAGS) -o arduino/HardwareSerial0.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial0.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HardwareSerial0.o

arduino/HardwareSerial1.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial1.cpp
	 $(CC) $(CCFLAGS) -o arduino/HardwareSerial1.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial1.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HardwareSerial1.o

arduino/HardwareSerial2.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial2.cpp
	 $(CC) $(CCFLAGS) -o arduino/HardwareSerial2.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial2.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HardwareSerial2.o

arduino/HardwareSerial3.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial3.cpp
	 $(CC) $(CCFLAGS) -o arduino/HardwareSerial3.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial3.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/HardwareSerial3.o

arduino/hooks.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/hooks.c
	 $(CC) $(CCFLAGS) -o arduino/hooks.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/hooks.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/hooks.o

arduino/IPAddress.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/IPAddress.cpp
	 $(CC) $(CCFLAGS) -o arduino/IPAddress.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/IPAddress.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/IPAddress.o

arduino/main.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/main.cpp
	 $(CC) $(CCFLAGS) -o arduino/main.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/main.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/main.o

arduino/new.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/new.cpp
	 $(CC) $(CCFLAGS) -o arduino/new.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/new.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/new.o

arduino/PluggableUSB.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/PluggableUSB.cpp
	 $(CC) $(CCFLAGS) -o arduino/PluggableUSB.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/PluggableUSB.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/PluggableUSB.o

arduino/Print.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/Print.cpp
	 $(CC) $(CCFLAGS) -o arduino/Print.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/Print.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Print.o

arduino/Stream.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/Stream.cpp
	 $(CC) $(CCFLAGS) -o arduino/Stream.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/Stream.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Stream.o

arduino/Tone.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/Tone.cpp
	 $(CC) $(CCFLAGS) -o arduino/Tone.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/Tone.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Tone.o

arduino/USBCore.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/USBCore.cpp
	 $(CC) $(CCFLAGS) -o arduino/USBCore.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/USBCore.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/USBCore.o

arduino/WInterrupts.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/WInterrupts.c
	 $(CC) $(CCFLAGS) -o arduino/WInterrupts.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/WInterrupts.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/WInterrupts.o

arduino/wiring.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/wiring.c
	 $(CC) $(CCFLAGS) -o arduino/wiring.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/wiring.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring.o

arduino/wiring_analog.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_analog.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_analog.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_analog.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_analog.o

arduino/wiring_digital.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_digital.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_digital.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_digital.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_digital.o

arduino/wiring_pulse.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_pulse.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_pulse.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_pulse.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_pulse.o

arduino/wiring_shift.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_shift.c
	 $(CC) $(CCFLAGS) -o arduino/wiring_shift.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/wiring_shift.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/wiring_shift.o

arduino/WMath.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/WMath.cpp
	 $(CC) $(CCFLAGS) -o arduino/WMath.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/WMath.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/WMath.o

arduino/WString.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/cores/arduino/WString.cpp
	 $(CC) $(CCFLAGS) -o arduino/WString.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/cores/arduino/WString.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/WString.o

arduino/SPI/SPI.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/libraries/SPI/src/SPI.cpp
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SPI/src/utility" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SPI/src/avr" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SPI/src/utility/avr" -o arduino/SPI/SPI.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SPI/src/SPI.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/SPI/SPI.o

arduino/Wire/Wire.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/libraries/Wire/src/Wire.cpp
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/utility" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/avr" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/utility/avr" -o arduino/Wire/Wire.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/Wire.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Wire/Wire.o

arduino/Wire/utility/twi.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/libraries/Wire/src/utility/twi.c
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/utility" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/avr" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/utility/avr" -o arduino/Wire/utility/twi.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/Wire/src/utility/twi.c"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/Wire/utility/twi.o

arduino/SoftwareSerial/SoftwareSerial.o:	C:/Archivos\ de\ programa/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/SoftwareSerial.cpp
	 $(CC) $(CCFLAGS) @arduino/libs.inc -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/utility" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/avr" -I "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/utility/avr" -o arduino/SoftwareSerial/SoftwareSerial.o -c "C:/Archivos de programa/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/SoftwareSerial.cpp"
	 $(AR) $(ARFLAGS) arduino/arduino.a arduino/SoftwareSerial/SoftwareSerial.o

clean:	tidy
	 rm -f arduino/arduino.a

tidy:
	 rm -f arduino/abi.o
	 rm -f arduino/CDC.o
	 rm -f arduino/HardwareSerial.o
	 rm -f arduino/HardwareSerial0.o
	 rm -f arduino/HardwareSerial1.o
	 rm -f arduino/HardwareSerial2.o
	 rm -f arduino/HardwareSerial3.o
	 rm -f arduino/hooks.o
	 rm -f arduino/IPAddress.o
	 rm -f arduino/main.o
	 rm -f arduino/new.o
	 rm -f arduino/PluggableUSB.o
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
	 rm -f arduino/SPI/SPI.o
	 rm -f arduino/Wire/Wire.o
	 rm -f arduino/Wire/utility/twi.o
	 rm -f arduino/SoftwareSerial/SoftwareSerial.o


###############################################################################
# Makefile for project 
###############################################################################

## General options
PROJECT = 
MCU = ATmega328P
COMPILER = "Arduino AVR"
TARGET = Debug
SHELL = C:\WINDOWS\system32\cmd.exe

## Tools general options
APPFLAGS=-C pro328 -N atmega328p -F 16000000
CCFLAGS=-Wall -gdwarf-2 -fno-exceptions -ffunction-sections -fdata-sections -DF_CPU=16000000 -I .. -DARDUINO_ARCH_AVR @arduino/libs.inc -O1 -mmcu=atmega328p -DDEBUG
LDFLAGS=-Wl,--gc-sections -mmcu=atmega328p

## Processing Tools
APP=mksketch
CC=avr-gcc
ASM=avr-gcc
LD=avr-gcc
#  E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/Debug.elf: TOOL=LD
#  Post: $(OBJ) avr-objcopy -O ihex -R .eeprom "./Debug.elf" "./Debug.hex"
#  Post: $(OBJ) avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex "./Debug.elf" "./Debug.eep" || exit 0 
#     E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/main.o: TOOL=CC
#        E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/main.cpp: TOOL=APP
#        Post: make -f arduino/Makefile all
#           E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/main.ino
#     E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/glcdfont.o: TOOL=CC
#        E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/glcdfont.c
#     E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/Adafruit_GFX.o: TOOL=CC
#        E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Adafruit_GFX.cpp
#     E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/Adafruit_SSD1306.o: TOOL=CC
#        E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Adafruit_SSD1306.cpp
#     E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/dataManagement.o: TOOL=CC
#        E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/dataManagement.cpp
#     E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/Debug/TimerOne.o: TOOL=CC
#        E:/Instalables/Proyectos/BeatDuino/project/arduino/Arduino 328/ATmega328P/TimerOne.cpp

# Build tree:
all:	Debug

Debug:	Debug.elf

Debug.elf:	main.o glcdfont.o Adafruit_GFX.o Adafruit_SSD1306.o dataManagement.o TimerOne.o 
	$(LD) $(LDFLAGS)  -o "./Debug.elf" "main.o" "glcdfont.o" "Adafruit_GFX.o" "Adafruit_SSD1306.o" "dataManagement.o" "TimerOne.o" arduino/arduino.a -lm
	$(OBJ) avr-objcopy -O ihex -R .eeprom "./Debug.elf" "./Debug.hex"
	$(OBJ) avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex "./Debug.elf" "./Debug.eep" || exit 0 

main.o:	main.cpp 
	$(CC) $(CCFLAGS)  -o "main.o" -c "main.cpp"

main.cpp:	../main.ino 
	$(APP) $(APPFLAGS)  -A "C:/Archivos de programa/Arduino" -O main.cpp ../main.ino
	make -f arduino/Makefile all

glcdfont.o:	../glcdfont.c 
	$(CC) $(CCFLAGS)  -o "glcdfont.o" -c "../glcdfont.c"

Adafruit_GFX.o:	../Adafruit_GFX.cpp 
	$(CC) $(CCFLAGS)  -o "Adafruit_GFX.o" -c "../Adafruit_GFX.cpp"

Adafruit_SSD1306.o:	../Adafruit_SSD1306.cpp 
	$(CC) $(CCFLAGS)  -o "Adafruit_SSD1306.o" -c "../Adafruit_SSD1306.cpp"

dataManagement.o:	../dataManagement.cpp 
	$(CC) $(CCFLAGS)  -o "dataManagement.o" -c "../dataManagement.cpp"

TimerOne.o:	../TimerOne.cpp 
	$(CC) $(CCFLAGS)  -o "TimerOne.o" -c "../TimerOne.cpp"

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf ./main.d
	rm -rf ./glcdfont.d
	rm -rf ./Adafruit_GFX.d
	rm -rf ./Adafruit_SSD1306.d
	rm -rf ./dataManagement.d
	rm -rf ./TimerOne.d
	-make -f arduino/Makefile tidy

# cleanup - delete all generated files
clean:	tidy
	rm -rf Debug.elf
	rm -rf main.o
	rm -rf main.cpp
	rm -rf glcdfont.o
	rm -rf Adafruit_GFX.o
	rm -rf Adafruit_SSD1306.o
	rm -rf dataManagement.o
	rm -rf TimerOne.o
	-make -f arduino/Makefile clean

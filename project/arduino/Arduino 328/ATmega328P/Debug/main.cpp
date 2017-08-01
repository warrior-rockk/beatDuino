#line 1 "../main.ino"
#include <Arduino.h>
#line 1
/*
 BeatDuino

 Metronomo digital programable con funciones de guardar repertorios y salida midi

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 v1.0	-	Release Inicial
 
 */

 #include <avr/wdt.h> 
//comunicacion con LCD OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 //notas musicales
#include <freqNotes.h>
//imagenes
#include <images.h>
void setup ();
void loop ();
void wellcomeTest ();
void doEncoder ();
#line 23

//Definiciones====================================

//version
#define majorVersion 1
#define minorVersion 0

//pines IO
#define ENC_A		2		
#define ENC_B   	1		
#define OUT_CLICK 	11		
#define LED_CLICK   13
#define OLED_RESET 	4

//config LCD
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//division nota
#define QUARTER		1
#define EIGHTH		2
#define SIXTEENTH   4

//==============================================

//Variables generales===========================

unsigned int bpm 			= 100;		//tempo general
unsigned long msTempo 		= 0;		//tempo en milisegundos
unsigned int clickDuration 	= 10;		//duración pulso click
unsigned long lastTime 		= 0;		//memoria tiempo anterior
boolean tick 				= true;		//flag de activar tick
unsigned int noteDivision	= SIXTEENTH;	//subdivision nota click
unsigned int barSignature   = 4;		//tipo compas
unsigned int actualTick     = 1;		//tiempo actual
//interfaz
unsigned int encPos 		= 0;		//posicion del encoder rotativo

//configuracion
void setup()
 { 
	//desactivamos el watchdog
	wdt_disable();
		
	//configuramos los pines
	pinMode(ENC_A,INPUT);
	pinMode(ENC_B,INPUT);
	pinMode(OUT_CLICK,OUTPUT);
	pinMode(LED_CLICK,OUTPUT);
	pinMode(OLED_RESET,OUTPUT); 
	//asignamos interrupcion a entrada encoder A	
	attachInterrupt(0, doEncoder, CHANGE);
	
	//inicializamos display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
	//Clear the buffer.
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	
	//Mensaje de inicio
	#ifndef DEBUG
		wellcomeTest();
	#endif
	
	//activamos el watchdog a 2 segundos
	wdt_enable(WDTO_2S);
 }

 //bucle principal
void loop()
 { 
	
	//conversion bpm
	bpm = map(encPos,0,100,1,300);
	
	//ms of actual tempo
	msTempo = (60000/bpm);
	
	//comprobamos si se cumple el siguiente tick si el tiempo supera el tiempo de negra dividido entre
	//la division de notas seleccionada
	if ((millis()-lastTime) >= (msTempo/noteDivision))
	{
		//realizamos tick
		tick = true;
		//incrementamos el numero de tick del compas
		actualTick == (barSignature*noteDivision) ? actualTick = 1 : actualTick++;
		lastTime = millis();		 
	}
	
	//actualizamos tick
	if (tick){
		//led
		digitalWrite(LED_CLICK, HIGH);
		//sonido del tick según si es el primer tiempo del compás
		if (actualTick == 1)
			tone(OUT_CLICK,NOTE_A4,clickDuration);
		else
			tone(OUT_CLICK,NOTE_C4,clickDuration);
		//desactivamos flag	
		tick = false;
	}
	else{
		digitalWrite(LED_CLICK, LOW);
		noTone(OUT_CLICK);
	}
	
	//actualizamos display
	display.clearDisplay();
	display.setCursor(0,0);
	display.print("BMP: "); 
	display.print(bpm); 
	display.display();
	
	//reseteamos el watchdog
	wdt_reset();
 }
 
 //funcion que realiza mensaje de inicio y test luces
void wellcomeTest()
{
	unsigned char welcome[] = {'B','e','a','t','D','u','i','n','o'};
	//unsigned char welcome2[] = {'v','e','r',' ',majorVersion+48,'.',minorVersion+48};
	unsigned char welcome3[] = {'b','y',' ','W','a','r','r','i','o','r'};
	
	int frames = 0;
	
	
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
    
    display.setCursor(0,0);
    for (int i=0;i<9;i++)
	{
		display.print(char(welcome[i]));
		display.display();
		delay(25);
		frames++;
	}
		
    /*display.setCursor(0,1);
	for (int i=0;i<7;i++)
	{
		lcd.print(char(welcome2[i]));
		delay(25);
		frames++;
	}*/
	
	delay(1000);
	
	display.print("\n");
	for (int i=0;i<10;i++)
	{
		display.print(char(welcome3[i]));
		display.display();
		delay(25);
		frames++;
	}
	
	delay(1000);
	
	display.drawBitmap(90, 20,  bmMetronome, 32, 33, 2);
	display.display();
	
	delay(1000);
}

//callback de la interrupcion 0 para leer el encoder
void doEncoder()
{
  //si el canal A y el B son iguales, estamos incrementando, si no, decrementando
  if (digitalRead(ENC_B) == digitalRead(ENC_A)) 
	encPos++;
  else
 	encPos--;  
}
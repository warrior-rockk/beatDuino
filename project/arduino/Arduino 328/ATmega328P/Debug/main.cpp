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
#include <EEPROM.h>
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
void playMetronome ();
void stopMetronome ();
void wellcomeTest ();
void doEncoder ();
void processButton (int pin ,int buttonNum );
#line 24

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
#define START_STOP  0

//botones
#define START_STOP_BT 0

//config LCD
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//division nota
#define QUARTER		1
#define EIGHTH		2
#define SIXTEENTH   4

//modos
#define METRONOME_MODE   0
#define LIVE_MODE        1

//repertorios y canciones
#define MAX_PLAYLISTS	5
#define MAX_SONGS       30
#define MAX_TITLE       15

//==============================================

//Estructuras===================================

//Estructura estado botones
struct button_
{
   bool pressed;
   bool longPress;
   bool pEdgePress;
   bool nEdgePress;
   unsigned int timerOn;
   unsigned int timerOff;
   unsigned int timerLong;
}button[2];

//tipo de estructura de una cancion
typedef struct
{
	String title;
	unsigned int tempo;
	unsigned int noteDivision;
	unsigned int beatSignature;
}song_;

//estructura de repetorio
struct playList_
{
	String title;
	song_ song[MAX_SONGS];
}playList;

//==============================================

//Variables generales===========================
unsigned int mode           = METRONOME_MODE;	//modo general
boolean refreshLCD			= false;			//refresco LCD
unsigned int bpm 			= 100;				//tempo general
unsigned long msTempo 		= 0;				//tempo en milisegundos
unsigned int clickDuration 	= 10;				//duración pulso click
unsigned long lastTime 		= 0;				//memoria tiempo anterior
unsigned int noteDivision	= QUARTER;			//subdivision nota click
unsigned int barSignature   = 4;				//tipo compas
unsigned int actualTick     = 1;				//tiempo actual
boolean tick 				= true;				//flag de activar tick
boolean play				= false;			//flag de activar metronomo
//interfaz
signed int deltaEnc             = 0;			//incremento o decremento del encoder
unsigned int buttonDelay     	= 2;			//Tiempo antirebote
unsigned int buttonLongPress 	= 60;			//Tiempo pulsacion larga para otras funciones

//configuracion
void setup()
 { 
	//desactivamos el watchdog
	wdt_disable();
	
	//configuramos los pines
	pinMode(ENC_A,INPUT_PULLUP);
	pinMode(ENC_B,INPUT_PULLUP);
	pinMode(START_STOP,INPUT_PULLUP);
	
	pinMode(OUT_CLICK,OUTPUT);
	pinMode(LED_CLICK,OUTPUT);
	pinMode(OLED_RESET,OUTPUT); 
	
	//asignamos interrupcion a entrada encoder A	
	attachInterrupt(0, doEncoder, CHANGE);
	
	//inicializamos display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
	
	//Mensaje de inicio
	#ifndef DEBUG
		wellcomeTest();
	#endif
	
	//Clear the buffer.
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	
	//activamos el watchdog a 2 segundos
	wdt_enable(WDTO_2S);
	
	/*playList[0].title = "Largo";
	playList[0].song[0].title = "De que se rie";
	playList[0].song[0].tempo = 168;
	playList[0].song[0].noteDivision = QUARTER;
	playList[0].song[0].beatSignature = 4;*/
 }

 //bucle principal
void loop()
 { 
	//comprobamos modo
	switch (mode)
	{
		//modo metronomo normal. Con la ruleta cambias el tempo.
		case METRONOME_MODE:
			//conversion bpm
			bpm = bpm + deltaEnc;
			deltaEnc = 0;
			
			//ms of actual tempo
			msTempo = (60000/bpm);
			
			//procesamos botones
			processButton(START_STOP,START_STOP_BT);
			
			//arranque-paro del sonido
			if (button[START_STOP_BT].pEdgePress)
				play = !play;
				
			//si está activado el sonido del metronomo
			if (play)
			{
				playMetronome();
			}
			else
			{
				stopMetronome();
			}
			
			refreshLCD = true;
			break;
	}
	
	
	//refresco LCD
	if (refreshLCD) {
		switch(mode)
		{
			case METRONOME_MODE:
				//actualizamos display del modo metronomo
				display.clearDisplay();
				display.setCursor(0,0);
				display.print("01.Revolviendo\n\n"); 
				display.setTextSize(2);
				display.print(bpm); 
				display.print(" BPM\n\n"); 
				display.setTextSize(1);
				display.print(barSignature);
				display.print("/");
				display.print(noteDivision*4);
				display.print("      ");
				play ? display.print("START") : display.print("STOP");
				display.display();
				
				break;
		}
		refreshLCD = false;
	}
	
	//reseteamos el watchdog
	wdt_reset();
 }

 //funcion que reproduce el metronomo al tempo y tipo de ritmo seleccionado
 void playMetronome()
 {
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
}

//funcion que detiene la reproduccion del metronomo
void stopMetronome()
{
	digitalWrite(LED_CLICK, LOW);
	noTone(OUT_CLICK);
	tick = false;
	actualTick = 0;
	lastTime = millis();
}
 //funcion que realiza mensaje de inicio y test luces
void wellcomeTest()
{
	unsigned char welcome[] = {'B','e','a','t','D','u','i','n','o'};
	unsigned char welcome2[] = {'v','e','r',' ',majorVersion+48,'.',minorVersion+48};
	unsigned char welcome3[] = {'b','y',' ','W','a','r','r','i','o','r'};
	
	int frames = 0;
	
	
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
    
    display.setCursor(0,0);
    for (unsigned int i=0;i<sizeof(welcome);i++)
	{
		display.print(char(welcome[i]));
		display.display();
		delay(25);
		frames++;
	}
		
    delay(1000);
	
	display.print("\n");
	for (unsigned int i=0;i<sizeof(welcome2);i++)
	{
		display.print(char(welcome2[i]));
		display.display();
		delay(25);
		frames++;
	}
	
	delay(1000);
	
	display.print("\n");
	for (unsigned int i=0;i<sizeof(welcome3);i++)
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
	deltaEnc++;
  else
 	deltaEnc--;  
}


//funcion que procesa un boton (estados y antirrebote)
void processButton(int pin,int buttonNum)
{
   //retardo activacion
   if (digitalRead(pin) == LOW)
      {		
	//reinicio tOff
	button[buttonNum].timerOff = 0;
	button[buttonNum].nEdgePress = false;
	//Flanco positivo si tOn vale 0
	button[buttonNum].pEdgePress = !button[buttonNum].pressed && button[buttonNum].timerOn >= buttonDelay;
	//contamos tiempo antirebote
	if (button[buttonNum].timerOn >= buttonDelay)
	    button[buttonNum].pressed = true;
	else
	    button[buttonNum].timerOn++;
   }
   
   //retardo desactivacion
   if (digitalRead(pin) == HIGH)
   {		
     //reinicio tOn,tLong y longPress
     button[buttonNum].timerOn = 0;
     button[buttonNum].timerLong = 0;
     button[buttonNum].longPress = false;
	 button[buttonNum].pEdgePress = false;
     //flanco negativo si tOff vale 0
     button[buttonNum].nEdgePress = button[buttonNum].pressed && button[buttonNum].timerOff >= buttonDelay;
     //contamos tiempo antirrebote
     if (button[buttonNum].timerOff >= buttonDelay)
	 button[buttonNum].pressed = false;
     else
	 button[buttonNum].timerOff++;
   }
   
   //pulsacion larga
   if (button[buttonNum].pressed)
   {
      //contamos pulsacion larga
      button[buttonNum].timerLong++;
      //activamos flag long en primer flanco de cuenta
      if (button[buttonNum].timerLong == buttonLongPress)
      	 button[buttonNum].longPress = true;	
      else
	 button[buttonNum].longPress = false;
	 
   }
}
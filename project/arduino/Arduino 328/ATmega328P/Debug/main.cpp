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
#include <avr/pgmspace.h>
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
//definiciones globales
#include <defines.h>
//funciones datos
#include <dataManagement.h>
void setup ();
void loop ();
void playMetronome ();
void stopMetronome ();
void wellcomeTest ();
void doEncoder ();
void processButton (int pin ,int buttonNum );
void refreshLCD ();
void readPlayListData ();
void readSongData ();
#line 28

//Definiciones====================================
//pines IO
#define START_STOP  0
#define ENC_B   	1		
#define ENC_A		2		
#define MENU_PIN   	3		
#define OLED_RESET 	4
#define ENTER_PIN	5
#define OUT_CLICK 	11		
#define LED_CLICK   13

//botones
#define START_STOP_BT 	0
#define MENU_BT 		1
#define ENTER_BT		2

//config LCD
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
//==============================================

//Constantes ===================================

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
}button[3];

//tipo de estructura de una cancion (13 bytes por cancion)
struct song_
{
	char title[MAX_SONG_TITLE];
	byte tempo;
	byte noteDivision;
	byte barSignature;
}actualSong;

//estructura de repetorio (398bytes por repertorio)
struct playList_
{
	char title[MAX_PLAYLIST_TITLE];
	byte numSong;
}actualPlayList;

//==============================================

//Variables generales===========================
byte mode          = LIVE_MODE;					//modo general
byte state         = MAIN_STATE;				//estado general
boolean refresh			= true;					//refresco LCD
unsigned int bpm 			= 100;				//tempo general
unsigned long msTempo 		= 0;				//tempo en milisegundos
unsigned int clickDuration 	= 10;				//duraci�n pulso click
unsigned long lastTime 		= 0;				//memoria tiempo anterior
unsigned int noteDivision	= QUARTER;			//subdivision nota click
unsigned int barSignature   = 4;				//tipo compas
unsigned int actualTick     = 1;				//tiempo actual
boolean tick 				= true;				//flag de activar tick
boolean play				= false;			//flag de activar metronomo
byte actualNumSong			= 0;				//cancion actual del repertorio
byte actualPlayListNum      = 0;				//numero de repetorio actual
//interfaz
signed int deltaEnc         = 0;				//incremento o decremento del encoder
unsigned int buttonDelay    = 2;				//Tiempo antirebote
unsigned int buttonLongPress= 60;				//Tiempo pulsacion larga para otras funciones
//menu
byte menuCategory           = MAIN_LIVE_CATEGORY;		//Categoria del menu actual
byte menuOption 			= CHANGE_PLAYLIST_OPTION;	//opcion seleccionada del menu
byte numMenuOptions			= 0;						//numero de opciones menu actual

//configuracion
void setup()
 { 
	//desactivamos el watchdog
	wdt_disable();
	
	//configuramos los pines
	pinMode(ENC_A,INPUT_PULLUP);
	pinMode(ENC_B,INPUT_PULLUP);
	pinMode(START_STOP,INPUT_PULLUP);
	pinMode(MENU_PIN,INPUT_PULLUP);
	pinMode(ENTER_PIN,INPUT_PULLUP);
	
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
	
	//prueba de elementos
	//debugWriteSongs();
	//debugWritePlayLists();
	
	//leemos los datos actuales
	readPlayListData();
	readSongData();
 }

 //bucle principal
void loop()
 { 
	//procesamos botones
	processButton(START_STOP,START_STOP_BT);
	processButton(MENU_PIN,MENU_BT);
	processButton(ENTER_PIN,ENTER_BT);
	
	//si pulsamos boton menu
	if (button[MENU_BT].pEdgePress)
	{
		state = MENU_STATE;
		menuOption = 0;
		menuCategory = 0;
		numMenuOptions = 4;
		refresh = true;
	}		
	
	//comprobamos estado
	switch (state)
	{
		//estado principal
		case MAIN_STATE: 
			//comprobamos modo
			switch (mode)
			{
				//modo directo. Eliges un repetorio y con la ruleta subes y bajas de tema.
				case LIVE_MODE:
					//cambio de cancion
					if (deltaEnc > 0 && actualNumSong < (MAX_SONGS-1))
					{
						actualNumSong++;
						readSongData();
						refresh = true;
					}
					if (deltaEnc < 0 && actualNumSong > 0)
					{
						actualNumSong--;
						readSongData();
						refresh = true;
					}
					
					deltaEnc = 0;
					
					//obtenemos datos tema
					bpm 			= actualSong.tempo;
					noteDivision 	= actualSong.noteDivision;
					barSignature 	= actualSong.barSignature;
			
					//ms of actual tempo
					msTempo = (60000/bpm);
					
					//arranque-paro del sonido
					if (button[START_STOP_BT].pEdgePress)
					{
						play = !play;
						refresh = true;
					}
					
					break;
					
				//modo metronomo normal. Con la ruleta cambias el tempo.
				case METRONOME_MODE:
					//conversion bpm
					bpm = bpm + deltaEnc;
					deltaEnc = 0;
					
					//ms of actual tempo
					msTempo = (60000/bpm);
					
					//arranque-paro del sonido
					if (button[START_STOP_BT].pEdgePress)
						play = !play;
						
					refresh = true;
					
					break;
			}
			break;

		//estado menu (generico para todos los modos)
		case MENU_STATE:
			//cambio de opcion
			if (deltaEnc > 0 )
			{
				menuOption < numMenuOptions-1 ? menuOption++ : menuOption=0;
				refresh = true;
			}
			if (deltaEnc < 0 )
			{
				menuOption > 0 ? menuOption-- : menuOption = numMenuOptions-1;
				refresh = true;
			}
			
			deltaEnc = 0;
			
			//si pulsamos enter
			if (button[ENTER_BT].pEdgePress)
			{
				switch (menuCategory)
				{
					case MAIN_LIVE_CATEGORY:
						switch (menuOption)
						{
							case CHANGE_PLAYLIST_OPTION:
								menuOption = 0;
								menuCategory = PLAYLIST_CHANGE_CATEGORY;
								numMenuOptions = MAX_PLAYLISTS;
								refresh = true;
								break;							
							case CHANGE_SONG_OPTION:
								menuOption = 0;
								menuCategory = SONG_CHANGE_CATEGORY;
								numMenuOptions = MAX_SONGS;
								refresh = true;
								break;							
						}
						break;
					case PLAYLIST_CHANGE_CATEGORY:
						//cambio de playlist
						actualPlayListNum = menuOption;
						actualNumSong=0;
						readPlayListData();
						readSongData();
						state = MAIN_STATE;
						refresh = true;
						
						break;
					case SONG_CHANGE_CATEGORY:
						//cambio de canci�n
						WritePlayListSong(actualPlayListNum,actualNumSong,menuOption);
						readSongData();
						state = MAIN_STATE;
						refresh = true;
						
						break;
				}						
			}
			
			break;
	}
	
	//si est� activado el sonido del metronomo
	if (play)
	{
		playMetronome();
	}
	else
	{
		stopMetronome();
	}
					
	//refresco interfaz
	if (refresh) {
		refreshLCD();
		refresh = false;
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
		//sonido del tick seg�n si es el primer tiempo del comp�s
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

//funcion para el refresco del LCD
void refreshLCD()
{
	//Segun modo
	switch(mode)
	{
		case LIVE_MODE:
			switch (state)
			{
				case MAIN_STATE:
					//actualizamos display del modo directo
					display.clearDisplay();
					display.setCursor(0,0);
					display.setTextColor(WHITE,BLACK);
					display.print(actualPlayListNum+1);
					display.print(".");
					display.print(actualPlayList.title);
					display.print("\n"); 
					display.print(actualNumSong+1);
					display.print(".");
					display.print(actualSong.title);
					display.print("\n\n"); 
					display.setTextSize(2);
					display.print(bpm); 
					display.print(" BPM\n\n"); 
					display.setTextSize(1);
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					display.print("      ");
					play ? display.print(F("START")) : display.print("STOP");
					display.display();
					break;
				case MENU_STATE:
					//actualizamos display del modo directo en menu
					display.clearDisplay();
					display.setCursor(0,0);
					//segun la categoria del menu
					switch (menuCategory)
					{
						case MAIN_LIVE_CATEGORY:
							menuOption == 0 ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
							display.println(F("Cambiar Repertorio"));
							menuOption == 1 ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
							display.println(F("Cambiar Cancion"));
							menuOption == 2 ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
							display.println(F("Insertar Cancion"));
							menuOption == 3 ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
							display.println(F("Eliminar Cancion"));
							break;
						case PLAYLIST_CHANGE_CATEGORY:
							for (int i=0;i<MAX_PLAYLISTS;i++)
							{
								menuOption == i ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
								char * title = readPlayListTitle(i);
								display.print(i+1);
								display.print(".");
								display.println(title);
								free (title);
							}
							break;
						case SONG_CHANGE_CATEGORY:
							display.println(F("Escoge la cancion:"));
							display.setTextColor(WHITE,BLACK);
							char * title = readSongTitle(menuOption);
							display.print(menuOption+1);
							display.print(".");
							display.println(title);
							free (title);
							
							break;
					}
					//mostramos pantalla
					display.display();
					break;
			}		
			break;
		case METRONOME_MODE:
			//actualizamos display del modo metronomo
			display.clearDisplay();
			display.setCursor(0,0);
			display.setTextColor(WHITE,BLACK);
			display.setTextSize(2);
			display.print(bpm); 
			display.print(" BPM\n\n"); 
			display.setTextSize(1);
			display.print(barSignature);
			display.print("/");
			display.print(noteDivision*4);
			display.print("      ");
			play ? display.print(F("START")) : display.print("STOP");
			display.display();
			
			break;
	}
}

//funcion para leer la informacion del playlist actual de EEPROM
void readPlayListData()
{
	//leemos el titulo del playList actual
	char * playListTitle = readPlayListTitle(actualPlayListNum);
	strcpy(actualPlayList.title,playListTitle);
	free(playListTitle);	
}

//funcion para leer la informacion de la cancion actual de EEPROM
void readSongData()
{
	//obtenemos el numero de cancion de la posicion actual del playlist
	byte actualSongPos = getSongNum(actualPlayListNum,actualNumSong);
	
	//leemos el titulo de la cancion actual
	char * songTitle = readSongTitle(actualSongPos);
	strcpy(actualSong.title,songTitle);
	free(songTitle);
	//leemos el tempo de la cancion actual
	actualSong.tempo = getSongTempo(actualSongPos);
	//leemos la division de nota de la cancion actual
	actualSong.noteDivision = getSongNoteDivision(actualSongPos);
	//leemos el compas de la cancion actual
	actualSong.barSignature = getSongBarSignature(actualSongPos);		
}
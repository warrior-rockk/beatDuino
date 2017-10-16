/*
 BeatDuino

 Metronomo digital programable con funciones de guardar repertorios y salida midi

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 TO-DO:
	-no se oyen los sonidos de prueba
	-indicar modo marcado en el menu de cambiarlo
	-comprobacion de rango de valores en el load config
	-que al cancelar la seleccion de una opcion, vuelva el valor original (punteros?)
	-solo debe leer la informacion de cancion al cambiar opcion si estas en modo live
	-un menu de repertorio que salgan mas de un registro y te desplaces en lista?
	-se esta refrescando lo minimo? ver ciclo de trabajo?
		
 v1.0	-	Release Inicial
 
 */
#include <avr/wdt.h> 
#include <avr/pgmspace.h>
#include <EEPROM.h>
//comunicacion con LCD OLED
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
//definiciones globales
#include <defines.h>
//textos
#include <strings.h>
//funciones datos
#include <dataManagement.h>
//midi
#include <TimerOne.h>

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

//estructura constante opciones menu
const struct {
	byte numOptions;
	byte prevPage;
    const char* const* strTable;
} menuPage[29] = {	4,MAIN_PAGE,mainStr,
					3,MAIN_PAGE,playListStr,
					MAX_PLAYLISTS,PLAYLIST_PAGE,NULL,
					2,PLAYLIST_PAGE,editPlayListStr,
					0,PLAYLIST_EDIT_PAGE,NULL,
					4,PLAYLIST_PAGE,changeOrderStr,
					MAX_SONGS,ORDER_PAGE,NULL,
					MAX_SONGS,ORDER_PAGE,NULL,
					MAX_SONGS-1,ORDER_PAGE,NULL, 
					MAX_SONGS,ORDER_PAGE,NULL,
					MAX_SONGS,ORDER_PAGE,NULL,
					2,ORDER_PAGE,confirmStr,
					2,PLAYLIST_PAGE,confirmStr,
					2,MAIN_PAGE,songStr,
					MAX_SONGS,SONG_PAGE,NULL,
					4,SELECT_EDIT_SONG_PAGE,editSongStr,
					0,EDIT_SONG_PAGE,NULL,
					255,EDIT_SONG_PAGE,NULL,
					3,EDIT_SONG_PAGE,noteDivisionStr,
					6,EDIT_SONG_PAGE,NULL,
					MAX_SONGS,SONG_PAGE,NULL,
					2,SONG_PAGE,confirmStr,
					5,MAIN_PAGE,settingsStr,
					2,SETTINGS_PAGE,modeStr,
					2,SETTINGS_PAGE,confirmStr,
					3,SETTINGS_PAGE,soundsStr,
					2,SETTINGS_PAGE,confirmStr,					
					2,SETTINGS_PAGE,confirmStr,					
					0,MAIN_PAGE,NULL,
				};

const unsigned int buttonDelay    	= 5;				//Tiempo antirebote (*10ms)
const unsigned int buttonLongPress	= 200;				//Tiempo pulsacion larga para otras funciones (*10ms)				

//==============================================

//Variables generales===========================
byte mode          	= METRONOME_MODE;			//modo general
byte state         	= MAIN_STATE;				//estado general
byte lastState		= 255;						//estado anterior
boolean refresh		= true;						//refresco LCD
unsigned int bpm 					= 100;//160;		//tempo general
unsigned int lastBpm				= 0;		//tempo anterior
unsigned int clickDuration 			= 50;		//duraci�n pulso click
unsigned long clickLastTime			= 0;		//cuenta del inicio pulso click
unsigned int noteDivision			= QUARTER;	//subdivision nota click
unsigned int barSignature   		= 4;		//tipo compas
volatile unsigned int actualTick    = 0;		//tiempo actual
byte tickSound				= SND_1;			//sonido del tick
boolean tick 				= true;				//flag de activar tick
boolean play				= true;				//flag de activar metronomo
boolean equalTicks			= false;			//flag de mismo sonido para todos los ticks
byte actualNumSong			= 0;				//cancion actual del repertorio
byte actualPlayListNum      = 0;				//numero de repetorio actual
//midi
boolean midiClock			= false;			//flag de envio de midi clock
float midiClockTime;							//intervalo midi interrupcion
volatile byte midiCounter	= 0;				//contador clocks midi
//display Oled
SSD1306AsciiAvrI2c display;
//interfaz
volatile signed int deltaEnc= 0;				//incremento o decremento del encoder
unsigned long countClockTime= 0;				//contador de tiempo para las bases 10ms
boolean clock10ms			= false;			//flanco de 10ms
//menu
byte actualMenuPage         = MAIN_PAGE;		//p�gina del menu actual
byte lastMenuPage			= 255;				//pagina anterior del menu
byte actualMenuOption 		= CHANGE_PLAYLIST_OPTION;	//opcion seleccionada del menu
//entrada texto o parametros
byte editCursor             = 0;				//posicion cursor edicion
char * editString;								//cadena a editar
byte editData				= 0;				//dato a editar
unsigned int editDataInt	= 0;				//dato a editar entero
byte editSelection			= 0;				//seleccion a editar
//debug
unsigned long startTime     = 0;				//tiempo de inicio de ejecucion ciclo para medir rendimiento
unsigned long lastCycleTime = 0;				//tiempo que tardo el ultimo ciclo
unsigned long minCycleTime  = 2000000;			//tiempo de ciclo minimo
unsigned long maxCycleTime  = 0;				//tiempo de ciclo maximo
byte general;

//================================

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
	
	pinMode(BEAT1_CLICK,OUTPUT);
	pinMode(SND0_CLICK,OUTPUT);
	pinMode(SND1_CLICK,OUTPUT);
	
	pinMode(LED_CLICK,OUTPUT);
	pinMode(OLED_RESET,OUTPUT); 
	
	//asignamos interrupcion a entrada encoder A	
	attachInterrupt(0, doEncoder, CHANGE);
	
	//inicializamos display
	#ifdef DEBUG
		display.begin(&Adafruit128x64, 0x3D);	//adafruit 0,96" y simulacion
	#else
		display.begin(&Adafruit128x64, 0x3C);	//display grande ebay
	#endif
	
	display.setFont(Adafruit5x7); 
  
	//Midi Baud Rate 
	Serial.begin(31250);
	
	//Mensaje de inicio
	#ifndef DEBUG
		wellcomeTest();
	#endif
	
	//Clear the buffer.
	display.clear();
	display.set1X();
	display.setBlackText(false);
	
	//iniciamos la interrupcion del metronomo ((temporal)
	midiClockTime = (float)((float)(60000/bpm)/MIDI_TICKS_BEAT)*1000;
	Timer1.initialize(midiClockTime); //uSecs
	Timer1.attachInterrupt(sendMidiClock);
	
	//activamos el watchdog a 2 segundos
	wdt_enable(WDTO_2S);
	
	//prueba de elementos
	//debugWriteSongs();
	//debugWritePlayLists();
	
	//leemos la configuracion
	loadConfig();
	
	//leemos los datos actuales
	readPlayListData();
	readSongData();
 
}

void loop()
{ 
	//inciio ejecucion
	startTime = micros();
		
	//procesamos botones
	processButton(START_STOP,START_STOP_BT);
	processButton(MENU_PIN,MENU_BT);
	processButton(ENTER_PIN,ENTER_BT);
	
	//si pulsamos boton menu/cancelar-atras
	if (button[MENU_BT].pEdgePress)
	{
		doMenuButton();
	}		
	
	//comprobamos estado
	switch (state)
	{
		//estado principal
		case MAIN_STATE: 
			doMainState();
			break;
		//estado menu (generico para todos los modos)
		case MENU_STATE:
			doMenuState();
			break;
	}
		
	//si ha cambiado el tempo, recalculamos la interrupcion
	if (lastBpm != bpm)
	{
		midiClockTime = (float)((float)(60000/bpm)/MIDI_TICKS_BEAT)*1000;
		Timer1.setPeriod(midiClockTime); //uSecs
		lastBpm = bpm;
	}
	
	//refresco interfaz
	if (refresh) {
		refreshLCD();
		refresh = false;
	}
	
	//flanco cada 10ms
	if (millis()- countClockTime >= 10)
	{
		countClockTime = millis();
		clock10ms = true;
	}
	else
		clock10ms = false;
		
	//calculo del tiempo de ciclo,maximo y minimo
	lastCycleTime = micros() - startTime;
	if (lastCycleTime > maxCycleTime)
		maxCycleTime = lastCycleTime;
	if (lastCycleTime < minCycleTime)
		minCycleTime = lastCycleTime;

	//resetemos valor encoder
	if (digitalRead(ENC_B) == digitalRead(ENC_A))
		deltaEnc = 0;
		
	//reseteamos el watchdog
	wdt_reset();
 
}

//callback de la interrupcion que se ejecuta 24 veces por negra al tempo actual
void sendMidiClock()
{
	//si se cumple el ancho de pulso del tick, quitamos las se�ales de tick
	if (millis() - clickLastTime > clickDuration)
		PORTB &= 0xF8;
		
	//comprobamos iteraccion del midiClock
	if (midiCounter >= (MIDI_TICKS_BEAT/noteDivision))
	{
		midiCounter = 0;
		//incrementamos el numero de tick del compas
		actualTick >= (barSignature-1) ? actualTick = 0 : actualTick++;
		//si est� en reproduccion
		if (play)
		{
			//guardamos tiempo inicio tick
			clickLastTime = millis();
			//sonido del tick seg�n si es el primer tiempo del comp�s y no est� configurado ticks iguales
			PORTB |= ((tickSound*2)+2) + (actualTick == 0 && !equalTicks);							
		}
	}
	else
		midiCounter++;
	
	//Enviamos un midi clock cada 24 veces por negra)
	if (midiClock)
		Serial.write(MIDI_CLOCK_MSG);	
}

//acciones del bot�n de menu/atras
void doMenuButton()
{
	//comprobamos estado
	switch (state)
	{
		//si est� en el menu
		case MENU_STATE:
			//si esta en la pagina inicial, sale del menu
			if (actualMenuPage == MAIN_PAGE)
			{
				state = MAIN_STATE;					
			}
			else //si no, va a la pagina previa
			{
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				actualMenuOption = 0;
			}
			break;
		//si no esta en el menu, salta al menu
		default:
			state = MENU_STATE;
			actualMenuOption = 0;
			actualMenuPage = MAIN_PAGE;
			
			break;
	}	
	refresh = true;
}

//acciones en el estado principal
void doMainState()
{
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
				deltaEnc = 0; //para que no se mueva
				refresh = true;
			}
			if (deltaEnc < 0 && actualNumSong > 0)
			{
				actualNumSong--;
				readSongData();
				deltaEnc = 0; //para que no se mueva
				refresh = true;
			}
								
			//obtenemos datos tema
			bpm 			= actualSong.tempo;
			noteDivision 	= actualSong.noteDivision;
			barSignature 	= actualSong.barSignature;

			//arranque-paro del sonido
			if (button[START_STOP_BT].pEdgePress)
			{
				play = !play;
				refresh = true;
			}
			
			break;
			
		//modo metronomo normal. Con la ruleta cambias el tempo.
		case METRONOME_MODE:
			//cambio tempo
			if (deltaEnc > 0 )
			{
				if (bpm < 255)
					bpm++;
				
				deltaEnc = 0;
				refresh = true;
			}
			if (deltaEnc < 0 )
			{
				if (bpm > 0)
					bpm--;
				
				deltaEnc = 0;
				refresh = true;
			}
						
			//arranque-paro del sonido
			if (button[START_STOP_BT].pEdgePress)
			{
				play = !play;
				refresh = true;
			}	
								
			break;
	}
}

//acciones para el estado Menu
void doMenuState()
{
	//cambio de opcion
	if (deltaEnc > 0 )
	{
		actualMenuOption < menuPage[actualMenuPage].numOptions-1 ? actualMenuOption++ : actualMenuOption=0;
		//si tiene el numero de opciones definido, reseteamos el encoder
		if (menuPage[actualMenuPage].numOptions != 0)
			deltaEnc = 0; //para que no se mueva						
		refresh = true;
	}
	if (deltaEnc < 0 )
	{
		actualMenuOption > 0 ? actualMenuOption-- : actualMenuOption = menuPage[actualMenuPage].numOptions-1;
		//si tiene el numero de opciones definido, reseteamos el encoder
		if (menuPage[actualMenuPage].numOptions != 0)
			deltaEnc = 0; //para que no se mueva						
		refresh = true;				
	}

	//si pulsamos enter
	if (button[ENTER_BT].pEdgePress)
	{
		switch (actualMenuPage)
		{
			case MAIN_PAGE:
				switch (actualMenuOption)
				{
					case PLAYLIST_OPTION:
						actualMenuOption = 0;
						actualMenuPage = PLAYLIST_PAGE;
						
						break;		
					case SONG_OPTION:
						actualMenuOption = 0;
						actualMenuPage = SONG_PAGE;
						
						break;	
					case SETTINGS_OPTION:
						actualMenuOption = 0;
						actualMenuPage = SETTINGS_PAGE;
						
						break;
					case INFO_OPTION:
						actualMenuOption = 0;
						actualMenuPage = INFO_PAGE;
						
						break;
				}
				break;
			case PLAYLIST_PAGE:
				switch (actualMenuOption)
				{
					case CHANGE_PLAYLIST_OPTION:
						actualMenuOption = 0;
						actualMenuPage = PLAYLIST_CHANGE_PAGE;
						
						break;							
					case EDIT_PLAYLIST_OPTION:
						actualMenuOption = 0;
						actualMenuPage = PLAYLIST_EDIT_PAGE;
						
						break;		
					case DELETE_PLAYLIST_OPTION:
						actualMenuOption = 0;
						actualMenuPage = PLAYLIST_DELETE_PAGE;
						
						break;	
				}
				break;
			case PLAYLIST_CHANGE_PAGE:
				//cambio de playlist
				actualPlayListNum = actualMenuOption;
				actualNumSong=0;
				readPlayListData();
				readSongData();
				state = MAIN_STATE;
				
				break;
			case PLAYLIST_EDIT_PAGE:
				switch (actualMenuOption)
				{
					//cambio nombre playlist
					case NAME_PLAYLIST_OPTION:
						actualMenuOption = 0;
						actualMenuPage = PLAYLIST_NAME_PAGE;
						
						//obtenemos el nombre para editarlo
						editString = readPlayListTitle(actualPlayListNum);
						editCursor = 0;								
						break;
					//cambio ordenes
					case ORDER_OPTION:
						actualMenuOption = 0;
						actualMenuPage = ORDER_PAGE;
						
						break;
				}
				break;
			case PLAYLIST_NAME_PAGE:
				//avanzamos el cursor de edicion de nombre o aceptamos cadena si el ultimo caracter es un enter
				if ((byte)editString[editCursor] == 216)
				{
					//sustituimos ultimo caracter enter
					editString[editCursor] = '\0';
					//guardamos titulo
					writePlayListTitle(actualPlayListNum,editString);
					free(editString);
					//salimos del menu
					actualMenuPage = menuPage[actualMenuPage].prevPage;
					actualMenuOption = 0;
					readPlayListData();
				}
				else 
				{
					//avanzamos cursor
					editCursor < MAX_PLAYLIST_TITLE ? editCursor++ : editCursor = 0;							
				}
				break;
			case ORDER_PAGE:
				switch (actualMenuOption)
				{
					//cambiar el orden
					case CHANGE_ORDER_OPTION:
						actualMenuOption = 0;
						actualMenuPage = CHANGE_ORDER_PAGE;
						
						editData = 0;
						break;
					//insertar cancion
					case INSERT_SONG_OPTION:
						actualMenuOption = 0;
						actualMenuPage = INSERT_SONG_PAGE;
						
						editData = 0;
						break;
					//borrar cancion
					case DELETE_SONG_OPTION:
						actualMenuOption = 0;
						actualMenuPage = DELETE_SONG_PAGE;
						
						editData = 0;
						break;
					//vaciar orden
					case EMPTY_ORDER_OPTION:
						actualMenuOption = 0;
						actualMenuPage = EMPTY_ORDER_PAGE;
						
						break;
				}
				break;
			case CHANGE_ORDER_PAGE:  //elegimos posicion del orden
				editData = actualMenuOption;
				actualMenuOption = 0;
				actualMenuPage = CHANGE_ORDER_PAGE_2;
				
				break;
			case CHANGE_ORDER_PAGE_2:
				//aceptamos el cambio de canci�n
				writePlayListSong(actualPlayListNum,editData,actualMenuOption);
				actualMenuOption=editData;
				editData=0;
				actualMenuPage = CHANGE_ORDER_PAGE;
				
				readSongData();
				break;
			case INSERT_SONG_PAGE:  
				//elegimos posicion de la inserccion
				editData = actualMenuOption;
				actualMenuOption = 0;
				actualMenuPage = INSERT_SONG_PAGE_2;
				
				break; 
			case INSERT_SONG_PAGE_2: 	
				//insertamos la cancion
				
				//movemos las canciones una posicion desde el final de la lista hasta la posicion de inserccion
				for (int i=MAX_SONGS-1;i>editData+1;i--)
				{
					writePlayListSong(actualPlayListNum,i,getSongNum(actualPlayListNum,i-1));
				}
				//escribimos la nueva cancion en la posicion de inserccion
				writePlayListSong(actualPlayListNum,editData+1,actualMenuOption);
				
				actualMenuOption=0;
				editData=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
									
				break;
			case DELETE_SONG_PAGE: 	
				//borramos la cancion. Movemos el resto de canciones una posicion hasta el final de la lista
				for (int i=actualMenuOption;i<MAX_SONGS-1;i++)
				{
					writePlayListSong(actualPlayListNum,i,getSongNum(actualPlayListNum,i+1));
				}
				
				actualMenuOption=0;
				editData=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
				break;
			case EMPTY_ORDER_PAGE: 	
				//vaciamos el orden
				if (actualMenuOption == 1) //SI
				{
					for (int i=0;i<MAX_SONGS;i++)
					{
						writePlayListSong(actualPlayListNum,i,0xFF);
					}
				}
										
				actualMenuOption=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
				
				break;
			case PLAYLIST_DELETE_PAGE: 	
				{
				//vaciamos el orden y quitamos el nombre al playlist
				if (actualMenuOption == 1) //SI
				{
					for (int i=0;i<MAX_SONGS;i++)
					{
						writePlayListSong(actualPlayListNum,i,0xFF);
					}
				}
				
				//vaciamos titulo playlist
				char title[MAX_PLAYLIST_TITLE];
				title[0] = (byte)0xFF;
				strncpy(title,title,MAX_PLAYLIST_TITLE);
				writePlayListTitle(actualPlayListNum,title);						
				
				actualMenuOption=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readPlayListData();
								
				}
				break;
			case SONG_PAGE:
				switch (actualMenuOption)
				{
					case EDIT_SONG_OPTION:
						actualMenuOption = 0;
						actualMenuPage = SELECT_EDIT_SONG_PAGE;
						break;
					case EMPTY_SONG_OPTION:
						actualMenuOption = 0;
						actualMenuPage = SELECT_EMPTY_SONG_PAGE;
						break;
				}
				break;
			case SELECT_EDIT_SONG_PAGE:
				{
				editSelection = actualMenuOption; //num cancion a editar
				actualMenuOption = 0;
				actualMenuPage = EDIT_SONG_PAGE;
				}
				break;
			case EDIT_SONG_PAGE:
				switch (actualMenuOption)
				{
					case CHANGE_SONG_NAME_OPTION:
						actualMenuOption = 0;
						actualMenuPage = CHANGE_SONG_NAME_PAGE;
						editString = readSongTitle(editSelection);
						break;
					case CHANGE_SONG_TEMPO_OPTION:
						actualMenuPage = CHANGE_SONG_TEMPO_PAGE;
						editData = getSongTempo(editSelection);
						actualMenuOption = editData;
						break;
					case CHANGE_SONG_NOTE_OPTION:
						actualMenuPage = CHANGE_SONG_NOTE_PAGE;
						editData = getSongNoteDivision(editSelection);
						switch (editData)
						{
							case QUARTER:
								actualMenuOption = 0;								
								break;
							case EIGHTH:
								actualMenuOption = 1;						
								break;
							case SIXTEENTH:
								actualMenuOption = 2;						
								break;
						}
						break;
					case CHANGE_SONG_BEAT_OPTION:
						actualMenuPage = CHANGE_SONG_BEAT_PAGE;
						editData = getSongBarSignature(editSelection);
						actualMenuOption = editData-2;
						break;
				}
				break;
			case CHANGE_SONG_NAME_PAGE:
				//avanzamos el cursor de edicion de nombre o aceptamos cadena si el ultimo caracter es un enter
				if ((byte)editString[editCursor] == 216)
				{
					//sustituimos ultimo caracter enter
					editString[editCursor] = '\0';
					//guardamos el nuevo nombre
					writeSongTitle(editSelection,editString);
					free(editString);
					actualMenuOption=0;
					actualMenuPage = menuPage[actualMenuPage].prevPage;
					readSongData();							
				}
				else 
				{
					//avanzamos cursor
					editCursor < MAX_SONG_TITLE ? editCursor++ : editCursor = 0;							
				}
				break;
			case CHANGE_SONG_TEMPO_PAGE:
				writeSongTempo(editSelection,actualMenuOption);
				actualMenuOption=0;
				editData = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
				break;
			case CHANGE_SONG_NOTE_PAGE:
				switch (actualMenuOption)
				{
					case 0:
						writeSongNoteDivision(editSelection,QUARTER);
						break;
					case 1:
						writeSongNoteDivision(editSelection,EIGHTH);
						break;
					case 2:
						writeSongNoteDivision(editSelection,SIXTEENTH);
						break;
				}
				editData = 0;
				actualMenuOption=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
				break;
			case CHANGE_SONG_BEAT_PAGE:
				writeSongBeatSignature(editSelection,actualMenuOption+2);
				editData = 0;
				actualMenuOption=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
				break;
			case SELECT_EMPTY_SONG_PAGE:
				{
				editSelection = actualMenuOption; //num cancion a editar
				actualMenuOption = 0;
				actualMenuPage = EMPTY_SONG_PAGE;
				}
				break;
			case EMPTY_SONG_PAGE:
				{
				//vaciamos la cancion de datos
				if (actualMenuOption == 1) //SI
				{
						//vaciamos titulo cancion
						char title[MAX_SONG_TITLE];
						title[0] = (byte)0xFF;
						strncpy(title,title,MAX_SONG_TITLE);
						writeSongTitle(editSelection,title);		
						writeSongTempo(editSelection,0xFF);							
						writeSongNoteDivision(editSelection,0xFF);							
						writeSongBeatSignature(editSelection,0xFF);							
				}
				
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();
				}
				break;
			case SETTINGS_PAGE:
				switch (actualMenuOption)
				{
					case MODE_OPTION:
						//leemos el modo
						editData = EEPROM.read(EEPROM_CONFIG_MODE);
						//si no esta seteado, lo seteamos
						editData != 0xFF ? actualMenuOption = editData : actualMenuOption = 0;
						actualMenuPage = MODE_PAGE;
						break;							
					case EQUAL_TICKS_OPTION:
						//leemos el ticks iguales
						editData = EEPROM.read(EEPROM_CONFIG_EQUAL_TICKS);
						//si no esta seteado, lo seteamos
						editData != 0xFF ? actualMenuOption = editData : actualMenuOption = 0;
						actualMenuPage = EQUAL_TICKS_PAGE;
						break;		
					case TICK_SOUND_OPTION:
						//leemos el sonido actual
						actualMenuOption = EEPROM.read(EEPROM_CONFIG_TICK_SOUND);
						//si no esta seteado, lo seteamos
						if (actualMenuOption < SND_1 || actualMenuOption > SND_3)
							actualMenuOption = SND_1;
						//actualizamos el sonido actual
						tickSound = editData;
						actualMenuPage = TICK_SOUND_PAGE;
						break;		
					case MIDI_CLOCK_OPTION:
						//leemos el canal midi
						editData = EEPROM.read(EEPROM_CONFIG_MIDI_CLOCK);
						//si no esta seteado, lo seteamos
						editData != 0xFF ? actualMenuOption = editData : actualMenuOption = 0;
						actualMenuPage = MIDI_CLOCK_PAGE;
						break;	
					case RESET_FABRIC_OPTION:
						actualMenuPage = RESET_FABRIC_PAGE;
						actualMenuOption = 0;
						break;	
				}
				break;
			case MODE_PAGE:
			{
				//cambiamos el modo
				mode = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_MODE,mode);
				
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				break;
			}
			case EQUAL_TICKS_PAGE:
			{
				//cambiamos el flag equal ticks
				equalTicks = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_EQUAL_TICKS,equalTicks);
				
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				break;
			}
			case TICK_SOUND_PAGE:
			{
				//cambiamos el sonido
				tickSound = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_TICK_SOUND,tickSound);
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				break;
			}
			case MIDI_CLOCK_PAGE:
			{
				//cambiamos la opcion
				midiClock = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_MIDI_CLOCK,midiClock);
				
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				break;
			}
			case RESET_FABRIC_PAGE:
			{
				//reiniciamos ajustes a fabrica
				if (actualMenuOption == 1) //si
					resetDefault();
				
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				break;
			}
			break;
		}						
		
		refresh = true;
	}
}

//funcion para el refresco del LCD
void refreshLCD()
{
	
	//si hemos cambiado de estado, refrescamos completamente
	if (lastState != state){
		display.clear();
		lastState = state;
	}
	
	//segun estado
	switch (state)
	{
		//estado principal
		case MAIN_STATE:
			//segun modo
			switch (mode)
			{
				//modo directo
				case LIVE_MODE:
					//actualizamos display del modo directo
					//display.clear();
					display.setCursor(0,0);
					display.setBlackText(false);
					display.print(actualPlayListNum+1);
					display.print(".");
					display.print(actualPlayList.title);
					display.print("\n"); 
					display.clear(0,END_OF_LINE,1,2);
					display.setCursor(0,1);
					display.print(actualNumSong+1);
					display.print(".");
					display.print(actualSong.title);
					display.print("\n\n"); 
					display.set2X();
					display.print(bpm); 
					display.print(F(" BPM\n\n")); 
					display.set1X();
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					display.print("      ");
					display.setCursor(96,7);
					play ? display.print(F("START")) : display.print("STOP ");
					//display.display();
					
					break;
				//modo metronomo
				case METRONOME_MODE:
					//actualizamos display del modo metronomo
					//display.clear(0,(display.fontWidth()+4)*5,2,3);
					display.clear(0,32,1,2);
					display.setCursor(11,1);
					display.setBlackText(false);
					display.set2X();
					display.print(bpm); 
					//display.setCursor(display.fontWidth()*5,2);
					display.setCursor(64,1);
					display.print(F("BPM")); 					
					display.set1X();
					display.setCursor(64,4);
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					display.setCursor(0,7);
					mode ? display.print(F("METRO")) : display.print("LIVE ");
					display.setCursor(96,7);
					play ? display.print(F("START")) : display.print("STOP ");
					//display.display();
					
					break;
			}
			
			break;
		case MENU_STATE:
			//actualizamos display del menu
			//si hemos cambiado de pagina, refrescamos completamente
			if (lastMenuPage != actualMenuPage){
				display.clear();
				lastMenuPage = actualMenuPage;
			}
			display.setCursor(0,0);
			//segun la p�gina del menu
			switch (actualMenuPage)
			{
				//cambio de repertorio
				case PLAYLIST_CHANGE_PAGE:
					{
					display.setBlackText(false);
					display.println(F("Elige repertorio:"));
					display.clear(0,END_OF_LINE,1,1);
					for (int i=0;i<MAX_PLAYLISTS;i++)
					{
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						char * title = readPlayListTitle(i);
						display.print(i+1);
						display.print(".");
						display.println(title);
						free (title);
					}
					}
					break;
				case PLAYLIST_NAME_PAGE:
					{
					display.setBlackText(false);
					display.println(F("Nombre repertorio:"));
					display.clear(0,END_OF_LINE,1,1);
					//cambio de caracter con encoder
					if (deltaEnc > 0)
					{
						if ((byte)editString[editCursor] == 0) // \0'
							editString[editCursor] = 32; //'space'
						if ((byte)editString[editCursor] == 'z') 
							editString[editCursor] = 216; //'simbolo enter'
						if ((byte)editString[editCursor] < 'z') 
							editString[editCursor] = editString[editCursor]+1;						
						
						deltaEnc = 0; //para que no se mueva
					}
					if (deltaEnc < 0)
					{
						if ((byte)editString[editCursor] == 216) //'simbolo enter'
							editString[editCursor] = 'z';
						else if ((byte)editString[editCursor] > 32) //'space'
							editString[editCursor] = editString[editCursor]-1;
						
						deltaEnc = 0; //para que no se mueva						
					}	
					//mostramos la cadena
					display.println(editString);
					//dibujamos cursor en la posicion actual
					//display.drawFastHLine((editCursor*6), 15, 6, WHITE);
					}
					break;
				//cambio de cancion (elegimos primero el orden)
				case CHANGE_ORDER_PAGE:
					{
					display.println(F("Elige la posicion:"));
					display.clear(0,END_OF_LINE,1,1);
					display.setBlackText(false);
					//leemos el titulo de la cancion de la posicion de edicion
					char * title = readSongTitle(getSongNum(actualPlayListNum,actualMenuOption));
					//lo mostramos
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				//cambio de cancion (elegimos la cancion)
				case CHANGE_ORDER_PAGE_2:
					{
					display.println(F("Posicion a editar:"));
					//leemos el titulo de la cancion de la posicion de edicion
					char * title = readSongTitle(getSongNum(actualPlayListNum,editData));
					//lo mostramos
					display.print(editData+1);
					display.print(".");
					display.println(title);
					display.println("\n");
					display.println(F("Elige la cancion:"));
					display.clear(0,END_OF_LINE,5,6);
					free(title);
					
					title = readSongTitle(actualMenuOption);
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				//insertar cancion (elegimos primero el orden)
				case INSERT_SONG_PAGE:
					{
					display.println(F("Insertar despues de:"));
					display.setBlackText(false);
					display.clear(0,END_OF_LINE,1,1);
					//leemos el titulo de la cancion de la posicion de edicion
					char * title = readSongTitle(getSongNum(actualPlayListNum,actualMenuOption));
					//lo mostramos
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				//insertar cancion (elegimos la cancion)
				case INSERT_SONG_PAGE_2:
					{
					display.println(F("Cancion a insertar:"));
					display.clear(0,END_OF_LINE,1,1);
					char * title = readSongTitle(actualMenuOption);
					display.print(editData+2); //se insertar� en la siguiente posicion 
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				//borrar cancion
				case DELETE_SONG_PAGE:
					{
					display.println(F("Borrar cancion"));
					display.clear(0,END_OF_LINE,1,1);
					display.setBlackText(false);
					//leemos el titulo de la cancion de la posicion de edicion
					char * title = readSongTitle(getSongNum(actualPlayListNum,actualMenuOption));
					//lo mostramos
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;		
				//elige cancion para editar
				case SELECT_EDIT_SONG_PAGE:
					{
					display.println(F("Elige la cancion:"));
					char * title = readSongTitle(actualMenuOption);
					display.clear(0,END_OF_LINE,1,1);
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				case CHANGE_SONG_NAME_PAGE:
					{
					display.setBlackText(false);
					display.println(F("Nombre cancion:"));
					display.clear(0,END_OF_LINE,1,1);
					//cambio de caracter con encoder
					if (deltaEnc > 0)
					{
						if ((byte)editString[editCursor] == 0) // \0'
							editString[editCursor] = 32; //'space'
						if ((byte)editString[editCursor] == 'z') 
							editString[editCursor] = 216; //'simbolo enter'
						if ((byte)editString[editCursor] < 'z') 
							editString[editCursor] = editString[editCursor]+1;		
			
						deltaEnc = 0; //para que no se mueva										
					}
					if (deltaEnc < 0)
					{
						if ((byte)editString[editCursor] == 216) //'simbolo enter'
							editString[editCursor] = 'z';
						else if ((byte)editString[editCursor] > 32) //'space'
							editString[editCursor] = editString[editCursor]-1;
												
						deltaEnc = 0; //para que no se mueva						
					}	
					//mostramos la cadena
					display.println(editString);
					//dibujamos cursor en la posicion actual
					//display.drawFastHLine((editCursor*6), 15, 6, WHITE);
					}
					break;
				case CHANGE_SONG_TEMPO_PAGE:
					{
					display.setBlackText(false);
					display.println(F("Elige el tempo:"));
					display.println("\n");
					display.clear(0,END_OF_LINE,3,4);
					display.print(actualMenuOption);
					display.println(F("  BPM"));
					}
					break;
				case CHANGE_SONG_BEAT_PAGE:
					{
					display.setBlackText(false);
					display.println(F("Tipo Compas:"));
					display.println("\n");
					display.clear(0,END_OF_LINE,3,4);
					display.println(actualMenuOption+2);					
					}
					break;
				//elige cancion para vaciar
				case SELECT_EMPTY_SONG_PAGE:
					{
					display.setBlackText(false);
					display.println(F("Elige la cancion:"));
					display.clear(0,END_OF_LINE,1,1);
					char * title = readSongTitle(actualMenuOption);
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;	
				//informacion
				case INFO_PAGE:
					{
					display.setBlackText(false);
					display.println(F("BeatDuino"));
					display.println(F("by Warrior"));
					display.print(F("Version:"));
					display.print(majorVersion);
					display.print(".");
					display.println(minorVersion);
					display.print(F("Ciclo Act:"));
					display.print(lastCycleTime);
					display.println(F("uS"));
					display.print(F("Ciclo Min:"));
					display.print(minCycleTime);
					display.println(F("uS"));
					display.print(F("Ciclo Max:"));
					display.print(maxCycleTime);
					display.println(F("uS"));
					}
					break;				
				//cualquier pagina de menu que solo muestra opciones
				default:
					{
					char buffer[30];
					for (int i=0;i<menuPage[actualMenuPage].numOptions;i++)
					{
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						//leemos la opcion de la pagina
						strcpy_P(buffer, (char*)pgm_read_word(&(menuPage[actualMenuPage].strTable[i])));
						display.println(buffer);	
					}
					}
			}
			//mostramos pantalla
			//display.display();
			
			break;
	}
	
}

//callback de la interrupcion 0 para leer el encoder
void doEncoder()
{
  //si el canal A y el B son iguales, estamos incrementando, si no, decrementando
  if (digitalRead(ENC_B) == digitalRead(ENC_A)) 
	deltaEnc=1;
  else
 	deltaEnc=-1;  
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
	    button[buttonNum].timerOn+=clock10ms;
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
	 button[buttonNum].timerOff+=clock10ms;
   }
   
   //pulsacion larga
   if (button[buttonNum].pressed)
   {
      //contamos pulsacion larga
      button[buttonNum].timerLong+=clock10ms;
      //activamos flag long en primer flanco de cuenta
      if (button[buttonNum].timerLong == buttonLongPress)
      	 button[buttonNum].longPress = true;	
      else
	 button[buttonNum].longPress = false;
	 
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

//funcion para leer la configuracion de EEPROM
void loadConfig()
{
	byte data;
	
	data = EEPROM.read(EEPROM_CONFIG_MODE);
	data != 0xFF ? mode =data : mode = mode;
	
	data = EEPROM.read(EEPROM_CONFIG_EQUAL_TICKS);
	data != 0xFF ? equalTicks =data : equalTicks = equalTicks;
	
	data = EEPROM.read(EEPROM_CONFIG_MIDI_CLOCK);
	data != 0xFF ? midiClock =data : midiClock = midiClock;
	
	data = EEPROM.read(EEPROM_CONFIG_TICK_SOUND);
	data >= SND_1 && data <= SND_3 ? tickSound =data : tickSound = tickSound;
	
}

//funcion para inicializar la EEPROM por defecto
void resetDefault()
{
	//inicializamos las canciones
	initSongs();
	//inicializamos el orden
	initPlayLists();
	//iniciamos configuracion
	EEPROM.update(EEPROM_CONFIG_MODE,(byte)0xFF);
	EEPROM.update(EEPROM_CONFIG_EQUAL_TICKS,(byte)0xFF);
	EEPROM.update(EEPROM_CONFIG_MIDI_CLOCK,(byte)0xFF);
	EEPROM.update(EEPROM_CONFIG_TICK_SOUND,(byte)SND_1);
	
	//reiniciamos
	while(true)
	{}
}

 //funcion que realiza mensaje de inicio y test luces
void wellcomeTest()
{
	unsigned char welcome[] = {'B','e','a','t','D','u','i','n','o'};
	unsigned char welcome2[] = {'v','e','r',' ',majorVersion+48,'.',minorVersion+48};
	unsigned char welcome3[] = {'b','y',' ','W','a','r','r','i','o','r'};
	
	int frames = 0;
	
	
	display.clear();
	display.set1X();
	display.setBlackText(false);
    
    display.setCursor(0,0);
    for (unsigned int i=0;i<sizeof(welcome);i++)
	{
		display.print(char(welcome[i]));
		//display.display();
		delay(25);
		frames++;
	}
		
    delay(1000);
	
	display.print("\n");
	for (unsigned int i=0;i<sizeof(welcome2);i++)
	{
		display.print(char(welcome2[i]));
		//display.display();
		delay(25);
		frames++;
	}
	
	delay(1000);
	
	display.print("\n");
	for (unsigned int i=0;i<sizeof(welcome3);i++)
	{
		display.print(char(welcome3[i]));
		//display.display();
		delay(25);
		frames++;
	}
	
	delay(1000);
	
	/*display.drawBitmap(90, 20,  bmMetronome, 32, 33, 2);
	//display.display();
	
	delay(1000);*/
}
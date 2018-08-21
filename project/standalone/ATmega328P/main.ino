/*
 BeatDuino

 Metronomo digital programable con funciones de guardar repertorios y salida midi

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 TO-DO:
	-Grabar Sonido metronomo Pau
			
 v1.0	-	Release Inicial
 v1.1   - 	Añadimos entrada configurable trigger y opciones primero/ultimo en edicion repertorios
 v1.2   -   Indicamos la accion que hacen los botones en toolbar, no lo que están haciendo
			Recordamos ultimo tempo en modo metronomo y ultima cancion y repertorio en modo live
 v1.3   -   Bug corregido en desincronizacion con el tempo real. Me comia un tiempo midi de negra antes de hacer el click
			Añadido sonido metronomo Boss
 v1.4   -   Corregido bug con array paginas menu que impedia subir el tiempo del temporizador
			Si volvemos a pulsar el temporizador cuando esta en marcha, lo detiene
			En editar canciones del repertorio, las teclas de funcion insertan o borran directamente
			Cambios textos menu repertorios para ser mas descriptivos
			Nueva funcion en modo metronomo para cambiar el compas. No se guarda en EEPROM
			Se define NO_PAGE para paginas de menu que vuelven directamente al modo principal
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
#ifndef ENG
#include <strings.h>
#else
#include <stringsENG.h>
#endif
//funciones datos
#include <dataManagement.h>
//midi
#include <TimerOne.h>
//importacion EEPROM (solo en modo compilacion INITIALIZE)
#ifdef INITIALIZE
#include <import.h>
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
}button[NUM_BUTTONS];

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
} menuPage[36] = {	4,NO_PAGE,mainStr,
					4,MAIN_PAGE,playListStr,
					MAX_PLAYLISTS,PLAYLIST_PAGE,NULL,
					2,PLAYLIST_PAGE,editPlayListStr,
					0,PLAYLIST_EDIT_PAGE,NULL,
					4,PLAYLIST_PAGE,changeOrderStr,
					MAX_SONGS,ORDER_PAGE,NULL,
					MAX_SONGS,ORDER_PAGE,NULL,
					MAX_SONGS,ORDER_PAGE,NULL, 
					MAX_SONGS,ORDER_PAGE,NULL,
					MAX_SONGS,ORDER_PAGE,NULL,
					2,ORDER_PAGE,confirmStr,
					MAX_PLAYLISTS,PLAYLIST_PAGE,NULL,
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
					7,MAIN_PAGE,settingsStr,
					2,SETTINGS_PAGE,modeStr,
					3,SETTINGS_PAGE,equalTicksStr,
					3,SETTINGS_PAGE,soundsStr,
					255,SETTINGS_PAGE,NULL,
					2,SETTINGS_PAGE,confirmStr,
					2,SETTINGS_PAGE,triggerStr,
					3,TRIGGER_PAGE,triggerFuncStr,
					2,TRIGGER_PAGE,triggerTypeStr,
					2,SETTINGS_PAGE,confirmStr,
					0,MAIN_PAGE,NULL,
					3,NO_PAGE,noteDivisionStr,
					6,NO_PAGE,NULL,
				};

const unsigned int buttonDelay    	= 5;				//Tiempo antirebote (*10ms)
const unsigned int buttonLongPress	= 200;				//Tiempo pulsacion larga para otras funciones (*10ms)				
const unsigned int encoderDelay		= 1;				//pulsos retardo modificacion encoder
//==============================================

//Variables generales===========================
byte mode          	= METRONOME_MODE;			//modo general
byte state         	= MAIN_STATE;				//estado general
byte lastState		= 255;						//estado anterior
boolean refresh		= true;						//refresco LCD
unsigned int bpm 					= 120;		//tempo general
unsigned int lastBpm				= 0;		//tempo anterior
unsigned long clickDuration			= 100000;	//duración pulso click en microsegundos
unsigned long clickLastTime			= 0;		//cuenta del inicio pulso click
unsigned int noteDivision			= QUARTER;	//subdivision nota click
unsigned int barSignature   		= 4;		//tipo compas
volatile unsigned int actualTick    = 0;		//tiempo actual
byte tickSound				= SND_1;			//sonido del tick
boolean tick 				= true;				//flag de activar tick
boolean play				= false;			//flag de activar metronomo
byte equalTicks			    = ALL_TICKS;		//tipo de tick: todos, tick fuerte, debil
byte actualNumSong			= 0;				//cancion actual del repertorio
byte actualPlayListNum      = 0;				//numero de repetorio actual
boolean stopTimer           = false;			//flag de temporizador parar metronomo
byte timeStopTimer 			= 5;				//tiempo predeterminado para parar el metronomo (s)
byte countStopTimer			= 0;				//contador temporizador parar metronomo
unsigned long iniStopTimer	= 0;				//tiempo inicial temporizador parar metronomo
//midi
boolean midiClock			= false;			//flag de envio de midi clock
float midiClockTime;							//intervalo midi interrupcion
volatile byte midiCounter	= 0;				//contador clocks midi
//display Oled
SSD1306AsciiAvrI2c display;
//interfaz
volatile signed int deltaEnc		= 0;				//incremento o decremento del encoder
volatile unsigned int deltaCount	= 0;				//contador pulsos para el encoder
unsigned long countClockTime		= 0;				//contador de tiempo para las bases 10ms
boolean clock10ms					= false;			//flanco de 10ms
//menu
byte actualMenuPage         = MAIN_PAGE;		//página del menu actual
byte lastMenuPage			= 255;				//pagina anterior del menu
byte actualMenuOption 		= 0;				//opcion seleccionada del menu
//entrada texto o parametros
byte editCursor             = 0;				//posicion cursor edicion
char * editString;								//cadena a editar
byte editData				= 0;				//dato a editar
unsigned int editDataInt	= 0;				//dato a editar entero
byte editSelection			= 0;				//seleccion a editar
//trigger
byte triggerFunction		= START_STOP_FUNC;	//Funcion del trigger externo
byte triggerType			= PUSH_TRIGGER;		//Tipo de trigger
boolean memTrigger			= false;			//memoria de switch-trigger
//debug
unsigned long startTime     = 0;				//tiempo de inicio de ejecucion ciclo para medir rendimiento
unsigned long lastCycleTime = 0;				//tiempo que tardo el ultimo ciclo
unsigned long minCycleTime  = 2000000;			//tiempo de ciclo minimo
unsigned long maxCycleTime  = 0;				//tiempo de ciclo maximo
int general;

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
	pinMode(FUNCTION_PIN,INPUT_PULLUP);
	pinMode(TRIGGER_PIN,INPUT_PULLUP);
	
	pinMode(BEAT1_CLICK,OUTPUT);
	pinMode(SND0_CLICK,OUTPUT);
	pinMode(SND1_CLICK,OUTPUT);
	
	pinMode(LED_CLICK,OUTPUT);
	
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
	
	//si se enciende con la tecla del enter pulsada
	if (digitalRead(ENTER_PIN) == LOW)
	{
		//enviamos el contenido del EEPROM por serie
		dumpEepromData();		
	}	
	
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
	
	//prueba de elementos
	//debugWriteSongs();
	//debugWritePlayLists();
	
	//leemos la configuracion
	loadConfig();
	
	//si hemos compilado como INITIALIZE, escribimos la EEPROM
	#ifdef INITIALIZE
		importEeprom();
	#endif
	
	//leemos los datos actuales
	readPlayListData();
	readSongData();
	
	//activamos el watchdog a 2 segundos
	wdt_enable(WDTO_2S);
}

void loop()
{ 
	//inciio ejecucion
	startTime = micros();
		
	//procesamos botones
	processButton(START_STOP,START_STOP_BT);
	processButton(MENU_PIN,MENU_BT);
	processButton(ENTER_PIN,ENTER_BT);
	processButton(FUNCTION_PIN,FUNCTION_BT);
	processButton(TRIGGER_PIN,TRIGGER_BT);
	
	//si pulsamos boton menu/cancelar-atras
	if (button[MENU_BT].pEdgePress)
	{
		doMenuButton();
	}		
	
	//si pulsamos boton funcion
	if (button[FUNCTION_BT].pEdgePress)
	{
		doFunctionButton();
	}
	
	//si pulsamos boton start/stop
	if (button[START_STOP_BT].pEdgePress)
	{
		doStartStopButton();
	}
	
	//si pulsamos el trigger externo (flanco positivo)
	if (button[TRIGGER_BT].pEdgePress)
	{		
		doTriggerButton();					
	}
	
	//si soltamos el trigger externo (flanco negativo)
	if (button[TRIGGER_BT].nEdgePress)
	{		
		//solo para el tipo interruptor
		if (triggerType == SWITCH_TRIGGER)
			doTriggerButton();					
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
	//si se cumple el ancho de pulso del tick
	if (micros() - clickLastTime > clickDuration)
	{
		//quitamos señales tick
		PORTB &= 0xF8;
		//apagamos led (PB3)
		PORTB &= ~(1<<3);
	}
	
	//comprobamos iteraccion del midiClock
	if (midiCounter >= (MIDI_TICKS_BEAT/noteDivision)-1)
	{
		midiCounter = 0;
		
		//incrementamos el numero de tick del compas
		actualTick >= (barSignature-1) ? actualTick = 0 : actualTick++;
		//si está en reproduccion
		if (play)
		{
			//guardamos tiempo inicio tick
			clickLastTime = micros();
			//sonido del tick según si es el primer tiempo del compás y está configurado para sonar ese tiempo
			PORTB |= ((tickSound*2)+2) + ((actualTick == 0 || equalTicks == STRONG_TICK) && equalTicks != WEAK_TICK);	
			//encendemos led (11 es PB3)
			PORTB |= 1 << 3;
		}
	}
	else
		midiCounter++;
	
	//Enviamos un midi clock cada 24 veces por negra)
	if (midiClock)
		Serial.write(MIDI_CLOCK_MSG);	
}

//acciones del botón de menu/atras
void doMenuButton()
{
	//comprobamos estado
	switch (state)
	{
		//si está en el menu
		case MENU_STATE:
			//si no hay pagina previa, sale del menu
			if (menuPage[actualMenuPage].prevPage == NO_PAGE)
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
			editCursor       = 0;
			actualMenuPage = MAIN_PAGE;
			
			break;
	}	
	refresh = true;
}

//acciones del botón de function
void doFunctionButton()
{
	//comprobamos estado
	switch (state)
	{
		//si está en modo principal
		case MAIN_STATE:
			switch(mode)
			{
			//si esta en modo metronomo: edita el compas
			case METRONOME_MODE:
				state = MENU_STATE;
				actualMenuPage = CHANGE_METRONOME_BEAT_PAGE;
				editData = barSignature;
				actualMenuOption = editData-2;
				
				break;			
			//si esta en modo repertorio, reproduce/para
			case LIVE_MODE:
				if (!stopTimer){
					stopTimer = true;
					iniStopTimer = millis();
					play = true;
				}else{
					stopTimer = false;
					iniStopTimer = 0;
					countStopTimer = 0;
					play = false;
				}
				break;
			}
			break;
		//si está en el menu
		case MENU_STATE:
			//dependiendo de la pagina, hace una funcion
			switch(actualMenuPage)
			{
			case INFO_PAGE:			//reset tiempos
				minCycleTime  = 2000000;
				maxCycleTime  = 0;

				break;
			case PLAYLIST_NAME_PAGE: //funcion SAVE
				//guardamos titulo
				writePlayListTitle(actualPlayListNum,editString);
				free(editString);
				//salimos del menu
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				actualMenuOption = 0;
				readPlayListData();
				
				break;
			case SELECT_EDIT_SONG_PAGE: //ir al ultimo
				actualMenuOption = MAX_SONGS-1;
				
				break;
			case CHANGE_ORDER_PAGE:	//eliminar cancion
				//borramos la cancion. Movemos el resto de canciones una posicion hasta el final de la lista
				for (int i=actualMenuOption;i<MAX_SONGS-1;i++)
				{
					writePlayListSong(actualPlayListNum,i,getSongNum(actualPlayListNum,i+1));
				}
				//la ultima cancion se queda vacía
				writePlayListSong(actualPlayListNum,MAX_SONGS-1,0);
				actualMenuOption=actualMenuOption;
				editData=0;
				actualMenuPage =  CHANGE_ORDER_PAGE;
				readSongData();
				
				break;
			case INSERT_SONG_PAGE:	//ir al ultimo
				actualMenuOption = MAX_SONGS-1;
				
				break;
			case DELETE_SONG_PAGE:	//ir al ultimo
				actualMenuOption = MAX_SONGS-1;
				
				break;
			case CHANGE_SONG_NAME_PAGE: //SAVE
				//guardamos el nuevo nombre
				writeSongTitle(editSelection,editString);
				free(editString);
				//salimos del menu
				actualMenuOption=0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				readSongData();

				break;
			}				
			break;
		//si no esta en el menu, no hace nada (de momento)
		default:			
			break;
	}	
	refresh = true;
}

//funcion del boton Start/Stop
void doStartStopButton()
{
	//comprobamos estado
	switch (state)
	{
		//modo principal: para y arranca el metronomo
		case MAIN_STATE:
			play = !play;
			break;
		//modo menu: segun la pantalla
		case MENU_STATE:
			switch(actualMenuPage)
			{
				case PLAYLIST_NAME_PAGE: //funcion borra caracter
					editString[editCursor] = 32;				
					break;
				case SELECT_EDIT_SONG_PAGE: //ir al primero
					actualMenuOption = 0;
					break;
				case CHANGE_ORDER_PAGE:	//insertar cancion
					//comprobamos que no quieras insertar en la ultima cancion (no caben mas)
					if (actualMenuOption < (MAX_SONGS-1))
					{
						//elegimos posicion de la inserccion
						editData = actualMenuOption;
						actualMenuOption = 0;
						actualMenuPage = INSERT_SONG_PAGE_2;
						//aprovechamos esta variable para que vuelva a esta pagina
						editDataInt = CHANGE_ORDER_PAGE;
					}
					break;
				case INSERT_SONG_PAGE:	//ir al primero
					actualMenuOption = 0;
					break;
				case DELETE_SONG_PAGE:	//ir al primero
					actualMenuOption = 0;
					break;
				case CHANGE_SONG_NAME_PAGE: //funcion borra caracter
					editString[editCursor] = 32;				
					break;
			}		
			break;
	}
	
	refresh = true;
}

//funcion del trigger externo
void doTriggerButton()
{
	switch(triggerFunction)
	{
		case START_STOP_FUNC: //funcion de play/stop
			play = !play;
			break;
		case NEXT_FUNC:	//funcion de siguiente o incrementar bpm
			switch(mode)
			{
				case LIVE_MODE:
					if (actualNumSong < (MAX_SONGS-1))
					{
						//cambio de cancion
						actualNumSong++;
						readSongData();	
						//obtenemos datos tema
						bpm 			= actualSong.tempo;
						noteDivision 	= actualSong.noteDivision;
						barSignature 	= actualSong.barSignature;			
					}
					
					break;
				case METRONOME_MODE:
					//cambio tempo
					if (bpm < 255)
						bpm++;
					
					break;
			}
			break;
		case PREV_FUNC:	//funcion de anterior o decrementar bpm
			switch(mode)
			{
				case LIVE_MODE:
					if (actualNumSong > 0)
					{
						//cambio de cancion
						actualNumSong--;
						readSongData();	
						//obtenemos datos tema
						bpm 			= actualSong.tempo;
						noteDivision 	= actualSong.noteDivision;
						barSignature 	= actualSong.barSignature;			
					}
					break;
				case METRONOME_MODE:
					//cambio tempo
					if (bpm > 1)
						bpm--;
					
					break;
			}
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
				//guardamos la cancion usada
				EEPROM_Write(EEPROM_LAST_PLAYLIST_USED,actualPlayListNum);
				EEPROM_Write(EEPROM_LAST_SONG_USED,actualNumSong);
				refresh = true;
			}
			if (deltaEnc < 0 && actualNumSong > 0)
			{
				actualNumSong--;
				readSongData();
				deltaEnc = 0; //para que no se mueva
				//guardamos la cancion usada
				EEPROM_Write(EEPROM_LAST_PLAYLIST_USED,actualPlayListNum);
				EEPROM_Write(EEPROM_LAST_SONG_USED,actualNumSong);
				refresh = true;
			}
								
			//obtenemos datos tema
			bpm 			= actualSong.tempo;
			noteDivision 	= actualSong.noteDivision;
			barSignature 	= actualSong.barSignature;
			
			//temporizador parada metronomo
			if (stopTimer){
				//si se ha alcanzado el tiempo
				if (countStopTimer >= timeStopTimer){	
					//parametros el metronomo
					play = false;
					//quitamos el temporizador
					stopTimer = false;
					countStopTimer = 0;
					iniStopTimer   = 0;
					refresh= true;
				}else{
					//si pasa un segundo, incrementamos
					if ((millis() - iniStopTimer)>=1000){
						countStopTimer++;
						iniStopTimer = millis();
					}	
				}
			}else{
				countStopTimer = 0;
				iniStopTimer   = 0;
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
				//guardamos el tempo usado
				EEPROM_Write(EEPROM_LAST_TEMPO_USED,bpm);				
				refresh = true;
			}
			if (deltaEnc < 0 )
			{
				if (bpm > 1)
					bpm--;
				
				deltaEnc = 0;
				//guardamos el tempo usado
				EEPROM_Write(EEPROM_LAST_TEMPO_USED,bpm);
				refresh = true;
			}
			
			//desactivamos temporizador 
			stopTimer = false;
			
			break;
	}
}

//acciones para el estado Menu
void doMenuState()
{
	//cambio de opcion
	if (deltaEnc > 0 )
	{
		//actualMenuOption < menuPage[actualMenuPage].numOptions-1 ? actualMenuOption++ : actualMenuOption=0;
		if (actualMenuOption < menuPage[actualMenuPage].numOptions-1) actualMenuOption++;
		//si tiene el numero de opciones definido, reseteamos el encoder
		if (menuPage[actualMenuPage].numOptions != 0)
			deltaEnc = 0; //para que no se mueva						
		refresh = true;
	}
	if (deltaEnc < 0 )
	{
		//actualMenuOption > 0 ? actualMenuOption-- : actualMenuOption = menuPage[actualMenuPage].numOptions-1;
		if (actualMenuOption > 0)  actualMenuOption-- ;
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
					case COPY_PLAYLIST_OPTION:
						actualMenuOption = 0;
						actualMenuPage = PLAYLIST_COPY_PAGE;
						
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
				//avanzamos el cursor de edicion de nombre
				editCursor < MAX_PLAYLIST_TITLE-2 ? editCursor++ : editCursor = 0;							
				
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
				//aceptamos el cambio de canción
				writePlayListSong(actualPlayListNum,editData,actualMenuOption);
				actualMenuOption=editData;
				editData=0;
				actualMenuPage = CHANGE_ORDER_PAGE;
				
				readSongData();
				break;
			case INSERT_SONG_PAGE:  
				//comprobamos que no quieras insertar en la ultima cancion (no caben mas)
				if (actualMenuOption < (MAX_SONGS-1))
				{
					//elegimos posicion de la inserccion
					editData = actualMenuOption;
					actualMenuOption = 0;
					actualMenuPage = INSERT_SONG_PAGE_2;
					//aprovechamos esta variable para que vuelva a esta pagina
					editDataInt = INSERT_SONG_PAGE;
				}
				
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
				
				actualMenuOption=editData+1;	//marcamos la nueva cancion insertada
				editData=0;
				actualMenuPage = editDataInt;
				readSongData();
									
				break;
			case DELETE_SONG_PAGE: 	
				//borramos la cancion. Movemos el resto de canciones una posicion hasta el final de la lista
				for (int i=actualMenuOption;i<MAX_SONGS-1;i++)
				{
					writePlayListSong(actualPlayListNum,i,getSongNum(actualPlayListNum,i+1));
				}
				//la ultima cancion se queda vacía
				writePlayListSong(actualPlayListNum,MAX_SONGS-1,0);
				actualMenuOption=actualMenuOption;
				editData=0;
				actualMenuPage =  DELETE_SONG_PAGE;
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
			case PLAYLIST_COPY_PAGE: 	
				//copiamos el orden actual al seleccionado
				for (int i=0;i<MAX_SONGS;i++)
				{
					writePlayListSong(actualMenuOption,i,getSongNum(actualPlayListNum,i));
				}
				//cambiamos el nombre
				writePlayListTitle(actualMenuOption,readPlayListTitle(actualPlayListNum));
				
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
				//avanzamos el cursor de edicion de nombre 
				//avanzamos cursor
				editCursor < (MAX_SONG_TITLE-2) ? editCursor++ : editCursor = 0;							
				
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
						
						actualMenuPage = TICK_SOUND_PAGE;
						break;	
					case STOP_TIMER_OPTION:
						//leemos el stop timer
						editData = EEPROM.read(EEPROM_CONFIG_STOP_TIMER);
						//si no esta seteado, lo seteamos
						editData >= 0 || editData <= 255  ? actualMenuOption = editData : actualMenuOption = timeStopTimer;
						actualMenuPage = STOP_TIMER_PAGE;
						break;	
					case MIDI_CLOCK_OPTION:
						//leemos el canal midi
						editData = EEPROM.read(EEPROM_CONFIG_MIDI_CLOCK);
						//si no esta seteado, lo seteamos
						editData != 0xFF ? actualMenuOption = editData : actualMenuOption = 0;
						actualMenuPage = MIDI_CLOCK_PAGE;
						break;	
					case TRIGGER_OPTION:
						actualMenuPage = TRIGGER_PAGE;
						actualMenuOption = 0;
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
				noteDivision =  QUARTER;
				barSignature = 4;	
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
			case STOP_TIMER_PAGE:
			{
				//cambiamos el tiempo
				timeStopTimer = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_STOP_TIMER,timeStopTimer);
				
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
			case TRIGGER_PAGE:
			{
				switch (actualMenuOption)
				{
					case TRIGGER_FUNC_OPTION:
						//leemos la funcion del trigger
						editData = EEPROM.read(EEPROM_CONFIG_TRIGGER_FUNC);
						//si no esta seteado, lo seteamos
						editData != 0xFF ? actualMenuOption = editData : actualMenuOption = 0;
						actualMenuPage = TRIGGER_FUNC_PAGE;						
						break;
					case TRIGGER_TYPE_OPTION:
						//leemos el tipo del trigger
						editData = EEPROM.read(EEPROM_CONFIG_TRIGGER_TYPE);
						//si no esta seteado, lo seteamos
						editData != 0xFF ? actualMenuOption = editData : actualMenuOption = 0;
						actualMenuPage = TRIGGER_TYPE_PAGE;						
						break;
				}
				break;
			}
			case TRIGGER_FUNC_PAGE:
			{
				//cambiamos la opcion
				triggerFunction = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_TRIGGER_FUNC,triggerFunction);
				
				actualMenuOption = 0;
				actualMenuPage = menuPage[actualMenuPage].prevPage;
				break;
			}
			case TRIGGER_TYPE_PAGE:
			{
				//cambiamos la opcion
				triggerType = actualMenuOption;
				//guardamos en config
				EEPROM_Write(EEPROM_CONFIG_TRIGGER_TYPE,triggerType);
				
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
			case CHANGE_METRONOME_NOTE_PAGE:
			{
				switch (actualMenuOption)
					{
						case 0:
							noteDivision = QUARTER;
							break;
						case 1:
							noteDivision = EIGHTH;
							break;
						case 2:
							noteDivision = SIXTEENTH;
							break;
					}
					editData = 0;
					actualMenuOption=0;
					actualMenuPage = 0;
					state = MAIN_STATE;
					
					break;	
			}
			case CHANGE_METRONOME_BEAT_PAGE:
			{
				barSignature = actualMenuOption+2;
				//saltamos a editar la division de nota				
				actualMenuPage = CHANGE_METRONOME_NOTE_PAGE;
				editData = noteDivision;
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
				};
				
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
					display.clear(0,39,3,4);
					display.print(bpm); 
					display.setCursor(40,3);
					display.print(F(" BPM")); 
					display.set1X();	
					//display.clear(57,80,6,6);
					display.clear(0,23,5,5);
					//display.setCursor(57,6);
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					if (play)
						stopTimer ? drawToolbar(stopOpt,timerOffOpt,menuOpt) : drawToolbar(stopOpt,timerOnOpt,menuOpt);
					else
						stopTimer ? drawToolbar(playOpt,timerOffOpt,menuOpt) : drawToolbar(playOpt,timerOnOpt,menuOpt);
					break;
				//modo metronomo
				case METRONOME_MODE:
					//actualizamos display del modo metronomo
					//display.clear(0,(display.fontWidth()+4)*5,2,3);
					display.set2X();
					display.clear(0,63,1,2);
					display.setCursor(11,1);
					display.setBlackText(false);					
					display.print(bpm); 
					//display.setCursor(display.fontWidth()*5,2);
					display.setCursor(64,1);
					display.print(F("BPM")); 					
					display.set1X();
					display.setCursor(64,4);
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					//dibujamos opciones barra
					play ? drawToolbar(playOpt,beatOpt,menuOpt) : drawToolbar(stopOpt,beatOpt,menuOpt); 
									
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
			//segun la página del menu
			switch (actualMenuPage)
			{
				//cambio de repertorio
				case PLAYLIST_CHANGE_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strPlaylistChange);
					display.println(buf);
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
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
					break;
				case PLAYLIST_NAME_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strPlaylistName);
					display.println(buf);
					display.clear(0,END_OF_LINE,1,1);
					//cambio de caracter con encoder
					if (deltaEnc > 0)
					{
						if ((byte)editString[editCursor] < 32) 
							editString[editCursor] = 32; //'space'
						else if ((byte)editString[editCursor] == 32) //'space'
							editString[editCursor] = 'A'; 
						else if ((byte)editString[editCursor] < 'z') 
							editString[editCursor] = editString[editCursor]+1;						
						
						deltaEnc = 0; //para que no se mueva
					}
					if (deltaEnc < 0)
					{
						if ((byte)editString[editCursor] > 'A')
							editString[editCursor] = editString[editCursor]-1;
						
						deltaEnc = 0; //para que no se mueva						
					}	
					//mostramos la cadena
					display.println(editString);
					//dibujamos cursor en la posicion actual
					display.clear(0,END_OF_LINE,2,2);
					display.setCursor((editCursor*6), 2);
					display.print("-");
					//dibujamos opciones toolbar
					drawToolbar(delOpt,saveOpt,backOpt);
					}
					break;
				//cambio de cancion (elegimos primero el orden)
				case CHANGE_ORDER_PAGE:
					{
					int index = actualMenuOption < (LAST_MENU_LINE-1) ? 0 : actualMenuOption-(LAST_MENU_LINE-1);
					
					//mostramos listado del orden
					for (int i = index;i<index+LAST_MENU_LINE;i++)
					{
						char * title;
					
						//leemos el titulo de la cancion actual
						title = readSongTitle(getSongNum(actualPlayListNum,i));
						//lo mostramos
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						if (i < 9) display.print("0");
						display.print(i+1);
						display.print(".");
						display.println(title);
						free (title);
					}					
					//dibujamos opciones barra
					drawToolbar(insertOpt,delOpt,backOpt);
					}
					break;
				//cambio de cancion (elegimos la cancion)
				case CHANGE_ORDER_PAGE_2:
					{
					char buf[30];
					strcpy_P(buf,strChangeOrder);
					display.println(buf);
					//leemos el titulo de la cancion de la posicion de edicion
					char * title = readSongTitle(getSongNum(actualPlayListNum,editData));
					//lo mostramos
					display.print(editData+1);
					display.print(".");
					display.println(title);
					display.println("\n");
					strcpy_P(buf,strChangeOrder2);
					display.println(buf);
					display.clear(0,END_OF_LINE,5,6);
					free(title);
					
					title = readSongTitle(actualMenuOption);
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
					break;
				//insertar cancion (elegimos primero el orden)
				case INSERT_SONG_PAGE:
					{
					int index = actualMenuOption < (LAST_MENU_LINE-1) ? 0 : actualMenuOption-(LAST_MENU_LINE-1);
					
					//mostramos listado del orden
					for (int i = index;i<index+LAST_MENU_LINE;i++)
					{
						char * title;
					
						//leemos el titulo de la cancion actual
						title = readSongTitle(getSongNum(actualPlayListNum,i));
						//lo mostramos
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						if (i < 9) display.print("0");
						display.print(i+1);
						display.print(".");
						display.println(title);
						free (title);
					}					
					//dibujamos opciones barra
					drawToolbar(firstOpt,lastOpt,backOpt);
					}
					break;
				//insertar cancion (elegimos la cancion)
				case INSERT_SONG_PAGE_2:
					{
					char buf[30];
					strcpy_P(buf,strInsertSong);
					display.println(buf);
					//leemos el titulo de la cancion de la posicion de edicion
					char * title = readSongTitle(getSongNum(actualPlayListNum,editData));
					//lo mostramos
					display.print(editData+1);
					display.print(".");
					display.println(title);
					display.println("\n");
					strcpy_P(buf,strChangeOrder2);
					display.println(buf);
					display.clear(0,END_OF_LINE,5,6);
					free(title);
					
					title = readSongTitle(actualMenuOption);
					display.print(actualMenuOption+1); //se insertará en la siguiente posicion 
					display.print(".");
					display.println(title);
					free (title);
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
					break;
				//borrar cancion
				case DELETE_SONG_PAGE:
					{
					int index = actualMenuOption < (LAST_MENU_LINE-1) ? 0 : actualMenuOption-(LAST_MENU_LINE-1);
					
					//mostramos listado del orden
					for (int i = index;i<index+LAST_MENU_LINE;i++)
					{
						char * title;
					
						//leemos el titulo de la cancion actual
						title = readSongTitle(getSongNum(actualPlayListNum,i));
						//lo mostramos
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						if (i < 9) display.print("0");
						display.print(i+1);
						display.print(".");
						display.println(title);
						free (title);
					}					
					//dibujamos opciones barra
					drawToolbar(firstOpt,lastOpt,backOpt);
					}
					break;	
				//copiar de repertorio
				case PLAYLIST_COPY_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strPlaylistCopy);
					display.println(buf);
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
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
					break;	
				//elige cancion para editar
				case SELECT_EDIT_SONG_PAGE:
					{
					int index = actualMenuOption < (LAST_MENU_LINE-1) ? 0 : actualMenuOption-(LAST_MENU_LINE-1);
					
					//mostramos listado del orden
					for (int i = index;i<index+LAST_MENU_LINE;i++)
					{
						char * title;
					
						//leemos el titulo de la cancion actual
						title = readSongTitle(i);
						//lo mostramos
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						if (i < 9) display.print("0");
						display.print(i+1);
						display.print(".");
						display.println(title);
						free (title);
					}		
					//dibujamos opciones barra
					drawToolbar(firstOpt,lastOpt,backOpt);
					}
					break;
				case CHANGE_SONG_NAME_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strSongName);
					display.println(buf);
					display.clear(0,END_OF_LINE,1,1);
					//cambio de caracter con encoder
					if (deltaEnc > 0)
					{
						if ((byte)editString[editCursor] < 32) 
							editString[editCursor] = 32; //'space'
						else if ((byte)editString[editCursor] == 32) //'space'
							editString[editCursor] = 'A'; 
						else if ((byte)editString[editCursor] < 'z') 
							editString[editCursor] = editString[editCursor]+1;						
						
						deltaEnc = 0; //para que no se mueva
					}
					if (deltaEnc < 0)
					{
						if ((byte)editString[editCursor] > 'A')
							editString[editCursor] = editString[editCursor]-1;
						
						deltaEnc = 0; //para que no se mueva						
					}	
					//mostramos la cadena
					display.println(editString);
					//dibujamos cursor en la posicion actual
					display.clear(0,END_OF_LINE,2,2);
					display.setCursor((editCursor*6), 2);
					display.print("-");
					//dibujamos opciones toolbar
					drawToolbar(delOpt,saveOpt,backOpt);
					}
					break;
				case CHANGE_SONG_TEMPO_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strChooseTempo);
					display.println(buf);
					display.println("\n");
					display.clear(0,END_OF_LINE,3,4);
					display.print(actualMenuOption);
					display.println(F("  BPM"));
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
					break;
				case CHANGE_SONG_BEAT_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strBarType);
					display.println(buf);
					display.println("\n");
					display.clear(0,END_OF_LINE,3,4);
					display.println(actualMenuOption+2);
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);					
					}
					break;
				//elige cancion para vaciar
				case SELECT_EMPTY_SONG_PAGE:
					{
					int index = actualMenuOption < (LAST_MENU_LINE-1) ? 0 : actualMenuOption-(LAST_MENU_LINE-1);
					
					//mostramos listado del orden
					for (int i = index;i<index+LAST_MENU_LINE;i++)
					{
						char * title;
					
						//leemos el titulo de la cancion actual
						title = readSongTitle(i);
						//lo mostramos
						actualMenuOption == i ? display.setBlackText(true) : display.setBlackText(false);
						if (i < 9) display.print("0");
						display.print(i+1);
						display.print(".");
						display.println(title);
						free (title);
					}		
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
					break;	
				//stop Timer
				case STOP_TIMER_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strTimeToStop);
					display.println(buf);
					display.println("\n");
					display.clear(0,END_OF_LINE,3,4);
					display.print(actualMenuOption);
					display.println(F("  seg"));
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
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
					display.clear(0,END_OF_LINE,3,3);
					display.print(F("Ciclo Act:"));					
					display.print(lastCycleTime);
					display.println(F("uS"));
					display.clear(0,END_OF_LINE,4,4);
					display.print(F("Ciclo Min:"));					
					display.print(minCycleTime);
					display.println(F("uS"));
					display.clear(0,END_OF_LINE,5,5);
					display.print(F("Ciclo Max:"));					
					display.print(maxCycleTime);
					display.println(F("uS"));
					//dibujamos opciones barra
					drawToolbar(NULL,resetOpt,backOpt);
					}
					break;	
				case CHANGE_METRONOME_BEAT_PAGE:
					{
					display.setBlackText(false);
					char buf[30];
					strcpy_P(buf,strBarType);
					display.println(buf);
					display.println("\n");
					display.clear(0,END_OF_LINE,3,4);
					display.println(actualMenuOption+2);
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);					
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
					//dibujamos opciones barra
					drawToolbar(NULL,NULL,backOpt);
					}
			}
			//mostramos pantalla
			//display.display();
			
			break;
	}
	
}

//funcion que dibuja las opciones de toolbar centradas
void drawToolbar(const char* txt1,const char* txt2,const char* txt3)
{
	char buffer[20];
	//configuramos texto
	display.setBlackText(true);
	display.set1X();
	//comprobamos cada opcion para escribirla
	if (txt1 != NULL){
		strcpy_P(buffer,txt1);
		display.setCursor(0,LAST_LINE);
		display.print(buffer);
	}
	if (txt2 != NULL){
		strcpy_P(buffer,txt2);
		display.setCursor((128>>1)-((strlen(buffer)>>1)*7),LAST_LINE);
		display.print(buffer);
	}
	if (txt3 != NULL){
		strcpy_P(buffer,txt3);
		display.setCursor(END_OF_LINE-(strlen(buffer)*7),LAST_LINE);
		display.print(buffer);
	}
	
	display.setBlackText(false);
}

//callback de la interrupcion 0 para leer el encoder
void doEncoder()
{
	//pulsos de retraso encoder
	if (deltaCount >= encoderDelay)
	{
		//si el canal A y el B son iguales, estamos incrementando, si no, decrementando
		deltaEnc = (digitalRead(ENC_B) == digitalRead(ENC_A)) ? 1: -1; 
		deltaCount = 0;
	}
	else
		deltaCount++;

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

//funcion para leer la configuracion de EEPROM. Comprueba los valores y si no son buenos o no están 
//seteados, carga los valores por defecto
void loadConfig()
{
	byte data;
	
	data = EEPROM.read(EEPROM_CONFIG_MODE);
	data >= METRONOME_MODE && data <= LIVE_MODE ? mode =data : mode = mode;
	
	data = EEPROM.read(EEPROM_CONFIG_EQUAL_TICKS);
	data >= 0 && data <= 2 ? equalTicks =data : equalTicks = equalTicks;
	
	data = EEPROM.read(EEPROM_CONFIG_MIDI_CLOCK);
	data >= 0 && data <= 1 ? midiClock =data : midiClock = midiClock;
	
	data = EEPROM.read(EEPROM_CONFIG_TICK_SOUND);
	data >= SND_1 && data <= SND_3 ? tickSound =data : tickSound = tickSound;
	
	data = EEPROM.read(EEPROM_CONFIG_STOP_TIMER);
	data >= 0 && data <= 255 ? timeStopTimer =data : timeStopTimer = timeStopTimer;
	
	data = EEPROM.read(EEPROM_CONFIG_TRIGGER_FUNC);
	data >= 0 && data <= 2 ? triggerFunction =data : triggerFunction = triggerFunction;
	
	data = EEPROM.read(EEPROM_CONFIG_TRIGGER_TYPE);
	data >= 0 && data <= 1 ? triggerType =data : triggerType = triggerType;
	
	//si el ultimo modo es metronomo, leemos el ultimo bpm, si no , cancion
	if (mode == METRONOME_MODE)
	{
		data = EEPROM.read(EEPROM_LAST_TEMPO_USED);
		data >= 0 && data <= 255 ? bpm =data : bpm = bpm;
	}
	else
	{
		data = EEPROM.read(EEPROM_LAST_PLAYLIST_USED);
		data >= 0 && data <= MAX_PLAYLISTS-1 ? actualPlayListNum =data : actualPlayListNum = 0;
		
		data = EEPROM.read(EEPROM_LAST_SONG_USED);
		data >= 0 && data <= MAX_SONGS-1 ? actualNumSong =data : actualNumSong = 0;
	}
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
	EEPROM.update(EEPROM_CONFIG_STOP_TIMER,timeStopTimer);
	EEPROM.update(EEPROM_CONFIG_TRIGGER_FUNC,triggerFunction);
	EEPROM.update(EEPROM_CONFIG_TRIGGER_TYPE,triggerType);
	//reiniciamos
	display.clear();
	char buf[30];
	strcpy_P(buf,strResetting);
	display.println(buf);
	while(true)
	{}
}

//funcion para cargar el contenido del import.h en EEPROM
void importEeprom()
{
	#ifdef INITIALIZE
	display.clear();
	char buf[30];
	strcpy_P(buf,strImportEEPROM);
	display.println(buf);
	//recorremos el array de importacion
	for (unsigned int i = 0; i < EEPROM_CONFIG_MODE; i++)
		{
			EEPROM_Write(EEPROM_SONGS_POS+i,pgm_read_byte(&eepromData[i]));
		}
	#endif
	delay(1000);
}

//funcion para dumpear por puerto serie el contenido del eeprom
void dumpEepromData()
{
	display.clear();
	char buf[30];
	strcpy_P(buf,strExportEEPROM);
	display.println(buf);
	for (unsigned int i = EEPROM_SONGS_POS; i<EEPROM_CONFIG_MODE;i++) 
		Serial.write(EEPROM.read(i));		
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
    
    //display.setCursor(0,0);
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
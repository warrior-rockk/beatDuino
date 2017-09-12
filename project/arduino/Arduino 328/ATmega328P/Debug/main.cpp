#line 1 "../main.ino"
#include <Arduino.h>
#line 1
/*
 BeatDuino

 Metronomo digital programable con funciones de guardar repertorios y salida midi

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 TO-DO:
	-solo debe leer la informacion de cancion al cambiar opcion si estas en modo live
	-un menu de repertorio que salgan mas de un registro y te desplaces en lista?
	-se esta refrescando lo minimo? ver ciclo de trabajo?
		
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
//textos
#include <strings.h>
//funciones datos
#include <dataManagement.h>
//midi
#include <TimerOne.h>
unsigned long micros ();
void delay (unsigned long ms );
int main (void );
void sendMidiClock ();
void doEncoder ();
void processButton (int pin ,int buttonNum );
void refreshLCD ();
void readPlayListData ();
void readSongData ();
void loadConfig ();
void resetDefault ();
void wellcomeTest ();
#line 37
//Definiciones====================================
//pines IO
#define START_STOP  5
#define ENC_B   	6		
#define ENC_A		2		
#define MENU_PIN   	3		
#define OLED_RESET 	4
#define ENTER_PIN	7
#define MIDI_TX		1
#define MIDI_RX		0 
#define OUT_CLICK 	11		
#define LED_CLICK   13

//botones
#define START_STOP_BT 	0
#define MENU_BT 		1
#define ENTER_BT		2

//páginas del menu
#define MAIN_PAGE 				0
	#define PLAYLIST_PAGE			1
		#define PLAYLIST_CHANGE_PAGE	2
		#define PLAYLIST_EDIT_PAGE		3
			#define PLAYLIST_NAME_PAGE	4
			#define ORDER_PAGE			5
				#define CHANGE_ORDER_PAGE	6
				#define CHANGE_ORDER_PAGE_2	7
				#define INSERT_SONG_PAGE	8
				#define INSERT_SONG_PAGE_2	9
				#define DELETE_SONG_PAGE	10
				#define EMPTY_ORDER_PAGE	11
		#define PLAYLIST_DELETE_PAGE    12
	#define SONG_PAGE				13	
		#define SELECT_EDIT_SONG_PAGE			14
			#define EDIT_SONG_PAGE                  15
				#define CHANGE_SONG_NAME_PAGE			16
				#define CHANGE_SONG_TEMPO_PAGE         	17
				#define CHANGE_SONG_NOTE_PAGE          	18
				#define CHANGE_SONG_BEAT_PAGE          	19
		#define SELECT_EMPTY_SONG_PAGE			20		
			#define EMPTY_SONG_PAGE				21
	#define SETTINGS_PAGE			22
		#define MODE_PAGE					23
		#define EQUAL_TICKS_PAGE			24
		#define TICK_SOUND_PAGE				25
		#define MIDI_CLOCK_PAGE				26
		#define RESET_FABRIC_PAGE			27
	#define INFO_PAGE				28
	
//opciones menu
#define PLAYLIST_OPTION				0
	#define CHANGE_PLAYLIST_OPTION		0
	#define EDIT_PLAYLIST_OPTION		1
		#define NAME_PLAYLIST_OPTION		0
		#define ORDER_OPTION                1
			#define CHANGE_ORDER_OPTION			0
			#define INSERT_SONG_OPTION          1
			#define DELETE_SONG_OPTION			2
			#define EMPTY_ORDER_OPTION			3
	#define DELETE_PLAYLIST_OPTION		2
#define SONG_OPTION					1
	#define EDIT_SONG_OPTION				0
		#define CHANGE_SONG_NAME_OPTION          0
		#define CHANGE_SONG_TEMPO_OPTION         1
		#define CHANGE_SONG_NOTE_OPTION          2
		#define CHANGE_SONG_BEAT_OPTION          3
	#define EMPTY_SONG_OPTION				1	
#define SETTINGS_OPTION				2	
	#define MODE_OPTION					0
	#define EQUAL_TICKS_OPTION			1
	#define TICK_SOUND_OPTION			2
	#define MIDI_CLOCK_OPTION			3
	#define RESET_FABRIC_OPTION			4
#define INFO_OPTION					3

//comandos midi
#define MIDI_CLOCK_MSG		0xF8
#define MIDI_START_MSG		0xFA
#define MIDI_STOP_MSG		0xFC
//midi clock per beat
#define MIDI_TICKS_BEAT		24

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
					0,SETTINGS_PAGE,NULL,
					2,SETTINGS_PAGE,confirmStr,					
					2,SETTINGS_PAGE,confirmStr,					
					0,MAIN_PAGE,NULL,
				};
				  
//==============================================

//Variables generales===========================
byte mode          	= METRONOME_MODE;			//modo general
byte state         	= MAIN_STATE;				//estado general
boolean refresh		= true;						//refresco LCD
unsigned int bpm 					= 160;		//tempo general
unsigned int lastBpm				= 0;		//tempo anterior
unsigned int clickDuration 			= 50;		//duración pulso click
unsigned int noteDivision			= QUARTER;	//subdivision nota click
unsigned int barSignature   		= 4;		//tipo compas
volatile unsigned int actualTick    = 0;		//tiempo actual
unsigned int tickSound		= NOTE_A4;			//sonido del tick
boolean tick 				= true;				//flag de activar tick
boolean play				= true;				//flag de activar metronomo
boolean equalTicks			= false;			//flag de mismo sonido para todos los ticks
byte actualNumSong			= 0;				//cancion actual del repertorio
byte actualPlayListNum      = 0;				//numero de repetorio actual
//midi
boolean midiClock			= false;			//flag de envio de midi clock
float midiClockTime;							//intervalo midi interrupcion
volatile byte midiCounter	= 0;				//contador clocks midi
//interfaz
volatile signed int deltaEnc= 0;				//incremento o decremento del encoder
unsigned int buttonDelay    = 2;				//Tiempo antirebote
unsigned int buttonLongPress= 60;				//Tiempo pulsacion larga para otras funciones
//menu
byte actualMenuPage         = MAIN_PAGE;				//página del menu actual
byte actualMenuOption 		= CHANGE_PLAYLIST_OPTION;	//opcion seleccionada del menu
//entrada texto o parametros
byte editCursor             = 0;				//posicion cursor edicion
char * editString;								//cadena a editar
byte editData				= 0;				//dato a editar
unsigned int editDataInt	= 0;				//dato a editar entero
byte editSelection			= 0;				//seleccion a editar
//interrupcion timer
volatile static unsigned long timer0Counter		= 0;
volatile static unsigned long timer0DelayTime 	= 0;
//debug
unsigned long startTime     = 0;				//tiempo de inicio de ejecucion ciclo para medir rendimiento
unsigned long lastCycleTime = 0;				//tiempo que tardo el ultimo ciclo
unsigned long minCycleTime  = 2000000;			//tiempo de ciclo minimo
unsigned long maxCycleTime  = 0;				//tiempo de ciclo maximo
byte general;

//================================
//callback de la interrupcion overflow del timer0
ISR(TIMER0_OVF_vect) {
	timer0Counter++;	
	timer0DelayTime++;
}
//sobreescribimos funcion micros
unsigned long micros()
{
	return timer0Counter;
}
//sobreescribimos funcion delay
void delay(unsigned long ms)
{
	timer0DelayTime = 0;
	while ((timer0DelayTime*31) < ms)
		{yield();}	
}

//configuracion void setup()
int main(void)
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
	
	noInterrupts(); // disable all interrupts
	TCCR0A = 0;
	TCCR0B = 0;
	TCNT0 = 255;   	//preload timer
	TCCR0B =5; 		//1024 preescaler
	TIMSK0 |= (1 << TOIE0);   // enable timer overflow interrupt
	interrupts();             // enable all interrupts  

	//asignamos interrupcion a entrada encoder A	
	attachInterrupt(0, doEncoder, CHANGE);
	
	//inicializamos display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
	
	//Midi Baud Rate 
	Serial.begin(31250);
	
	//Mensaje de inicio
	//#ifndef DEBUG
		wellcomeTest();
	//#endif
	
	//Clear the buffer.
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	
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
 

 //bucle principal void loop()
while(true)
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
		
		//comprobamos estado
		switch (state)
		{
			//si está en el menu
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
			
			break;

		//estado menu (generico para todos los modos)
		case MENU_STATE:
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
						//aceptamos el cambio de canción
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
								editDataInt = EEPROMReadInt(EEPROM_CONFIG_TICK_SOUND);
								//si no esta seteado, lo seteamos
								if (editDataInt < NOTE_B0 || editDataInt > NOTE_DS8)
									editDataInt = NOTE_A4;
								//actualizamos el sonido actual
								tickSound = editDataInt;
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
						tickSound = editDataInt;
						//guardamos en config
						EEPROMWriteInt(EEPROM_CONFIG_TICK_SOUND,tickSound);
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
}
//callback de la interrupcion que se ejecuta 24 veces por negra al tempo actual
void sendMidiClock()
{
	//comprobamos iteraccion del midiClock
	if (midiCounter >= (MIDI_TICKS_BEAT/noteDivision))
	{
		midiCounter = 0;
		//incrementamos el numero de tick del compas
		actualTick >= (barSignature-1) ? actualTick = 0 : actualTick++;
		//si está en reproduccion
		if (play)
		{
			//sonido del tick según si es el primer tiempo del compás y no está configurado ticks iguales
			if (actualTick == 0 && !equalTicks)
				tone(OUT_CLICK,NOTE_F5,clickDuration);
			else
				tone(OUT_CLICK,tickSound,clickDuration);
		}
	}
	else
		midiCounter++;
	
	//Enviamos un midi clock cada 24 veces por negra)
	if (midiClock)
		//midi.write(MIDI_CLOCK_MSG);	
		Serial.write(MIDI_CLOCK_MSG);	
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
					display.print(F(" BPM\n\n")); 
					display.setTextSize(1);
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					display.print("      ");
					play ? display.print(F("START")) : display.print("STOP");
					display.display();
					
					break;
				//modo metronomo
				case METRONOME_MODE:
					//actualizamos display del modo metronomo
					display.clearDisplay();
					display.setCursor(0,0);
					display.setTextColor(WHITE,BLACK);
					display.setTextSize(2);
					display.print(bpm); 
					display.print(F(" BPM\n\n")); 
					display.setTextSize(1);
					display.print(barSignature);
					display.print("/");
					display.print(noteDivision*4);
					display.print("      ");
					play ? display.print(F("START")) : display.print("STOP");
					display.display();
					break;
			}
			
			break;
		case MENU_STATE:
			//actualizamos display del menu
			display.clearDisplay();
			display.setCursor(0,0);
			//segun la página del menu
			switch (actualMenuPage)
			{
				//cambio de repertorio
				case PLAYLIST_CHANGE_PAGE:
					{
					display.setTextColor(WHITE,BLACK);
					display.println(F("Elige repertorio:"));
					for (int i=0;i<MAX_PLAYLISTS;i++)
					{
						actualMenuOption == i ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
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
					display.setTextColor(WHITE,BLACK);
					display.println(F("Nombre repertorio:"));
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
					display.drawFastHLine((editCursor*6), 15, 6, WHITE);
					}
					break;
				//cambio de cancion (elegimos primero el orden)
				case CHANGE_ORDER_PAGE:
					{
					display.println(F("Elige la posicion:"));
					display.setTextColor(WHITE,BLACK);
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
					free(title);
					
					title = readSongTitle(actualMenuOption);
					display.print(editData+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				//insertar cancion (elegimos primero el orden)
				case INSERT_SONG_PAGE:
					{
					display.println(F("Insertar despues de:"));
					display.setTextColor(WHITE,BLACK);
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
					char * title = readSongTitle(actualMenuOption);
					display.print(editData+2); //se insertará en la siguiente posicion 
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				//borrar cancion
				case DELETE_SONG_PAGE:
					{
					display.println(F("Borrar cancion"));
					display.setTextColor(WHITE,BLACK);
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
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;
				case CHANGE_SONG_NAME_PAGE:
					{
					display.setTextColor(WHITE,BLACK);
					display.println(F("Nombre cancion:"));
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
					display.drawFastHLine((editCursor*6), 15, 6, WHITE);
					}
					break;
				case CHANGE_SONG_TEMPO_PAGE:
					{
					display.setTextColor(WHITE,BLACK);
					display.println(F("Elige el tempo:"));
					display.println("\n");
					display.print(actualMenuOption);
					display.println(F("  BPM"));
					}
					break;
				case CHANGE_SONG_BEAT_PAGE:
					{
					display.setTextColor(WHITE,BLACK);
					display.println(F("Tipo Compas:"));
					display.println("\n");
					display.println(actualMenuOption+2);					
					}
					break;
				//elige cancion para vaciar
				case SELECT_EMPTY_SONG_PAGE:
					{
					display.setTextColor(WHITE,BLACK);
					display.println(F("Elige la cancion:"));
					char * title = readSongTitle(actualMenuOption);
					display.print(actualMenuOption+1);
					display.print(".");
					display.println(title);
					free (title);
					}
					break;	
				case TICK_SOUND_PAGE:
					{
					if (deltaEnc > 0)
					{
						editDataInt++;	
						tickSound = editDataInt;
						
						deltaEnc = 0; //para que no se mueva						
					}
					if (deltaEnc < 0)
					{
						editDataInt--;			
						tickSound = editDataInt;
						
						deltaEnc = 0; //para que no se mueva						
					}	
					display.setTextColor(WHITE,BLACK);
					display.println(F("Elige el sonido:"));
					display.println("\n");
					display.print(editDataInt);
					display.println(F("  Hz"));
					}
					break;
				//informacion
				case INFO_PAGE:
					{
					display.setTextColor(WHITE,BLACK);
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
						actualMenuOption == i ? display.setTextColor(BLACK,WHITE) : display.setTextColor(WHITE,BLACK);
						//leemos la opcion de la pagina
						strcpy_P(buffer, (char*)pgm_read_word(&(menuPage[actualMenuPage].strTable[i])));
						display.println(buffer);	
					}
					}
			}
			//mostramos pantalla
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
	
	int dataInt;
	dataInt = EEPROMReadInt(EEPROM_CONFIG_TICK_SOUND);
	if (dataInt < NOTE_B0 ||dataInt > NOTE_DS8)
		tickSound = NOTE_A4;
	else
		tickSound = dataInt;
	
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
	EEPROMWriteInt(EEPROM_CONFIG_TICK_SOUND,NOTE_A4);
	
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
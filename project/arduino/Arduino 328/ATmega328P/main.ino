/*
 BeatDuino

 Metronomo digital programable con funciones de guardar repertorios y salida midi

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 TO-DO:
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
	#define SETTINGS_PAGE			14
	
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
	#define CHANGE_SONG_OPTION          0
#define SETTINGS_OPTION				2	

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
} menuPage[13] = {	3,MAIN_PAGE,mainStr,
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
				};
				  
//==============================================

//Variables generales===========================
byte mode          = LIVE_MODE;					//modo general
byte state         = MENU_STATE;//MAIN_STATE;				//estado general
boolean refresh			= true;					//refresco LCD
unsigned int bpm 			= 100;				//tempo general
unsigned long msTempo 		= 0;				//tempo en milisegundos
unsigned int clickDuration 	= 10;				//duración pulso click
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
byte actualMenuPage         = PLAYLIST_EDIT_PAGE;//MAIN_PAGE;				//página del menu actual
byte actualMenuOption 		= CHANGE_PLAYLIST_OPTION;	//opcion seleccionada del menu

//entrada texto o parametros
byte editCursor             = 0;				//posicion cursor edicion
char * editString;								//cadena a editar
byte editData				= 0;				//dato a editar
//debug
byte general;
//================================
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
				refresh = true;
				break;
			//si no esta en el menu, salta al menu
			default:
				state = MENU_STATE;
				actualMenuOption = 0;
				actualMenuPage = MAIN_PAGE;
				
				refresh = true;
				break;
		}		
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
				actualMenuOption < menuPage[actualMenuPage].numOptions-1 ? actualMenuOption++ : actualMenuOption=0;
				refresh = true;
			}
			if (deltaEnc < 0 )
			{
				actualMenuOption > 0 ? actualMenuOption-- : actualMenuOption = menuPage[actualMenuPage].numOptions-1;
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
								
								refresh = true;
								break;														
						}
						break;
					case PLAYLIST_PAGE:
						switch (actualMenuOption)
						{
							case CHANGE_PLAYLIST_OPTION:
								actualMenuOption = 0;
								actualMenuPage = PLAYLIST_CHANGE_PAGE;
								
								refresh = true;
								break;							
							case EDIT_PLAYLIST_OPTION:
								actualMenuOption = 0;
								actualMenuPage = PLAYLIST_EDIT_PAGE;
								
								refresh = true;
								break;		
							case DELETE_PLAYLIST_OPTION:
								actualMenuOption = 0;
								actualMenuPage = PLAYLIST_DELETE_PAGE;
								
								refresh = true;
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
						refresh = true;
						
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
								refresh = true;
								break;
							//cambio ordenes
							case ORDER_OPTION:
								actualMenuOption = 0;
								actualMenuPage = ORDER_PAGE;
								
								refresh = true;
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
						refresh = true;
						break;
					case ORDER_PAGE:
						switch (actualMenuOption)
						{
							//cambiar el orden
							case CHANGE_ORDER_OPTION:
								actualMenuOption = 0;
								actualMenuPage = CHANGE_ORDER_PAGE;
								
								editData = 0;
								refresh = true;
								break;
							//insertar cancion
							case INSERT_SONG_OPTION:
								actualMenuOption = 0;
								actualMenuPage = INSERT_SONG_PAGE;
								
								editData = 0;
								refresh = true;
								break;
							//borrar cancion
							case DELETE_SONG_OPTION:
								actualMenuOption = 0;
								actualMenuPage = DELETE_SONG_PAGE;
								
								editData = 0;
								refresh = true;
								break;
							//vaciar orden
							case EMPTY_ORDER_OPTION:
								actualMenuOption = 0;
								actualMenuPage = EMPTY_ORDER_PAGE;
								
								refresh = true;
								break;
						}
						break;
					case CHANGE_ORDER_PAGE:  //elegimos posicion del orden
						editData = actualMenuOption;
						actualMenuOption = 0;
						actualMenuPage = CHANGE_ORDER_PAGE_2;
						
						
						refresh = true;
						break;
					case CHANGE_ORDER_PAGE_2:
						//aceptamos el cambio de canción
						writePlayListSong(actualPlayListNum,editData,actualMenuOption);
						actualMenuOption=editData;
						editData=0;
						actualMenuPage = CHANGE_ORDER_PAGE;
						
						readSongData();
						refresh = true;						
						break;
					case INSERT_SONG_PAGE:  
						//elegimos posicion de la inserccion
						editData = actualMenuOption;
						actualMenuOption = 0;
						actualMenuPage = INSERT_SONG_PAGE_2;
						
						
						refresh = true;
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
						
						refresh = true;						
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
						
						refresh = true;						
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
						
						refresh = true;						
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
						
						refresh = true;						
						}
						break;
				}						
			}
			
			break;
	}
	
	//si está activado el sonido del metronomo
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
	
	//resetemos valor encoder
	deltaEnc = 0;
	
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
					}
					if (deltaEnc < 0)
					{
						if ((byte)editString[editCursor] == 216) //'simbolo enter'
							editString[editCursor] = 'z';
						else if ((byte)editString[editCursor] > 32) //'space'
							editString[editCursor] = editString[editCursor]-1;
												
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
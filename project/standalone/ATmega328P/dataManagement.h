/*
 BeatDuino

 Declaracion de funciones para la gestion de los datos en memoria (repertorios y canciones)

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 */

//Constantes ===================================

//definimos la estructura datos EEPROM

const byte EEPROM_SONGS_POS						= 0x00;												//Posicion Inicio Memoria Canciones
const unsigned int EEPROM_PLAYLIST_POS			= ((MAX_SONG_TITLE+3)*MAX_SONGS)+EEPROM_SONGS_POS; 	//Posicion Inicio Memoria Repertorio
const unsigned int EEPROM_CONFIG_MODE			= ((MAX_PLAYLIST_TITLE+MAX_SONGS)*MAX_PLAYLISTS)+EEPROM_PLAYLIST_POS;//Posicion Inicio Configuracion Modo
const unsigned int EEPROM_CONFIG_EQUAL_TICKS	= EEPROM_CONFIG_MODE+1;								//Posicion Inicio Configuracion Ticks Iguales
const unsigned int EEPROM_CONFIG_MIDI_CLOCK		= EEPROM_CONFIG_EQUAL_TICKS+1;						//Posicion Inicio Configuracion Midi Clock
const unsigned int EEPROM_CONFIG_TICK_SOUND		= EEPROM_CONFIG_MIDI_CLOCK+1;						//Posicion Inicio Configuracion Sonido Ticks
const unsigned int EEPROM_CONFIG_STOP_TIMER		= EEPROM_CONFIG_TICK_SOUND+1;						//Posicion Inicio Configuracion Tiempo Temporizador Parada
const unsigned int EEPROM_CONFIG_TRIGGER_FUNC	= EEPROM_CONFIG_STOP_TIMER+1;						//Posicion Inicio Configuracion Funcion de entrada Trigger
const unsigned int EEPROM_CONFIG_TRIGGER_TYPE	= EEPROM_CONFIG_TRIGGER_FUNC+1;						//Posicion Inicio Configuracion Tipo de entrada Trigger

//textos por defecto
const char emptyPlayListStr[] PROGMEM = "VACIO  ";
const char emptySongStr[] PROGMEM = "Vacio          ";

//==============================================

//Prototipos funciones
//=====================

//funcion para leer el titulo de un playlist de memoria
char * readPlayListTitle(byte numPlayList);

//funcion para leer el titulo de una cancion de memoria
char * readSongTitle(byte numSong);

//funcion para obtener el numero de cancion de una posicion de playlist
byte getSongNum(byte playListNum,byte playListPos);

//funcion para obtener el tempo de una cancion
byte getSongTempo(byte songNum);

//funcion para obtener la division de nota de una cancion
byte getSongNoteDivision(byte songNum);

//funcion para obtener el compas de una cancion
byte getSongBarSignature(byte songNum);

//funcion para guardar el titulo de un playlist
void writePlayListTitle(byte playListNum,char * title);

//funcion para guardar una cancion en el repertorio
void writePlayListSong(byte playListNum,byte playListPos,byte songNum);

//funcion para guardar el titulo de una cancion
void writeSongTitle(byte songNum,char * title);

//funcion para escribitr el tempo de una cancion
void writeSongTempo(byte songNum,byte tempo);

//funcion para escribir la division de nota de una cancion
void writeSongNoteDivision(byte songNum,byte note);

//funcion para escribir el compas de una cancion
void writeSongBeatSignature(byte songNum,byte beat);

//funcion para inicializar la memoria de Canciones
void initSongs();

//funcion para inicializar la memoria de ordenes
void initPlayLists();

//funcion de escritura EEPROM que comprueba si el dato ya está para no reescribirlo y alargar la vida de la memoria
void EEPROM_Write(int memPos,byte data);
//Funcion para escribir int en EEPROM 
void EEPROMWriteInt(int p_address, int p_value);
//Funcion para leer ints de EEPROM
unsigned int EEPROMReadInt(int p_address);
//------------debug--------------------------
//funcion de prueba para rellenar el EEPROM de datos
void debugWriteSongs();
//funcion de prueba para escribir unos setlist en memoria eepprom
void debugWritePlayLists();
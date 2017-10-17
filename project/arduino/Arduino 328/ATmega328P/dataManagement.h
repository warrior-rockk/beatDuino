/*
 BeatDuino

 Declaracion de funciones para la gestion de los datos en memoria (repertorios y canciones)

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 */

//Constantes ===================================

//definimos la estructura datos EEPROM

const byte EEPROM_SONGS_POS						= 0x00;			//Posicion Inicio Memoria Canciones
const unsigned int EEPROM_PLAYLIST_POS			= 0x186;		//Posicion Inicio Memoria Repertorio
const unsigned int EEPROM_CONFIG_MODE			= 0x1FB;		//Posicion Inicio Configuracion Modo
const unsigned int EEPROM_CONFIG_EQUAL_TICKS	= 0x1FC;		//Posicion Inicio Configuracion Ticks Iguales
const unsigned int EEPROM_CONFIG_MIDI_CLOCK		= 0x1FD;		//Posicion Inicio Configuracion Midi Clock
const unsigned int EEPROM_CONFIG_TICK_SOUND		= 0x1FE;		//Posicion Inicio Configuracion Sonido Ticks


//textos por defecto
const char emptyPlayListStr[] PROGMEM = "VACIO  ";
const char emptySongStr[] PROGMEM = "Vacio    ";

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

//funcion de escritura EEPROM que comprueba si el dato ya est� para no reescribirlo y alargar la vida de la memoria
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
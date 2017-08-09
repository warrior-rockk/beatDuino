/*
 BeatDuino

 Declaracion de funciones para la gestion de los datos en memoria (repertorios y canciones)

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 */

//Constantes ===================================

//definimos la estructura datos EEPROM

const byte EEPROM_SONGS_POS				= 0x00;			//Posicion Inicio Memoria Canciones
const unsigned int EEPROM_PLAYLIST_POS	= 0x186;		//Posicion Inicio Memoria Canciones

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

//funcion para guardar una cancion en el repertorio
void WritePlayListSong(byte playListNum,byte playListPos,byte songNum);

//------------debug--------------------------
//funcion de prueba para rellenar el EEPROM de datos
void debugWriteSongs();
//funcion de prueba para escribir unos setlist en memoria eepprom
void debugWritePlayLists();
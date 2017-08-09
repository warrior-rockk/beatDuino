/*
 BeatDuino

 funciones para la gestion de los datos en memoria (repertorios y canciones)

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 */
#include <Arduino.h>
#include <EEPROM.h>
#include <defines.h>
#include <dataManagement.h>

//LECTURA
//=================================================================

//funcion para leer el titulo de un playlist de memoria
char * readPlayListTitle(byte numPlayList)
{
	//alocamos el espacio de memoria para el titulo del playlist
	char * buf = (char *) malloc (MAX_PLAYLIST_TITLE);
	
	//nos posicionamos en el inicio de memoria del playList que nos pasan por parametro
	int actualPlayListPos = EEPROM_PLAYLIST_POS+(38*numPlayList);
	
	//obtenemos el titulo del playlist
	for (int i=0;i<MAX_PLAYLIST_TITLE;i++)
	{
		buf[i] = EEPROM.read(actualPlayListPos);
		actualPlayListPos++;
	}
		
	//devolvemos el puntero al string
	return buf;
}

//funcion para leer el titulo de una cancion de memoria
char * readSongTitle(byte numSong)
{
	//alocamos el espacio de memoria para el titulo de la cancion
	char * buf = (char *) malloc (MAX_SONG_TITLE);
	
	//nos posicionamos en el inicio de memoria de la cancion que nos pasan por parametro
	unsigned int memPos = EEPROM_SONGS_POS+(13*numSong);
		
	//obtenemos el titulo del playlist
	for (int i=0;i<MAX_SONG_TITLE;i++)
	{
			
		buf[i] = EEPROM.read(memPos);
		memPos++;
	}
	
	//devolvemos el puntero al string
	return buf;
}

//funcion para obtener el numero de cancion de una posicion de playlist
byte getSongNum(byte playListNum,byte playListPos)
{
	return EEPROM.read(((EEPROM_PLAYLIST_POS+(38*playListNum))+8+playListPos));
}

//funcion para obtener el tempo de una cancion
byte getSongTempo(byte songNum)
{
	return EEPROM.read(((EEPROM_SONGS_POS+(13*songNum))+10));
}

//funcion para obtener la division de nota de una cancion
byte getSongNoteDivision(byte songNum)
{
	return EEPROM.read(((EEPROM_SONGS_POS+(13*songNum))+11));
}

//funcion para obtener el compas de una cancion
byte getSongBarSignature(byte songNum)
{
	return EEPROM.read(((EEPROM_SONGS_POS+(13*songNum))+12));
}

//ESCRITURA
//=================================================================

//funcion para guardar una cancion en el repertorio
void WritePlayListSong(byte playListNum,byte playListPos,byte songNum)
{
	//nos posicionamos en el inicio de memoria del playList y de la posicion de cancion
	unsigned int memPos = (EEPROM_PLAYLIST_POS+(38*playListNum))+8+playListPos;
	
	//escribimos el numero de cancion en la posicion recibida
	EEPROM.write(memPos,songNum);
		
}

//funcion de prueba para escribir unos setlist en memoria eepprom
void debugWritePlayLists()
{
	unsigned int memPos = EEPROM_PLAYLIST_POS;
	
	randomSeed(analogRead(0));
	
	for (int j=0;j<MAX_PLAYLISTS;j++)
	{
		for (int i=0;i<MAX_PLAYLIST_TITLE-1;i++)
		{
			EEPROM.write(memPos,random(65,90));
			memPos++;
		}
		EEPROM.write(memPos,'\0');
		memPos++;
		for (int i=0;i<MAX_SONGS;i++)
		{
			EEPROM.write(memPos,random(0,29));
			memPos++;
		}
	}
}

//funcion de prueba para rellenar el EEPROM de datos
void debugWriteSongs()
{	
	unsigned int memPos = EEPROM_SONGS_POS;
	
	randomSeed(analogRead(0));
	
	for (int j=0;j<MAX_SONGS;j++)
	{
		for (int i=0;i<MAX_SONG_TITLE-1;i++)
		{
			EEPROM.write(memPos,random(97,120));
			memPos++;
		}
		EEPROM.write(memPos,'\0');
		memPos++;
		EEPROM.write(memPos,random(10,250));
		memPos++;
		EEPROM.write(memPos,random(1,4));
		memPos++;
		EEPROM.write(memPos,random(2,8));
		memPos++;	
	}
	
}
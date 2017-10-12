/*
 BeatDuino

 Definiciones del programa generales

 created 1 Agosto 2017
 by Warrior / Warcom Ing.

 */
#ifndef _PROJECT_DEFINES_H
#define _PROJECT_DEFINES_H

//version
#define majorVersion 1
#define minorVersion 0

//pines IO
#define MIDI_RX		0 
#define MIDI_TX		1
#define ENC_A		2		
#define MENU_PIN   	3		
#define OLED_RESET 	4
#define START_STOP  5
#define ENC_B   	6		
#define ENTER_PIN	7

#define BEAT1_CLICK	8		
#define SND0_CLICK 	9		
#define SND1_CLICK 	10		

#define LED_CLICK   13

//botones
#define START_STOP_BT 	0
#define MENU_BT 		1
#define ENTER_BT		2

//comandos midi
#define MIDI_CLOCK_MSG		0xF8
#define MIDI_START_MSG		0xFA
#define MIDI_STOP_MSG		0xFC
//midi clock per beat
#define MIDI_TICKS_BEAT		24

//division nota
#define QUARTER		1
#define EIGHTH		2
#define SIXTEENTH   4

//modos
#define LIVE_MODE        0
#define METRONOME_MODE   1

//estados
#define MAIN_STATE		0
#define MENU_STATE		1

//repertorios y canciones
#define MAX_PLAYLISTS		3
#define MAX_PLAYLIST_TITLE  8
#define MAX_SONGS       	30
#define MAX_SONG_TITLE  	10

//tipos de sonido click
#define SND_1 			0
#define SND_2 			1
#define SND_3 			2

//defines del display
#define END_OF_LINE		128
#define LAST_LINE		7

//MENU
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


#endif //PROJECT_DEFINES_H
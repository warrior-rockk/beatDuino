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
#define minorVersion 6

//pines IO
#define MIDI_RX			0 
#define MIDI_TX			1
#define ENC_A			2		
#define MENU_PIN   		3		
#define FUNCTION_PIN	4
#define START_STOP  	5
#define ENC_B   		6		
#define ENTER_PIN		7

#define BEAT1_CLICK		8		
#define SND0_CLICK 		9		
#define SND1_CLICK 		10		
#define LED_CLICK   	11
#define TRIGGER_PIN     12

//botones
#define NUM_BUTTONS		5

#define START_STOP_BT 	0
#define MENU_BT 		1
#define ENTER_BT		2
#define FUNCTION_BT		3
#define TRIGGER_BT      4

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
#define METRONOME_MODE   0
#define LIVE_MODE        1

//estados
#define MAIN_STATE		0
#define MENU_STATE		1

//repertorios y canciones
#define MAX_SONGS       	40
#define MAX_SONG_TITLE  	16
#define MAX_PLAYLISTS		3
#define MAX_PLAYLIST_TITLE  8

//tipos de sonido click
#define SND_1 			0
#define SND_2 			1
#define SND_3 			2

//tipo de tick
#define ALL_TICKS       0
#define STRONG_TICK     1
#define WEAK_TICK       2

//defines del display
#define END_OF_LINE		128
#define LAST_LINE		7
#define LAST_MENU_LINE  6
#define LAST_CHAR       21

//tipos de funcion del trigger
#define START_STOP_FUNC	0
#define NEXT_FUNC		1
#define PREV_FUNC		2

//tipos de entrada trigger
#define PUSH_TRIGGER	0
#define SWITCH_TRIGGER  1

//MENU
//p�ginas del menu
#define MAIN_PAGE 				0
	#define PLAYLIST_PAGE			1
		#define PLAYLIST_CHANGE_PAGE	2
		#define PLAYLIST_EDIT_PAGE		3
			#define PLAYLIST_NAME_PAGE	4
			#define CHANGE_ORDER_PAGE	5
			#define CHANGE_ORDER_PAGE_2	6
			#define INSERT_SONG_PAGE	7
			#define INSERT_SONG_PAGE_2	8
			#define DELETE_SONG_PAGE	9
			#define EMPTY_ORDER_PAGE	10
		#define PLAYLIST_COPY_PAGE      11
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
		#define STOP_TIMER_PAGE				26		
		#define MIDI_CLOCK_PAGE				27
		#define TRIGGER_PAGE				28
			#define TRIGGER_FUNC_PAGE		29
			#define TRIGGER_TYPE_PAGE		30
		#define RESET_FABRIC_PAGE			31
	#define INFO_PAGE				32
#define CHANGE_METRONOME_NOTE_PAGE	33
#define CHANGE_METRONOME_BEAT_PAGE	34
#define NO_PAGE					255

//opciones menu
#define PLAYLIST_OPTION				0
	#define CHANGE_PLAYLIST_OPTION		0
	#define EDIT_PLAYLIST_OPTION		1
		#define NAME_PLAYLIST_OPTION		0	
		#define CHANGE_ORDER_OPTION			1
		#define INSERT_SONG_OPTION          2
		#define DELETE_SONG_OPTION			3
		#define EMPTY_ORDER_OPTION			4
	#define COPY_PLAYLIST_OPTION        2
	#define DELETE_PLAYLIST_OPTION		3
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
	#define STOP_TIMER_OPTION			3
	#define MIDI_CLOCK_OPTION			4
	#define TRIGGER_OPTION				5
		#define TRIGGER_FUNC_OPTION			0
		#define TRIGGER_TYPE_OPTION			1
	#define RESET_FABRIC_OPTION			6
#define INFO_OPTION					3


#endif //PROJECT_DEFINES_H
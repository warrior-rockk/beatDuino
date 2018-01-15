/*
 Textos en memoria programa idioma Ingles

 by Warrior / Warcom Ing.
 v1.0
 */
//textos opciones menu
const char str0[] PROGMEM = "Setlists";
const char str1[] PROGMEM = "Songs";
const char str2[] PROGMEM = "Settings";
const char str2_1[] PROGMEM = "Info";
const char* const mainStr[] PROGMEM = {str0, str1, str2, str2_1};

const char str3[] PROGMEM = "Change Setlist";
const char str4[] PROGMEM = "Edit Setlist";
const char str4_1[] PROGMEM = "Copy Setlist";
const char str5[] PROGMEM = "Delete Setlist";
const char* const playListStr[] PROGMEM = {str3, str4, str4_1, str5};

const char str6[] PROGMEM = "Change Name";
const char str7[] PROGMEM = "Change Order";

const char* const editPlayListStr[] PROGMEM = {str6, str7};

const char str8[] PROGMEM = "Change Order";
const char str9[] PROGMEM = "Insert Song";
const char str10[] PROGMEM = "Remove Song";
const char str11[] PROGMEM = "Empty Setlist";

const char* const changeOrderStr[] PROGMEM = {str8, str9, str10, str11};

const char str12[] PROGMEM = "NO";
const char str13[] PROGMEM = "YES";

const char* const confirmStr[] PROGMEM = {str12, str13};

const char str14[] PROGMEM = "Edit Song";
const char str15[] PROGMEM = "Delete Song";

const char* const songStr[] PROGMEM = {str14, str15};

const char str16[] PROGMEM = "Change Name";
const char str17[] PROGMEM = "Change Tempo";
const char str18[] PROGMEM = "Change Division";
const char str19[] PROGMEM = "Change Bar";

const char* const editSongStr[] PROGMEM = {str16, str17, str18, str19};

const char str20[] PROGMEM = "4";
const char str21[] PROGMEM = "8";
const char str22[] PROGMEM = "16";

const char* const noteDivisionStr[] PROGMEM = {str20, str21, str22};

const char str23[] PROGMEM 	= "Mode";
const char str24[] PROGMEM 	= "Audible Ticks";
const char str25[] PROGMEM 	= "Tick Sound";
const char str25_1[] PROGMEM = "Timer";
const char str26[] PROGMEM 	= "Midi Clock";
const char str26_1[] PROGMEM= "External Trigger";
const char str26_2[] PROGMEM= "Reset Defaults";

const char* const settingsStr[] PROGMEM = {str23, str24, str25, str25_1, str26, str26_1, str26_2};

const char str27[] PROGMEM = "Metronome";
const char str28[] PROGMEM = "Setlist";

const char* const modeStr[] PROGMEM = {str27, str28};

const char str29[] PROGMEM = "Sound 1";
const char str30[] PROGMEM = "Sound 2";
const char str31[] PROGMEM = "Sound 3";

const char* const soundsStr[] PROGMEM = {str29, str30, str31};

const char str32[] PROGMEM = "All";
const char str33[] PROGMEM = "Strong Tick";
const char str34[] PROGMEM = "Weak Tick";

const char* const equalTicksStr[] PROGMEM = {str32, str33, str34};

const char str35[] PROGMEM = "Trigger Function";
const char str35_1[] PROGMEM = "Trigger Type";

const char* const triggerStr[] PROGMEM = {str35, str35_1};

const char str36[] PROGMEM = "Start and Stop";
const char str37[] PROGMEM = "Next";
const char str38[] PROGMEM = "Previous";

const char* const triggerFuncStr[] PROGMEM = {str36, str37, str38};

const char str39[] PROGMEM = "Pushbutton";
const char str40[] PROGMEM = "Switch";

const char* const triggerTypeStr[] PROGMEM = {str39, str40};

//Opciones toolbar
const char stopOpt[] PROGMEM = "STOP";
const char playOpt[] PROGMEM = "PLAY";
const char menuOpt[] PROGMEM = "MENU";
const char delOpt[] PROGMEM = "DEL";
const char saveOpt[] PROGMEM = "SAVE";
const char backOpt[] PROGMEM = "BACK";
const char resetOpt[] PROGMEM = "RESET";
const char firstOpt[] PROGMEM = "FIRST";
const char lastOpt[] PROGMEM = "LAST";
const char timerOnOpt[] PROGMEM = "T ON ";
const char timerOffOpt[] PROGMEM = "T OFF";

//textos pantallas
const char strPlaylistChange[] PROGMEM = "Choose Setlist:";
const char strPlaylistName[] PROGMEM = "Setlist name:";
const char strChangeOrder[] PROGMEM = "Edit position:";
const char strChangeOrder2[] PROGMEM = "Choose Song:";
const char strInsertSong[] PROGMEM = "Insert before:";
const char strPlaylistCopy[] PROGMEM = "Copy current on:";
const char strSongName[] PROGMEM = "Song name:";
const char strChooseTempo[] PROGMEM = "Choose Tempo:";
const char strBarType[] PROGMEM = "Beat type:";
const char strTimeToStop[] PROGMEM = "Time to Stop:";
const char strResetting[] PROGMEM = "Resetting...";
const char strImportEEPROM[] PROGMEM = "Importting EEPROM...";
const char strExportEEPROM[] PROGMEM = "Exportting EEPROM...";
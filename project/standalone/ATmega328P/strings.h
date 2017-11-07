/*
 Textos en memoria programa

 by Warrior / Warcom Ing.
 v1.0
 */
//textos opciones menu
const char str0[] PROGMEM = "Repertorios";
const char str1[] PROGMEM = "Canciones";
const char str2[] PROGMEM = "Ajustes";
const char str2_1[] PROGMEM = "Info";
const char* const mainStr[] PROGMEM = {str0, str1, str2, str2_1};

const char str3[] PROGMEM = "Cambiar de Repertorio";
const char str4[] PROGMEM = "Editar Repertorio";
const char str4_1[] PROGMEM = "Copiar Repertorio";
const char str5[] PROGMEM = "Eliminar Repertorio";
const char* const playListStr[] PROGMEM = {str3, str4, str4_1, str5};

const char str6[] PROGMEM = "Cambiar Nombre";
const char str7[] PROGMEM = "Cambiar Orden";

const char* const editPlayListStr[] PROGMEM = {str6, str7};

const char str8[] PROGMEM = "Cambiar Orden";
const char str9[] PROGMEM = "Insertar Cancion";
const char str10[] PROGMEM = "Eliminar Cancion";
const char str11[] PROGMEM = "Vaciar Orden";

const char* const changeOrderStr[] PROGMEM = {str8, str9, str10, str11};

const char str12[] PROGMEM = "NO";
const char str13[] PROGMEM = "SI";

const char* const confirmStr[] PROGMEM = {str12, str13};

const char str14[] PROGMEM = "Editar Cancion";
const char str15[] PROGMEM = "Eliminar Cancion";

const char* const songStr[] PROGMEM = {str14, str15};

const char str16[] PROGMEM = "Cambiar Nombre";
const char str17[] PROGMEM = "Cambiar Tempo";
const char str18[] PROGMEM = "Cambiar Division";
const char str19[] PROGMEM = "Cambiar Compas";

const char* const editSongStr[] PROGMEM = {str16, str17, str18, str19};

const char str20[] PROGMEM = "4";
const char str21[] PROGMEM = "8";
const char str22[] PROGMEM = "16";

const char* const noteDivisionStr[] PROGMEM = {str20, str21, str22};

const char str23[] PROGMEM 	= "Modo";
const char str24[] PROGMEM 	= "Tiempos audibles";
const char str25[] PROGMEM 	= "Sonido Tick";
const char str25_1[] PROGMEM = "Temporizador";
const char str26[] PROGMEM 	= "Midi Clock";
const char str26_1[] PROGMEM= "Reset Fabrica";

const char* const settingsStr[] PROGMEM = {str23, str24, str25, str25_1, str26, str26_1};

const char str27[] PROGMEM = "Metronomo";
const char str28[] PROGMEM = "Repertorio";

const char* const modeStr[] PROGMEM = {str27, str28};

const char str29[] PROGMEM = "Sonido 1";
const char str30[] PROGMEM = "Sonido 2";
const char str31[] PROGMEM = "Sonido 3";

const char* const soundsStr[] PROGMEM = {str29, str30, str31};

const char str32[] PROGMEM = "Todos";
const char str33[] PROGMEM = "Tiempo fuerte";
const char str34[] PROGMEM = "Tiempo debil";

const char* const equalTicksStr[] PROGMEM = {str32, str33, str34};

//Opciones toolbar
const char stopOpt[] PROGMEM = "STOP";
const char playOpt[] PROGMEM = "PLAY";
const char menuOpt[] PROGMEM = "MENU";
const char delOpt[] PROGMEM = "BORRA";
const char saveOpt[] PROGMEM = "SALV";
const char backOpt[] PROGMEM = "ATRAS";
const char resetOpt[] PROGMEM = "RESET";
const char firstOpt[] PROGMEM = "PRIM";
const char lastOpt[] PROGMEM = "ULTI";
const char timerOnOpt[] PROGMEM = "T ON ";
const char timerOffOpt[] PROGMEM = "T OFF";
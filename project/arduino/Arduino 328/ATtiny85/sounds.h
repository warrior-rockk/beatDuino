// Audio encoded as unsigned 8-bit, 8kHz sampling rate

//array de sonidos disponibles
/*
0	0	0	no sound	
0	0	1	spare	
0	1	0	tone1 	
0	1	1	tone1 beat 1	
1	0	0	tone2 	
1	0	1	tone2 beat 1	
1	1	0	tone3 	
1	1	4	tone3 beat 1	
*/
const unsigned char snd1_beat[] PROGMEM = 
{
	81,0,0,0,124,253,189,100,51,251,140,108,23,255,124,110,21,245,132,98,39,219,156,71,69,187,187,43,100,156,213,21,120,140,223,15,124,138,217,25,112,152,197,49,88,177,171,79,61,203,144,104,41,217,130,116,35,219,130,114,43,207,144,98,65,183,166,75,90,158,189,53,110,138,203,43,120,130,205,45,118,136,195,59,126,
};

const unsigned char snd1_beat1[] PROGMEM = 
{
	81,0,0,0,127,230,46,199,122,107,151,152,44,254,0,214,97,112,144,143,61,222,41,163,151,57,199,84,123,156,105,101,206,12,237,54,147,138,118,100,197,33,206,92,105,182,72,150,142,91,146,150,53,227,33,183,114,116,128,159,54,215,53,156,143,83,164,118,101,164,107,104,191,40,203,83,134,136,130,89,198,42,193,100,127,
};

const unsigned char snd2_beat[] PROGMEM = 
{
	81,0,0,0,124,247,189,37,15,156,251,162,17,33,183,249,132,5,55,209,239,100,0,83,229,223,73,0,112,243,199,47,9,142,251,174,25,23,172,251,144,9,45,199,245,114,1,71,221,231,85,0,98,239,211,57,3,130,249,185,33,17,160,251,158,15,35,187,249,128,3,59,211,237,96,0,87,231,219,69,1,116,245,197,126,
};

const unsigned char snd2_beat1[] PROGMEM =
{
	81,0,0,0,127,231,5,165,204,0,200,171,4,228,133,20,247,96,45,255,60,77,252,30,114,239,10,153,215,0,189,183,1,220,145,13,241,108,36,254,70,66,255,40,102,244,16,141,223,2,177,193,0,211,159,8,236,120,28,251,82,56,255,49,90,248,22,128,231,5,167,204,0,200,169,4,228,132,20,247,94,46,255,60,128,
};

const unsigned char snd3_beat[] PROGMEM = 
{
  89,2,0,0,128,127,129,126,131,116,16,13,14,14,16,15,18,18,209,245,234,238,236,234,240,207,22,25,21,25,23,25,25,31,208,234,227,228,229,224,234,195,27,34,29,33,31,34,32,42,207,224,220,219,221,215,226,184,32,43,36,42,38,43,38,54,204,215,213,211,214,207,219,175,38,51,42,50,44,51,44,64,201,206,207,204,207,200,213,166,43,59,49,57,51,58,49,73,198,199,
  200,197,201,193,206,158,49,65,55,64,57,65,55,82,194,192,195,190,195,187,200,151,55,72,61,70,62,71,60,90,190,185,189,185,189,182,194,145,60,77,66,76,68,77,66,96,186,180,184,179,184,177,188,140,66,82,72,81,73,82,70,103,181,174,179,174,179,172,183,136,71,87,77,86,78,87,75,108,177,169,174,170,174,167,178,132,77,91,82,90,82,91,80,113,173,165,170,165,169,164,
  173,129,82,95,86,94,87,95,84,117,169,161,166,162,165,160,168,126,86,98,90,97,91,99,88,120,165,158,162,158,161,157,164,124,91,101,94,101,95,102,92,123,162,154,158,155,158,154,160,122,95,104,98,103,98,105,96,126,158,151,155,152,155,151,156,121,99,106,101,106,101,107,99,127,155,149,152,149,151,148,153,120,102,109,104,109,104,110,103,129,152,146,149,147,149,146,150,120,105,111,
  107,111,107,112,106,130,149,144,147,145,146,144,147,120,108,113,110,113,110,113,108,131,146,142,144,143,144,142,144,120,111,114,112,114,112,115,111,132,144,141,142,141,142,141,142,120,113,116,114,116,114,117,113,132,142,139,140,139,140,139,139,120,115,117,116,117,116,118,115,132,140,138,139,138,138,138,138,121,117,119,118,119,118,119,117,132,138,136,137,136,137,136,136,121,119,120,119,120,119,120,
  119,132,136,135,136,135,135,135,134,122,120,121,121,121,121,121,120,132,135,134,134,134,134,134,133,122,122,122,122,122,122,122,122,132,134,133,133,133,133,133,132,123,123,123,123,123,123,123,123,131,133,132,132,132,132,132,131,124,124,124,124,124,124,124,124,131,132,132,132,132,131,132,130,124,124,124,124,124,124,124,125,130,131,131,131,131,131,131,130,125,125,125,125,125,125,125,125,130,131,130,
  130,130,130,131,129,125,126,125,126,125,126,125,126,130,130,130,130,130,130,130,129,126,126,126,126,126,126,126,126,129,129,130,129,130,129,130,129,126,126,126,126,126,126,126,127,129,129,129,129,129,129,129,128,126,127,127,127,127,127,127,127,129,129,129,129,129,129,129,128,127,127,127,127,127,127,127,127,129,129,129,129,129,128,129,128,127,127,127,127,127,127,127,127,128,128,128,128,128,128,128,
  128,127,127,127,127,
};

const unsigned char snd3_beat1[] PROGMEM = 
{
  87,2,0,0,128,127,129,126,130,119,13,0,9,0,202,255,250,239,22,0,12,0,203,255,246,232,23,6,16,5,204,250,243,224,25,11,20,11,204,244,239,218,27,17,23,18,204,238,236,212,29,23,26,25,203,233,232,206,30,29,29,31,203,228,228,200,32,34,32,37,202,223,225,195,35,39,35,43,201,218,221,189,37,44,38,49,200,214,218,184,39,48,41,54,199,210,214,180,42,52,
  44,60,197,205,211,175,44,57,47,65,196,201,208,171,47,61,50,69,194,198,205,167,49,64,53,74,192,194,202,163,52,68,55,78,191,190,199,159,54,71,58,82,189,187,196,156,57,74,61,86,187,184,193,153,60,77,63,90,185,181,190,150,62,80,66,94,183,178,188,147,65,83,68,97,181,175,185,144,68,86,71,100,179,173,182,142,70,88,73,103,177,170,180,139,73,90,75,106,175,168,
  177,137,75,92,78,108,173,166,175,135,78,94,80,110,171,164,172,134,80,96,82,113,169,162,170,132,83,98,84,115,167,160,168,131,85,100,87,117,165,158,166,129,87,101,89,118,163,156,164,128,90,103,91,120,162,154,162,127,92,104,93,121,160,153,160,126,94,106,95,123,158,151,158,125,96,107,96,124,157,150,156,124,98,108,98,125,155,149,154,124,100,109,100,126,153,148,153,123,101,111,
  102,127,152,146,151,123,103,112,103,128,150,145,150,122,105,113,105,128,149,144,148,122,106,113,106,129,148,143,147,122,108,114,108,129,146,142,145,122,109,115,109,130,145,141,144,122,110,116,110,130,144,140,143,122,112,117,111,130,143,140,142,122,113,117,113,131,142,139,141,122,114,118,114,131,141,138,140,122,115,119,115,131,140,137,139,122,116,119,116,131,139,137,138,122,117,120,117,131,138,136,
  137,122,118,120,118,131,137,136,136,122,119,121,119,131,136,135,135,123,120,121,119,131,136,135,135,123,120,122,120,131,135,134,134,123,121,122,121,131,134,134,133,123,122,123,122,131,134,133,133,123,122,123,122,131,133,133,132,124,123,123,123,131,133,132,132,124,123,124,123,131,132,132,131,124,124,124,124,130,132,132,131,124,124,124,124,130,131,131,130,125,124,125,125,130,131,131,130,125,125,125,
  125,130,131,131,130,125,125,125,125,130,130,130,130,125,125,125,126,130,130,130,129,126,126,126,126,130,130,130,129,126,126,126,126,129,130,130,129,126,126,126,126,129,129,129,129,126,126,126,127,129,129,129,129,126,127,126,127,129,129,129,128,126,127,127,127,129,129,129,128,127,127,127,127,129,129,129,128,127,127,127,127,129,129,129,128,127,127,127,127,129,128,129,128,127,127,127,127,128,128,128,
  128,127,127,
};

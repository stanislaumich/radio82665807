////////////////////////////////////////////////////////////////////////
//
// переменные базовые и для работы для настройки радио
//
////////////////////////////////////////////////////////////////////////
//#include "radio.h"
//#include "RDA5807M.h"

#ifndef stasradio
#define stasradio
#endif
#include <EEPROM.h>

// назначение пинов для кнопок управления
#define pbstop D0
#define pbprev D3
#define pbnext D4

static volatile  int volmax = 15; // громкость максимальная
static volatile  int vol = 2; // громкость
static volatile  int voladdr = 1; // громкость адрес в eeprom


static volatile  int frqmin = 860; // частота мин
static volatile  int frqmax = 1080; // частота мах
static volatile  int frq = 1043; // частота
static volatile  int frqstep = 10; // частота шаг
static volatile  int frqaddr1 = 2; // частота адрес в eeprom 1я*256+2я ячейка, 1043=1024+17, 1=4, 2=17
static volatile  int frqaddr2 = 3; // частота адрес в eeprom

static volatile int tries = 50; // попыток соединиться с вайфай


////////////////////////////////////////////////////////////////////////
//
// Веб Сервер
//
////////////////////////////////////////////////////////////////////////

const char* serverIndex = "<font size=40><form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form><br><br><a href='/m'>(unMute)</a><br><br><a href='/seekup'>(seekup)</a><br><a href='/seekdown'>(seekdown)</a><br><br>VOLUME <a href='/vol+'>(+)</a> <a href='/vol-'>(-)</a><br><br>FRQ <a href='/frq+'>(+)</a> <a href='/frq-'>(-)</a><br><br>"; 
const char* volindex = "<font size=40><a href='/'>MAIN</a> VOL <a href='/vol+'>(+)</a> <a href='/vol-'>(-)</a> VOL=";
const char* frqindex = "<font size=40><a href='/'>MAIN</a> FRQ <a href='/frq+'>(+)</a> <a href='/frq-'>(-)</a> FRQ=";
const char* helpindex = "<font size=40><a href='/'>MAIN</a><br>VOLUME <a href='/vol+'>(+)</a><a href='/vol-'>(-)</a><br><br>FRQ <a href='/frq+'>(+)</a> <a href='/frq-'>(-)</a><br><br><a href='/m'>(unMute)</a><br><br><a href='/s?frq=10340'>(set frq)</a><br><br><a href='/seekup'>(seekup)</a><br><a href='/seekdown'>(seekdown)</a>"; 



////////////////////////////////////////////////////////////////////////
//
// переменные для работы для настройки радио
//
////////////////////////////////////////////////////////////////////////

#define FIX_BAND     RADIO_BAND_FM   ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION  10340            ///< The station that will be tuned by this sketch is 103.40 MHz.
#define FIX_VOLUME   1               ///< The volume that will be set by this sketch is level 4.


////////////////////////////////////////////////////////////////////////
//
// функции для работы для настройки радио
//
////////////////////////////////////////////////////////////////////////
void nop(void)
{
    
}
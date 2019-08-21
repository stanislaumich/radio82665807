////////////////////////////////////////////////////////////////////////
//
// переменные базовые и для работы для настройки радио
//
////////////////////////////////////////////////////////////////////////
#ifndef stasradio
#define stasradio
#endif
#include <EEPROM.h>



static volatile  int volmax = 15; // громкость максимальная
static volatile  int vol = 2; // громкость
static volatile  int voladdr = 2; // громкость адрес в eeprom


static volatile  int frqmin = 860; // частота мин
static volatile  int frqmax = 1080; // частота мах
static volatile  int frq = 1043; // частота
static volatile  int frqstep = 1; // частота шаг
static volatile  int frqaddr1 = 0; // частота адрес в eeprom 1я*256+2я ячейка, 1043=1024+17, 1=4, 2=17
static volatile  int frqaddr2 = 1; // частота адрес в eeprom

static volatile int tries = 50; // попыток соединиться с вайфай


////////////////////////////////////////////////////////////////////////
//
// Веб Сервер
//
////////////////////////////////////////////////////////////////////////

const char* serverIndex = "<font size=40><form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form><br><br>VOLUME <a href='/vol+'>(+)</a> <a href='/vol-'>(-)</a><br><br>FRQ <a href='/frq+'>(+)</a> <a href='/frq-'>(-)</a><br><br>"; 
const char* volindex = "<font size=40><a href='/'>MAIN</a> VOL <a href='/vol+'>(+)</a> <a href='/vol-'>(-)</a> VOL=";
const char* frqindex = "<font size=40><a href='/'>MAIN</a> FRQ <a href='/frq+'>(+)</a> <a href='/frq-'>(-)</a> FRQ=";



////////////////////////////////////////////////////////////////////////
//
// функции для работы для настройки радио
//
////////////////////////////////////////////////////////////////////////
void setvol(int vol)
{
 int t = vol;
 //EEPROM.write(voladdr, vol);
}

void setfrq(int frq)
{
 int t = frq;
 //EEPROM.write(voladdr, vol);
}
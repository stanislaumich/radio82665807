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


static volatile  int frqmiin = 860; // частота мин
static volatile  int frqmax = 1080; // частота мах
static volatile  int frq = 1043; // частота
static volatile  int frqstep = 1; // частота шаг
static volatile  int frqaddr1 = 0; // частота адрес в eeprom 1я*256+2я ячейка, 1043=1024+17, 1=4, 2=17
static volatile  int frqaddr2 = 1; // частота адрес в eeprom



////////////////////////////////////////////////////////////////////////
//
// функции для работы для настройки радио
//
////////////////////////////////////////////////////////////////////////
void setvol(int vol)
{
 int t = vol;
 EEPROM.write(voladdr, vol);
}
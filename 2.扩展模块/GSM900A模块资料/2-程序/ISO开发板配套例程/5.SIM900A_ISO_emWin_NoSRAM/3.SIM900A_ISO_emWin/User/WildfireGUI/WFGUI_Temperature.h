#ifndef __WFGUI_TEMPERATURE_H
#define __WFGUI_TEMPERATURE_H

#include "WFGUI_Common.h"

/* ͼ�α������� */
#define SCALE_FACTOR 2

enum SENSOR_TYPE{DHT11,DS18B20,INTERNAL};		//DHT11��������DS18B20��������оƬ�ڲ��¶ȼ��

void WFGUI_Temperature(void);

#endif


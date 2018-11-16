#ifndef __WFGUI_TEMPERATURE_H
#define __WFGUI_TEMPERATURE_H

#include "WFGUI_Common.h"

/* 图形比例因子 */
#define SCALE_FACTOR 2

enum SENSOR_TYPE{DHT11,DS18B20,INTERNAL};		//DHT11传感器，DS18B20传感器，芯片内部温度检测

void WFGUI_Temperature(void);

#endif


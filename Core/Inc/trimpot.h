#include "cmsis_os.h"
#include "main.h"
#include <stdbool.h>
#ifndef TRIMPOT_H
#define TRIMPOT_H



typedef struct {

	uint8_t potNumber;
	float Readings[10];
	float highestReading;
	float lowestReading;
	uint16_t highCount;
	uint16_t lowCount;
	uint32_t lastHighTime;
	uint32_t lastLowTime;
	uint32_t highestTime;
	uint32_t lowestTime;


} trimpotSensor;



trimpotSensor *initTrimpot(uint8_t);






float readADC(trimpotSensor*,ADC_HandleTypeDef*,TIM_HandleTypeDef*);

bool printValues(trimpotSensor*,UART_HandleTypeDef*);


#endif

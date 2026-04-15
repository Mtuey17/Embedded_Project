#include "trimpot.h"
#include <stdlib.h>
#include "cmsis_os.h"
#include <stdbool.h>
#include "main.h"
#include <stdio.h>
trimpotSensor *initTrimpot(uint8_t num){





	trimpotSensor *tp_instance = malloc(sizeof(trimpotSensor));

	for (int i=0;i<10;i++){tp_instance->Readings[i]=0.0;}
	tp_instance->potNumber=num;
	tp_instance-> highestReading=0.0;
	tp_instance-> lowestReading=100.0;
	tp_instance-> highCount=0;
	tp_instance-> lowCount=0;
	tp_instance-> lastHighTime=0;
	tp_instance-> lastLowTime=0;
	tp_instance-> highestTime=0;
	tp_instance-> lowestTime=0;
	return tp_instance;
}






float readADC(trimpotSensor *trimpot ,ADC_HandleTypeDef *PORT){


	uint16_t currentReading = HAL_ADC_GetValue(PORT);
	float multiplyer=3.32f/4095.0f;
	float rawValue=multiplyer*currentReading;

	//shifting all values 1 position
	for (int i=9; i>0; i--){trimpot->Readings[i]=trimpot->Readings[i-1];}

	//applying low pass filter
	float smoothingFactor=0.9;
	trimpot->Readings[0] =smoothingFactor * rawValue + (1 - smoothingFactor) * trimpot->Readings[1];

	if (trimpot->Readings[0]>(3.32f*.9f)){trimpot->highCount++; trimpot->lastHighTime=HAL_GetTick();}
	if (trimpot->Readings[0]<(3.32f*.1f)){trimpot->lowCount++; trimpot->lastLowTime=HAL_GetTick();}
	if (trimpot->Readings[0]>trimpot->highestReading){
		trimpot->highestReading=trimpot->Readings[0];
		trimpot->highestTime=HAL_GetTick();
	}
	if (trimpot->Readings[0]<trimpot->lowestReading){
		trimpot->lowestReading=trimpot->Readings[0];
		trimpot->lowestTime=HAL_GetTick();
	}



return trimpot->Readings[0];
}

bool printValues(trimpotSensor *trimpot,UART_HandleTypeDef *UART){

	char buf[64];
	int len = sprintf(buf, "-------- POT %d --------\r\n", trimpot->potNumber);
	HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);
	len = sprintf(buf, "High Readings: %d last @ %0.3fs \r\n", trimpot->highCount,trimpot->lastHighTime/1000.0f);
	HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);
	len = sprintf(buf, "Low Readings: %d last @ %0.3fs \r\n", trimpot->lowCount,trimpot->lastLowTime/1000.0f);
	HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);


	len = sprintf(buf, "Highest Reading: %0.3f @ %0.3fs \r\n", trimpot->highestReading,trimpot->highestTime/1000.0f);
	HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);
	len = sprintf(buf, "Lowest Reading: %0.3f @ %0.3fs \r\n", trimpot->lowestReading,trimpot->lowestTime/1000.0f);
	HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);


	len = sprintf(buf, "Last 10 Readings: ");
		HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);
	for (int i=0;i<10;i++){
		len = sprintf(buf, "%0.2f,", trimpot->Readings[i]);
		HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);
	}
	len = sprintf(buf, "\r\n------------------------\r\n\n");
	HAL_UART_Transmit(UART, (uint8_t*)buf, len,100);





return false;
}


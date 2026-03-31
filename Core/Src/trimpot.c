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
	tp_instance-> lowestReading=0.0;
	tp_instance-> highCount=0;
	tp_instance-> lowCount=0;
	tp_instance-> highPressTime=0;
	tp_instance-> lowPressTime=0;
	return tp_instance;
}



float readADC(trimpotSensor *trimpot ,ADC_HandleTypeDef *PORT){

	HAL_ADC_Start(PORT);
	HAL_ADC_PollForConversion(PORT, 50);
	uint16_t currentReading = HAL_ADC_GetValue(PORT);
	float multiplyer=3.32f/4095.0f;
	float rawValue=multiplyer*currentReading;
	HAL_ADC_Stop(PORT);

	//shifting all values 1 position
	for (int i=9; i>0; i--){trimpot->Readings[i]=trimpot->Readings[i-1];}

	//applying low pass filter
	//float smoothingFactor=0.1;
	trimpot->Readings[0] =rawValue;//smoothingFactor * rawValue + (1 - smoothingFactor) * trimpot->Readings[1];

	if (trimpot->Readings[0]>(3.32f*.9f)){trimpot->highCount++;}
	if (trimpot->Readings[0]<(3.32f*.1f)){trimpot->lowCount++;}



return trimpot->Readings[0];
}

bool printValues(trimpotSensor *trimpot,UART_HandleTypeDef UART){

	 char buf[64];
	int len = sprintf(buf, "-------- POT %d --------\r\n", trimpot->potNumber);
	HAL_UART_Transmit(&UART, (uint8_t*)buf, len,100);
	len = sprintf(buf, "High Readings: %d \r\n", trimpot->highCount);
	HAL_UART_Transmit(&UART, (uint8_t*)buf, len,100);
	len = sprintf(buf, "Low Readings: %d \r\n", trimpot->lowCount);
	HAL_UART_Transmit(&UART, (uint8_t*)buf, len,100);
	len = sprintf(buf, "Last 10 Readings: ");
		HAL_UART_Transmit(&UART, (uint8_t*)buf, len,100);
	for (int i=0;i<10;i++){
		len = sprintf(buf, "%0.2f,", trimpot->Readings[i]);
		HAL_UART_Transmit(&UART, (uint8_t*)buf, len,100);
	}

	len = sprintf(buf, "\n\r");
	HAL_UART_Transmit(&UART, (uint8_t*)buf, len,100);






return false;
}


/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "trimpot.h"

#include <string.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


trimpotSensor *Pot1;
trimpotSensor *Pot2;



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

UART_HandleTypeDef huart2;

/* Definitions for PotOneTask */
osThreadId_t PotOneTaskHandle;
uint32_t PotOneTaskBuffer[ 256 ];
osStaticThreadDef_t PotOneTaskControlBlock;
const osThreadAttr_t PotOneTask_attributes = {
  .name = "PotOneTask",
  .cb_mem = &PotOneTaskControlBlock,
  .cb_size = sizeof(PotOneTaskControlBlock),
  .stack_mem = &PotOneTaskBuffer[0],
  .stack_size = sizeof(PotOneTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PotTwoTask */
osThreadId_t PotTwoTaskHandle;
uint32_t PotTwoTaskBuffer[ 256 ];
osStaticThreadDef_t PotTwoTaskControlBlock;
const osThreadAttr_t PotTwoTask_attributes = {
  .name = "PotTwoTask",
  .cb_mem = &PotTwoTaskControlBlock,
  .cb_size = sizeof(PotTwoTaskControlBlock),
  .stack_mem = &PotTwoTaskBuffer[0],
  .stack_size = sizeof(PotTwoTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PrinterTask */
osThreadId_t PrinterTaskHandle;
uint32_t PrinterTaskBuffer[ 128 ];
osStaticThreadDef_t PrinterTaskControlBlock;
const osThreadAttr_t PrinterTask_attributes = {
  .name = "PrinterTask",
  .cb_mem = &PrinterTaskControlBlock,
  .cb_size = sizeof(PrinterTaskControlBlock),
  .stack_mem = &PrinterTaskBuffer[0],
  .stack_size = sizeof(PrinterTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ButtonTask */
osThreadId_t ButtonTaskHandle;
uint32_t ButtonTaskBuffer[ 128 ];
osStaticThreadDef_t ButtonTaskControlBlock;
const osThreadAttr_t ButtonTask_attributes = {
  .name = "ButtonTask",
  .cb_mem = &ButtonTaskControlBlock,
  .cb_size = sizeof(ButtonTaskControlBlock),
  .stack_mem = &ButtonTaskBuffer[0],
  .stack_size = sizeof(ButtonTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for LEDTask */
osThreadId_t LEDTaskHandle;
uint32_t LEDTaskBuffer[ 128 ];
osStaticThreadDef_t LEDTaskControlBlock;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .cb_mem = &LEDTaskControlBlock,
  .cb_size = sizeof(LEDTaskControlBlock),
  .stack_mem = &LEDTaskBuffer[0],
  .stack_size = sizeof(LEDTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for PotOneMutex */
osMutexId_t PotOneMutexHandle;
osStaticMutexDef_t PrinterMutexControlBlock;
const osMutexAttr_t PotOneMutex_attributes = {
  .name = "PotOneMutex",
  .cb_mem = &PrinterMutexControlBlock,
  .cb_size = sizeof(PrinterMutexControlBlock),
};
/* Definitions for PotTwoMutex */
osMutexId_t PotTwoMutexHandle;
osStaticMutexDef_t PotTwoMutexControlBlock;
const osMutexAttr_t PotTwoMutex_attributes = {
  .name = "PotTwoMutex",
  .cb_mem = &PotTwoMutexControlBlock,
  .cb_size = sizeof(PotTwoMutexControlBlock),
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC2_Init(void);
void StartPotOneTask(void *argument);
void StartPotTwoTask(void *argument);
void StartPrinterTask(void *argument);
void StartButtonTask(void *argument);
void StartLEDTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 ADCs are ijn interupt mode
 this function will be called when either of
 the ADCs are ready
 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */

  Pot1=initTrimpot(1);
  Pot2=initTrimpot(2);




  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of PotOneMutex */
  PotOneMutexHandle = osMutexNew(&PotOneMutex_attributes);

  /* creation of PotTwoMutex */
  PotTwoMutexHandle = osMutexNew(&PotTwoMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of PotOneTask */
  PotOneTaskHandle = osThreadNew(StartPotOneTask, NULL, &PotOneTask_attributes);

  /* creation of PotTwoTask */
  PotTwoTaskHandle = osThreadNew(StartPotTwoTask, NULL, &PotTwoTask_attributes);

  /* creation of PrinterTask */
  PrinterTaskHandle = osThreadNew(StartPrinterTask, NULL, &PrinterTask_attributes);

  /* creation of ButtonTask */
  ButtonTaskHandle = osThreadNew(StartButtonTask, NULL, &ButtonTask_attributes);

  /* creation of LEDTask */
  LEDTaskHandle = osThreadNew(StartLEDTask, NULL, &LEDTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MORSE_CODE_BUTTON_Pin START_COMMAND_BUTTON_Pin */
  GPIO_InitStruct.Pin = MORSE_CODE_BUTTON_Pin|START_COMMAND_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : STATUS_LED_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


bool pot1Ready=false;
bool pot2Ready=false;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){

	if (hadc==&hadc1){


		pot1Ready=true;
	}
	else{
		pot2Ready=true;
	};

}





/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartPotOneTask */
/**
  * @brief  Function implementing the PotOneTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartPotOneTask */
void StartPotOneTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */



  for(;;)
	{
		pot1Ready = false;
		HAL_ADC_Start_IT(&hadc1);

		osDelay(10);   // enough time for conversion to finish

		if (pot1Ready) {
			osMutexAcquire(PotOneMutexHandle, osWaitForever);
			readADC(Pot1, &hadc1);
			osMutexRelease(PotOneMutexHandle);
		}

		osDelay(490);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartPotTwoTask */
/**
* @brief Function implementing the PotTwoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPotTwoTask */
void StartPotTwoTask(void *argument)
{
  /* USER CODE BEGIN StartPotTwoTask */
  /* Infinite loop */
	for(;;)
	{
		pot2Ready = false;
		HAL_ADC_Start_IT(&hadc2);
		osDelay(10);
		if (pot2Ready) {
			osMutexAcquire(PotTwoMutexHandle, osWaitForever);
			readADC(Pot2, &hadc2);
			osMutexRelease(PotTwoMutexHandle);
		}

		osDelay(490);
	}
  /* USER CODE END StartPotTwoTask */
}

/* USER CODE BEGIN Header_StartPrinterTask */
/**
* @brief Function implementing the PrinterTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPrinterTask */
void StartPrinterTask(void *argument)
{
  /* USER CODE BEGIN StartPrinterTask */
  /* Infinite loop */
  osThreadSuspend(PrinterTaskHandle);
  for(;;)
  {

	osMutexAcquire(PotOneMutexHandle, osWaitForever);
	printValues(Pot1,&huart2);
	osMutexRelease(PotOneMutexHandle);

	osMutexAcquire(PotTwoMutexHandle, osWaitForever);
	printValues(Pot2,&huart2);
	osMutexRelease(PotTwoMutexHandle);

    osDelay(500);
  }
  /* USER CODE END StartPrinterTask */
}

/* USER CODE BEGIN Header_StartButtonTask */
/**
* @brief Function implementing the ButtonTask thread.
* @param argument: Not used
* @retval None
*/

bool startButtonTask=false;
void commandStartButton(){
	startButtonTask=true;
}
bool instructionStarted=false;
char currentWord[5]="";
uint8_t currentIndex=0;
bool getMainCommand=true;
char mainCommand;
char subCommand;

char checkLetter(char  currentSequence[]){



	if (strcmp(currentSequence, ".-") == 0) return 'A';
	if (strcmp(currentSequence, "-...") == 0) return 'B';
	if (strcmp(currentSequence, "-.-.") == 0) return 'C';
	if (strcmp(currentSequence, "-..") == 0) return 'D';
	if (strcmp(currentSequence, ".") == 0) return 'E';
	if (strcmp(currentSequence, "..-.") == 0) return 'F';
	if (strcmp(currentSequence, "--.") == 0) return 'G';
	if (strcmp(currentSequence, "....") == 0) return 'H';


	if (strcmp(currentSequence, ".----") == 0) return '1';
	if (strcmp(currentSequence, "..---") == 0) return '2';
	if (strcmp(currentSequence, "...--") == 0) return '3';
	if (strcmp(currentSequence, "....-") == 0) return '4';
	if (strcmp(currentSequence, ".....") == 0) return '5';
	if (strcmp(currentSequence, "-....") == 0) return '6';
	if (strcmp(currentSequence, "--...") == 0) return '7';
	if (strcmp(currentSequence, "---..") == 0) return '8';
	if (strcmp(currentSequence, "----.") == 0) return '9';
	if (strcmp(currentSequence, "-----") == 0) return '0';
	return 'z';
}

bool commandSet=false;

/* USER CODE END Header_StartButtonTask */
void StartButtonTask(void *argument)
{
  /* USER CODE BEGIN StartButtonTask */
	  /* Infinite loop */
    uint32_t pressTime = 0;//lst time button was pressed
    uint32_t holdTime = 0;//how long button was held
    uint32_t lastSymbolTime = 0;//time of most recent dot/dash


    //storing previous button state
    GPIO_PinState lastState = 1;

    const uint32_t dashThreshold = 250;
    const uint32_t debounceDelay = 20;
    for (;;)
    {

    	if (startButtonTask){
    	//reading button
        GPIO_PinState currentState = HAL_GPIO_ReadPin(MORSE_CODE_BUTTON_GPIO_Port,MORSE_CODE_BUTTON_Pin);


        //if state has changed since last read
        if (currentState != lastState)
        {
        	//delaying for debounce
            osDelay(debounceDelay);

            //reading state again
            currentState = HAL_GPIO_ReadPin(MORSE_CODE_BUTTON_GPIO_Port,MORSE_CODE_BUTTON_Pin);


            //checking if state has changed
            if (currentState != lastState)
            {
            	//updating state
                lastState = currentState;

                //if button is pressed getting time
                if (currentState == 0){ pressTime = HAL_GetTick();}
                else//if button is released
                {
                	//seeing how long the button was held for
                    holdTime = HAL_GetTick() - pressTime;

                        //if hold time is below threshold, dot
                        if (holdTime < dashThreshold){currentWord[currentIndex] = '.';}
                        else{currentWord[currentIndex] = '-';}
                        currentIndex++;
                        currentWord[currentIndex] = '\0';//adding null terminator to next spot


                    //showing that user has started to input data
                    instructionStarted = true;
                    lastSymbolTime = HAL_GetTick();//updating tiestamp

                    //printing morse code combo every button press
                    HAL_UART_Transmit(&huart2, (uint8_t*)currentWord, strlen(currentWord), HAL_MAX_DELAY);
                    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
                }
            }
        }

        //if there is a significant wait between button presses and
        //the user has started to give instructions, assume user is done
        //and find corresponding char

        if ( ((HAL_GetTick() - lastSymbolTime > 2000) && instructionStarted)||currentIndex>=4)
        {
        	//chcking for a matching char

        	if (getMainCommand){
        		mainCommand = checkLetter(currentWord);
        		if (mainCommand!='z'){getMainCommand=false;}

        		HAL_UART_Transmit(&huart2, (uint8_t*)"Main Command: ", 14, HAL_MAX_DELAY);
        		HAL_UART_Transmit(&huart2, (uint8_t*)&mainCommand, 1, HAL_MAX_DELAY);
        		HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);

        	}
        	else{
        		subCommand = checkLetter(currentWord);
        		if (subCommand!='z'){
        			getMainCommand=true;
        		 HAL_UART_Transmit(&huart2, (uint8_t*)"Sub Command: ", 13, HAL_MAX_DELAY);
        		            HAL_UART_Transmit(&huart2, (uint8_t*)&subCommand, 1, HAL_MAX_DELAY);
        		            HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
        		            startButtonTask=false;
        		            commandSet=true;
        		}

        	}



            instructionStarted = false;
            currentIndex = 0;
            currentWord[0] = '\0';
        }
    	}
        osDelay(5);
    }
  /* USER CODE END StartButtonTask */
}

/* USER CODE BEGIN Header_StartLEDTask */
/**
* @brief Function implementing the LEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLEDTask */
void StartLEDTask(void *argument)
{
  /* USER CODE BEGIN StartLEDTask */
  /* Infinite loop */
	uint16_t delayTime=0;
  for(;;)
  {

    if (!startButtonTask&&!commandSet){HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, 0); delayTime=1;}
    else{
    	if (commandSet&&!startButtonTask){HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, 1); delayTime=1;}
    	else{
    	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    	if (getMainCommand){delayTime=500;}
    	else{delayTime=250;}
    	}
    }
    osDelay(delayTime);
  }
  /* USER CODE END StartLEDTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

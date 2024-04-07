/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "gpio.h"
#include "cmd.h"
#include "MS5837Task.h"
#include "MS5837.h"
#include <string.h>
#include <stdlib.h>
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for cmdTask */
osThreadId_t cmdTaskHandle;
const osThreadAttr_t cmdTask_attributes = {
  .name = "cmdTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ms5837Task */
osThreadId_t ms5837TaskHandle;
const osThreadAttr_t ms5837Task_attributes = {
  .name = "ms5837Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for txTask */
osThreadId_t txTaskHandle;
const osThreadAttr_t txTask_attributes = {
  .name = "txTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for rxQueue */
osMessageQueueId_t rxQueueHandle;
const osMessageQueueAttr_t rxQueue_attributes = {
  .name = "rxQueue"
};
/* Definitions for txQueue */
osMessageQueueId_t txQueueHandle;
const osMessageQueueAttr_t txQueue_attributes = {
  .name = "txQueue"
};
/* Definitions for usart8Mutex */
osMutexId_t usart8MutexHandle;
const osMutexAttr_t usart8Mutex_attributes = {
  .name = "usart8Mutex"
};
/* Definitions for ms5837BinarySem */
osSemaphoreId_t ms5837BinarySemHandle;
const osSemaphoreAttr_t ms5837BinarySem_attributes = {
  .name = "ms5837BinarySem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void CmdTask(void *argument);
extern void Ms5837Task(void *argument);
extern void TxTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of usart8Mutex */
  usart8MutexHandle = osMutexNew(&usart8Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of ms5837BinarySem */
  ms5837BinarySemHandle = osSemaphoreNew(1, 0, &ms5837BinarySem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of rxQueue */
  rxQueueHandle = osMessageQueueNew (3, sizeof(rxStruct), &rxQueue_attributes);

  /* creation of txQueue */
  txQueueHandle = osMessageQueueNew (3, sizeof(txStruct), &txQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of cmdTask */
  cmdTaskHandle = osThreadNew(CmdTask, NULL, &cmdTask_attributes);

  /* creation of ms5837Task */
  ms5837TaskHandle = osThreadNew(Ms5837Task, NULL, &ms5837Task_attributes);

  /* creation of txTask */
  txTaskHandle = osThreadNew(TxTask, NULL, &txTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

  // unsigned char s_buf[]="hello world\r\n";
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);
    osDelay(60);
    // if(HAL_LOCKED == huart3.Lock)
    // { 
    //   __HAL_UNLOCK(&huart3);      // 暴力解锁
    // }
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


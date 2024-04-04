#include "bsp_soft_i2c.h"
#include "tim.h"

#define USE_FREE_RTOS 1
#define DENSITY 1029

#if USE_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h" //os 使用
#include "cmsis_os.h"
#endif

typedef struct 
{
    float temperture;
    float pressure;
    float depth;
}MS5837_data;


void MS5837_init(void);
void MS583703BA_RESET(void);
MS5837_data MS5837_Getdata(void);
uint32_t MS583703BA_getConversion(uint8_t command);

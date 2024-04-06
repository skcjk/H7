#include "MS5837Task.h"
#include "MS5837.h"
#include "cmd.h"
#include <string.h>
#include <stdlib.h>

extern osSemaphoreId_t ms5837BinarySemHandle;

void Ms5837Task(void *argument)
{
    MS5837_data data;

    char return_str[100];

    MS5837_init();
    while(1)
    {
        if (osSemaphoreAcquire(ms5837BinarySemHandle, osWaitForever) == osOK){
            data = MS5837_Getdata();

            sprintf(return_str, "{\n\t\"data\": \"ms5837\",\n\t\"temperature\": %.2f,\n\t\"pressure\": %.2f,\n\t\"depth\": %.2f\n}", data.temperture, data.pressure, data.depth);

            usart3Printf(return_str, strlen(return_str));
        }

        osDelay(50);
    }
}

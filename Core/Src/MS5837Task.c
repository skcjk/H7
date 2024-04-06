#include "MS5837Task.h"
#include "MS5837.h"
#include "cmd.h"
#include <string.h>
#include <stdlib.h>

extern osSemaphoreId_t ms5837BinarySemHandle;
uint8_t ms5837ScanMode = 0;

void Ms5837Task(void *argument)
{
    MS5837_data data;

    char return_str[100];

    MS5837_init();
    while (1)
    {
        if (osSemaphoreAcquire(ms5837BinarySemHandle, 50) == osOK || ms5837ScanMode == 1)
        {
            data = MS5837_Getdata();

            sprintf(return_str, "{\n\t\"return\":\n\t\"ms5837\",\n\t\"temperature\":\n\t%.2f,\n\t\"pressure\":\n\t%.2f,\n\t\"depth\":\n\t%.2f}", data.temperture, data.pressure, data.depth);

            usart3Printf(return_str, strlen(return_str));
        }
    }
}

#include "MS5837Task.h"
#include "MS5837.h"
#include "cmd.h"
#include <string.h>
#include <stdlib.h>

extern osSemaphoreId_t ms5837BinarySemHandle;
extern osMessageQueueId_t txQueueHandle;
uint8_t ms5837ScanMode = 0;

void Ms5837Task(void *argument)
{
    MS5837_data data;


    MS5837_init();
    while (1)
    {
        txStruct transmitRxToQuene;
        if (osSemaphoreAcquire(ms5837BinarySemHandle, 50) == osOK || ms5837ScanMode == 1)
        {
            data = MS5837_Getdata();

            sprintf((char*)transmitRxToQuene.tx_buf, "{\n\t\"return\":\n\t\"ms5837\",\n\t\"temperature\":\n\t%.2f,\n\t\"pressure\":\n\t%.2f,\n\t\"depth\":\n\t%.2f}", data.temperture, data.pressure, data.depth);

            transmitRxToQuene.data_length = strlen((char *)transmitRxToQuene.tx_buf);

            osMessageQueuePut(txQueueHandle, &transmitRxToQuene, 0, 0);
        }
    }
}

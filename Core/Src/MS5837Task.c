#include "MS5837Task.h"
#include "MS5837.h"
#include "cmd.h"
#include <string.h>
#include <stdlib.h>
#include "cJson.h"
#include "cJSON_Utils.h"

extern osSemaphoreId_t ms5837BinarySemHandle;

void Ms5837Task(void *argument)
{
    cJSON *root;
    MS5837_data data;

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "data", "ms5837");
    cJSON_AddNumberToObject(root, "temperature", 0.0);
    cJSON_AddNumberToObject(root, "pressure", 0.0);
    cJSON_AddNumberToObject(root, "depth", 0.0);

    MS5837_init();
    while(1)
    {
        if (osSemaphoreAcquire(ms5837BinarySemHandle, osWaitForever) == osOK){
            data = MS5837_Getdata();
					
            cJSON_ReplaceItemInObject(root, "temperature", cJSON_CreateNumber(data.temperture));
            cJSON_ReplaceItemInObject(root, "pressure", cJSON_CreateNumber(data.pressure));
            cJSON_ReplaceItemInObject(root, "depth", cJSON_CreateNumber(data.depth));
            char *return_str = cJSON_Print(root);

            usart3Printf(return_str, strlen(return_str));
            // HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);

            free(return_str); // 释放内存
        }

        osDelay(50);
    }
}

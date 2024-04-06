#include "cmd.h"
#include "cmsis_os.h"
#include "stm32h7xx_hal_dma.h"
#include <string.h>
#include <stdlib.h>
#include "tim.h"

extern osSemaphoreId_t cmdRxBinarySemHandle;
extern osSemaphoreId_t ms5837BinarySemHandle;
extern osMutexId_t usart3MutexHandle;
extern uint8_t ms5837ScanMode;

unsigned char rx_buf[RX_BUF_LEN];
unsigned char tx_buf[TX_BUF_LEN];

cJSON *error_root;

void CmdTask(void *argument)
{

    callback_t callbacks[] = {
        {"sum", sum},
        {"ms5837", getMs5837Data},
    };

    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, RX_BUF_LEN);

    uint8_t data_length;

    error_root = cJSON_CreateObject();
    cJSON_AddStringToObject(error_root, "error_type", "null");

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
        osSemaphoreAcquire(cmdRxBinarySemHandle, osWaitForever); // 当有rx中断时

        data_length = RX_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);

        cJSON *root = cJSON_ParseWithLength((char *)rx_buf, (size_t)data_length); // 解析接收信息

        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, RX_BUF_LEN); // rx空闲中断打开
        // huart3.RxState = HAL_UART_STATE_READY;
        // __HAL_UNLOCK(&huart3);


        if (root == NULL)
        { // 解析失败

            cJSON_ReplaceItemInObject(error_root, "error_type", cJSON_CreateString("prase error"));
            char *error_str = cJSON_Print(error_root);

            usart3Printf(error_str, strlen(error_str));

            free(error_str); // 释放内存
        }
        else
        { // 解析成功
            cJSON *cmdItem = cJSON_GetObjectItem(root, "cmd");
            cJSON *argvItem = cJSON_GetObjectItem(root, "argv");

            if ((cmdItem != NULL) && (argvItem != NULL))
            {
                for (uint8_t i = 0; i < sizeof(callbacks) / sizeof(callbacks[0]); i++)
                {
                    if (!strcmp(callbacks[i].name, cJSON_GetStringValue(cmdItem)))
                    {
                        callbacks[i].fn(argvItem);
                    }
                }
            }
            else // 指令错误
            {
                cJSON_ReplaceItemInObject(error_root, "error_type", cJSON_CreateString("cmd error"));
                char *error_str = cJSON_Print(error_root);
                usart3Printf(error_str, strlen(error_str));
                free(error_str); // 释放内存
            }
        }

        cJSON_Delete(root); // 释放内存
    }
}

void usart3Printf(char *temp_buf, uint16_t len)
{
    osMutexAcquire(usart3MutexHandle, osWaitForever); // 获取usart3互斥锁
    memcpy(tx_buf, temp_buf, len + 2);
    tx_buf[len] = '\r';
    tx_buf[len + 1] = '\n';

    HAL_UART_Transmit_DMA(&huart3, (unsigned char *)tx_buf, len + 2);
    while (!__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC)) // 等待发送完毕
    {
        osDelay(5);
    }

    osMutexRelease(usart3MutexHandle); // 释放usart3互斥锁
}

void sum(cJSON *root)
{
    double sum;

    cJSON *aItem = cJSON_GetObjectItem(root, "a");
    cJSON *bItem = cJSON_GetObjectItem(root, "b");
    if ((aItem != NULL && cJSON_IsNumber(aItem)) && (bItem != NULL && cJSON_IsNumber(bItem)))
    {
        sum = cJSON_GetNumberValue(aItem) + cJSON_GetNumberValue(bItem);

        cJSON *return_root = cJSON_CreateObject();
        cJSON_AddStringToObject(return_root, "return", "sum");
        cJSON_AddNumberToObject(return_root, "result", sum);
        char *return_str = cJSON_Print(return_root);

        usart3Printf(return_str, strlen(return_str));

        free(return_str); // 释放内存
        cJSON_Delete(return_root);
    }
    else
    {
        cJSON_ReplaceItemInObject(error_root, "error_type", cJSON_CreateString("argv error"));
        char *error_str = cJSON_Print(error_root);

        usart3Printf(error_str, strlen(error_str));
        free(error_str); // 释放内存
    }
}

void getMs5837Data(cJSON *root)
{
    cJSON *modeItem = cJSON_GetObjectItem(root, "mode");
    if ((modeItem != NULL && cJSON_IsString(modeItem)))
    {
        if (!strcmp(cJSON_GetStringValue(modeItem), "scanoff"))
        {
            ms5837ScanMode = 0;
        }
        else if (!strcmp(cJSON_GetStringValue(modeItem), "scanon"))
        {
            ms5837ScanMode = 1;
            osSemaphoreRelease(ms5837BinarySemHandle);
        }
        else if (!strcmp(cJSON_GetStringValue(modeItem), "once"))
            osSemaphoreRelease(ms5837BinarySemHandle);
        else
        {
            cJSON_ReplaceItemInObject(error_root, "error_type", cJSON_CreateString("no such mode"));
            char *error_str = cJSON_Print(error_root);

            usart3Printf(error_str, strlen(error_str));
            free(error_str); // 释放内存
        }
    }
}

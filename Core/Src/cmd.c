#include "cmd.h"
#include "cmsis_os.h"
#include "stm32h7xx_hal_dma.h"
#include <string.h>

extern osSemaphoreId_t cmdRxBinarySemHandle;
extern osMutexId_t usart3MutexHandle;

unsigned char rx_buf[RX_BUF_LEN];

void CmdTask(void *argument){
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, RX_BUF_LEN);
    __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
    HAL_UART_Receive_DMA(&huart3, rx_buf, RX_BUF_LEN);
    uint8_t data_length;

    while (osSemaphoreAcquire(cmdRxBinarySemHandle, osWaitForever) == osOK) // 当有rx中断时
    {
        data_length = RX_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
        usart3Printf(rx_buf, data_length); // 发送接收到的消息
        memset(rx_buf, 0, data_length);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, sizeof(rx_buf)); // rx空闲中断打开
        __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
    }
}

void usart3Printf(unsigned char *tx_buf,uint16_t len)
{
    if (osMutexAcquire(usart3MutexHandle, osWaitForever) == osOK) // 获取usart3互斥锁
    {
        HAL_UART_Transmit_DMA(&huart3, tx_buf, len);
        while (HAL_UART_GetState(&huart3) == HAL_UART_STATE_BUSY_TX) // 等待发送完毕
        {
            osDelay(5);
        }

        osMutexRelease(usart3MutexHandle); // 释放usart3互斥锁
    }
}

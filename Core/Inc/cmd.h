#include "usart.h"
#include <stdarg.h>
#include <stdio.h>
#include "cJson.h"
#include "cJSON_Utils.h"

#define RX_BUF_LEN 200
#define TX_BUF_LEN 200

typedef void (*callback)(cJSON *root);

typedef struct
{
    unsigned char rx_buf[RX_BUF_LEN];
    uint8_t data_length;
} rxStruct;

typedef struct
{
    unsigned char tx_buf[TX_BUF_LEN];
    uint8_t data_length;
} txStruct;

typedef struct
{
    const char *name;
    callback fn;
} callback_t;

void CmdTask(void *argument);
void TxTask(void *argument);

void sum(cJSON *root);
void getMs5837Data(cJSON *root);

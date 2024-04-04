#include "usart.h"
#include <stdarg.h>
#include <stdio.h>
#include "cJson.h"
#include "cJSON_Utils.h"

#define RX_BUF_LEN 128

typedef void (*callback)(cJSON *root);

typedef struct
{
    const char *name;
    callback fn;
} callback_t;

void CmdTask(void *argument);
void usart3Printf(char *format, uint16_t len);

void sum(cJSON *root);

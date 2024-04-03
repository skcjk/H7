#include "usart.h"
#include <stdarg.h>
#include <stdio.h>

#define RX_BUF_LEN 128

void CmdTask(void *argument);
void usart3Printf(unsigned char *format,uint16_t len);

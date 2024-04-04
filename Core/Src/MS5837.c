#include "MS5837.h"

uint16_t Cal_C[7]; // 用于存放PROM中的6组数据C1-C6
/*******************************************************************************
 * @函数名称	MS583730BA_RESET
 * @函数说明   复位MS5611
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *******************************************************************************/
void MS583703BA_RESET(void)
{
#if USE_FREE_RTOS
    taskENTER_CRITICAL();
#endif
    IIC_Start();
    IIC_Send_Byte(0xEC); // CSB接地，主机地址：0XEE，否则 0X77
    IIC_Wait_Ack();
    IIC_Send_Byte(0x1E); // 发送复位命令
    IIC_Wait_Ack();
    IIC_Stop();
#if USE_FREE_RTOS
    taskEXIT_CRITICAL();
#endif
}

/*******************************************************************************
 * @函数名称	MS5611_init
 * @函数说明   初始化5611
 * @输入参数  	无
 * @输出参数   无
 * @返回参数   无
 *******************************************************************************/
void MS5837_init(void)
{
    uint8_t inth, intl;
    uint8_t i;
    // float air_temp, air_press, air_depth;

    IIC_Init();
    MS583703BA_RESET(); // Reset Device  复位MS5837
#if USE_FREE_RTOS
    osDelay(40);
#else
    delay_us(40000); // 复位后延时（注意这个延时是一定必要的，可以缩短但似乎不能少于20ms）
#endif

    for (i = 1; i <= 6; i++)
    {
#if USE_FREE_RTOS
        taskENTER_CRITICAL();
#endif
        IIC_Start();
        IIC_Send_Byte(0xEC);
        IIC_Wait_Ack();
        IIC_Send_Byte(0xA0 + (i * 2));
        IIC_Wait_Ack();
        IIC_Stop();
        delay_us(5);
        IIC_Start();
        IIC_Send_Byte(0xEC + 0x01); // 进入接收模式
        delay_us(1);
        IIC_Wait_Ack();
        inth = IIC_Read_Byte(1); // 带ACK的读数据
        delay_us(1);
        intl = IIC_Read_Byte(0); // 最后一个字节NACK
        IIC_Stop();
#if USE_FREE_RTOS
        taskEXIT_CRITICAL();
#endif
        Cal_C[i] = (((uint16_t)inth << 8) | intl);
    }

    // for (i = 0; i < 5; i++)
    // {
    // 	delay_ms(1);
    // 	MS5837_Getdata(&air_temp, &air_press, &air_depth); //获取大气压
    // 	Atmdsphere_Pressure += air_press;
    // 	//printf("%d\t", Pressure); //串口输出原始数据
    // }
    // Atmdsphere_Pressure = Atmdsphere_Pressure / 5.0f;
    // printf("Atmdsphere_Pressure:%d\r\n", Atmdsphere_Pressure); //串口输出原始数据
}

/**************************实现函数********************************************
 *函数原型:unsigned long MS561101BA_getConversion(void)
 *功　　能:    读取 MS5837 的转换结果
 *******************************************************************************/
uint32_t MS583703BA_getConversion(uint8_t command)
{

    uint32_t conversion = 0;
    uint8_t temp[3];

#if USE_FREE_RTOS
    taskENTER_CRITICAL();
#endif
    IIC_Start();
    IIC_Send_Byte(0xEC); // 写地址
    IIC_Wait_Ack();
    IIC_Send_Byte(command); // 写转换命令
    IIC_Wait_Ack();
    IIC_Stop();
#if USE_FREE_RTOS
    taskEXIT_CRITICAL();
#endif

#if USE_FREE_RTOS
    osDelay(10);
#else
    delay_us(10000); // 等待AD转换完成
#endif

#if USE_FREE_RTOS
    taskENTER_CRITICAL(); //进入临界区，关中断
#endif
    IIC_Start();
    IIC_Send_Byte(0xEC); // 写地址
    IIC_Wait_Ack();
    IIC_Send_Byte(0); // start read sequence
    IIC_Wait_Ack();
    IIC_Stop();

    IIC_Start();
    IIC_Send_Byte(0xEC + 0x01); // 进入接收模式
    IIC_Wait_Ack();
    temp[0] = IIC_Read_Byte(1); // 带ACK的读数据  bit 23-16
    temp[1] = IIC_Read_Byte(1); // 带ACK的读数据  bit 8-15
    temp[2] = IIC_Read_Byte(0); // 带NACK的读数据 bit 0-7
    IIC_Stop();
#if USE_FREE_RTOS
    taskEXIT_CRITICAL(); ////推出临界区，开中断
#endif

    conversion = ((uint32_t)temp[0] << 16) | ((uint32_t)temp[1] << 8) | temp[2];

    return conversion;
}

///***********************************************
//  * @brief  读取气压
//  * @param  None
//  * @retval None
//************************************************/
MS5837_data MS5837_Getdata(void)
{
    MS5837_data data;

    uint16_t senst1;   // C1压力灵敏度
    uint16_t offt1;    // C2压力补偿值
    uint16_t tcs;      // C3压力灵敏度温度系数
    uint16_t tco;      // C4压力补偿温度系数
    uint16_t tref;     // C5参考温度
    uint16_t tempsens; // C6温度传感器温度系数

    senst1 = Cal_C[1];
    offt1 = Cal_C[2];
    tcs = Cal_C[3];
    tco = Cal_C[4];
    tref = Cal_C[5];
    tempsens = Cal_C[6];

    uint32_t digitalPressureValue;
    uint32_t digitalTemperatureValue;

    /*读取压力数据*/

    digitalPressureValue = MS583703BA_getConversion(0x48);
#if USE_FREE_RTOS
    osDelay(40);
#else
    delay_us(40000); // 等待AD转换完成
#endif

    /*读取温度数据*/
    digitalTemperatureValue = MS583703BA_getConversion(0x58);
    /*对温度进行一阶修正*/

    int32_t dT;
    int32_t temp;
    dT = digitalTemperatureValue - (uint32_t)tref * 256l;
    temp = (int32_t)(2000l + dT * (uint32_t)tempsens / 8388608LL);

    /*对压力进行一阶修正*/
    int64_t off;
    int64_t sens;
    int32_t pres;

    off = (int64_t)offt1 * 65536l + ((int64_t)tco * (int64_t)dT) / 128l;
    sens = (int64_t)senst1 * 32768l + ((int64_t)tcs * (int64_t)dT) / 256l;
    pres = (int32_t)(((int64_t)digitalPressureValue * sens / 2097152l - off) / 8192l);

    /*对温度和压力进行二阶修正*/

    int64_t ti = 0;
    int64_t offi = 0;
    int64_t sensi = 0;
    int64_t off2 = 0;
    int64_t sens2 = 0;

    if (temp < 2000)
    {
        ti = (3 * (int64_t)dT * (int64_t)dT / 8589934592LL);
        offi = (3 * ((int64_t)temp - 2000l) * ((int64_t)temp - 2000l) / 2);
        sensi = (5 * ((int64_t)temp - 2000l) * ((int64_t)temp - 2000l) / 8);

        if(temp < -1500){
			offi = offi + 7 * (temp + 1500l) * (temp + 1500l);
			sensi = sensi + 4 * (temp + 1500l) * (temp + 1500l);
		}
    }
    else{
		ti = (2*(int64_t)dT*(int64_t)dT)/137438953472LL;
		offi = (((int64_t)temp-2000l)*((int64_t)temp-2000l))/16;
		sensi = 0;
	}

    off2 = off-offi;
	sens2 = sens-sensi;

    temp = temp - ti;
    pres = ((digitalPressureValue * sens2) / 2097152l - off2)/ 8192l;

    data.temperture = (float)temp / 100.0f;
    data.pressure = (float)pres / 10.0f;
    data.depth = ((data.pressure*100)-101300)/(DENSITY*9.80665);

    return data;
}

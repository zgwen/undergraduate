#include "adx345.h"
#include "iic.h"
#include <stdlib.h>
#include "math.h"

#define ADX345_DelayMs(x)     {delay_ms(x);}  //延时函数

/****************************************************************************
* Function Name  : ADX345_WriteReg
* Description    : 设置ADX345寄存器
* Input          : addr：寄存器地址
*                * dat：吸入数据
* Output         : None
* Return         : None
****************************************************************************/

static int8_t ADX345_WriteReg(uint8_t addr, uint8_t dat)
{
	I2C_Start();
	I2C_Send_Byte(ADX345_ADDR); //24C02写地址
    if(I2C_Wait_Ack())          //如果无应答，表示发送失败
    {
        return 0xFF;
    }
    I2C_Send_Byte(addr); 
    if(I2C_Wait_Ack())         //如果无应答，表示发送失败
    {
        return 0xFF;
    }
    I2C_Send_Byte(dat); 
    if(I2C_Wait_Ack())
    {
        return 0xFF;
    }
    
    I2C_Stop();
    return 0;   
}

/****************************************************************************
* Function Name  : ADX345_ReadReg
* Description    : 读取ADX345寄存器
* Input          : addr：读取地址
* Output         : None
* Return         : 读取到的8位数据
****************************************************************************/

static uint8_t ADX345_ReadReg(uint8_t addr)
{
    uint8_t readValue = 0xFF;

    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR);    //24C02写地址
    if(I2C_Wait_Ack())
    {
        return readValue;
    }
    I2C_Send_Byte(addr); 
    if(I2C_Wait_Ack())
    {
        return readValue;
    }

    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR + 1); //24C02读地址
    if(I2C_Wait_Ack())
    {
        return readValue;       
    }
    readValue = I2C_Read_Byte(0);
    I2C_Stop();

    return readValue;
}

/****************************************************************************
* Function Name  : ADX345_ReadXYZ
* Description    : 读取X,Y,Z的AD值
* Input          : xValue：X轴的保存地址
*                * yValue：Y轴的保存地址
*                * zValue：Z轴的保存地址
* Output         : None
* Return         : 0：读取成功。0xFF：读取失败
****************************************************************************/

static int8_t ADX345_ReadXYZ(int16_t *xValue, int16_t *yValue, int16_t *zValue)
{
    uint8_t readValue[6], i;
    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR);    //24C02写地址
    if(I2C_Wait_Ack())
    {
        return 0xFF;
    }
    I2C_Send_Byte(0x32);           //发送读地址（X轴首地址）
    if(I2C_Wait_Ack())
    {
        return 0xFF;
    }

    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR + 1); //24C02读地址
    if(I2C_Wait_Ack())
    {
        return 0xFF;       
    }

    /* 读取六个字节数据 */
    for(i=0; i<6; i++)
    {
        
        if(i == 5)        //接收最后一个字节时，发送NACK
        {
            readValue[i] = I2C_Read_Byte(0);
        }
        else             //发送接收后应答
        {
            readValue[i] = I2C_Read_Byte(1); 
        }
    }
    I2C_Stop();
    /* 处理读取到的数据 */
	*xValue = (uint16_t)(readValue[1] << 8) + readValue[0]; 	    
	*yValue = (uint16_t)(readValue[3] << 8) + readValue[2]; 	    
	*zValue = (uint16_t)(readValue[5] << 8) + readValue[4]; 	   

   return 0;
}

/****************************************************************************
* Function Name  : ADX345_Init
* Description    : 初始化ADX345，并核对ADX的ID
* Input          : None
* Output         : None
* Return         : 0：成功。0xFF：失败
****************************************************************************/

int ADX345_Init(void)
{
    I2C_INIT();
    if(ADX345_ReadReg(ADX_DEVID) == 0xE5)
    {
        ADX345_WriteReg(ADX_DATA_FORMAT,0x2B);//13位全分辨率,输出数据右对齐,16g量程 
		ADX345_WriteReg(ADX_BW_RATE,0x0A);	  //数据输出速度为100Hz
		ADX345_WriteReg(ADX_POWER_CTL,0x28);  //链接使能,测量模式
		ADX345_WriteReg(ADX_INT_ENABLE,0x00); //不使用中断		 
	 	ADX345_WriteReg(ADX_OFSX,0x00);       //敲击阀值
		ADX345_WriteReg(ADX_OFSY,0x00);       //X轴偏移
		ADX345_WriteReg(ADX_OFSZ,0x00);       //Y轴偏移
        return 0;
    }
   
    return 0xFF; //返回初始化失败
}

/****************************************************************************
* Function Name  : ADX345_Adjust
* Description    : ADX345进行校正。
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void ADX345_Adjust(void)
{
    int32_t offx = 0, offy = 0, offz = 0;
    int16_t xValue, yValue, zValue;
    uint8_t i;

	ADX345_WriteReg(ADX_POWER_CTL, 0x00);	 //先进入休眠模式.
	ADX345_DelayMs(100);
	ADX345_Init(); 
    ADX345_DelayMs(20);
    
    /* 读取十次数值 */
    for(i=0; i<10; i++)
    {
        ADX345_ReadXYZ(&xValue, &yValue, &zValue);
        offx += xValue;
        offy += yValue;
        offz += zValue;
        ADX345_DelayMs(10);   //才样频率在100HZ，10ms采样一次最好         
    }
    
    /* 求出平均值 */
    offx /= 10;
    offy /= 10;
    offz /= 10;
    
    /* 全分辨率下，每个输出LSB为3.9 mg或偏移寄存器LSB的四分之一，所以除以4 */
    xValue = -(offx / 4);
	yValue = -(offy / 4);
	zValue = -((offz - 256) / 4);
    
    /* 设置偏移量 */
	ADX345_WriteReg(ADX_OFSX, xValue);
	ADX345_WriteReg(ADX_OFSY, yValue);
	ADX345_WriteReg(ADX_OFSZ, zValue); 
            
}

/****************************************************************************
* Function Name  : ADX_GetXYZData
* Description    : 读取ADX的XYZ轴的值（进行过数据处理）
* Input          : xValue：X轴的保存地址
*                * yValue：Y轴的保存地址
*                * zValue：Z轴的保存地址
* Output         : None
* Return         : None
****************************************************************************/

void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue)
{
    int32_t xTotal = 0, yTotal = 0, zTotal = 0;
    uint8_t i;
    
    *xValue = 0;
    *yValue = 0;
    *zValue = 0;

    /* 读取十次采样值 */
    for(i=0; i<10; i++)
    {
        ADX345_ReadXYZ(xValue, yValue, zValue);
        xTotal += *xValue;
        yTotal += *yValue;
        zTotal += *zValue;
        ADX345_DelayMs(10);  //才样频率在100HZ，10ms采样一次最好 
    }
    
    /* 求出平均值 */
    *xValue = xTotal / 10;
    *yValue = yTotal / 10;
    *zValue = zTotal / 10;       
}

/****************************************************************************
* Function Name  : ADX_GetAngle
* Description    : 将AD值转换成角度值
* Input          : xValue：x的值
*                * yValue：y的值
*                * zValue：z的值
*                * dir：0：与Z轴的角度;1：与X轴的角度;2：与Y轴的角度.
* Output         : None
* Return         : None
****************************************************************************/

int16_t ADX_GetAngle(float xValue, float yValue, float zValue, uint8_t dir)
{
	float temp;
 	float res = 0;

	switch(dir)
	{   
        /* 与自然Z轴的角度 */
		case 0:
 			temp = sqrt((xValue * xValue + yValue * yValue)) / zValue;
 			res = atan(temp);
 			break;
        
        /* 与自然X轴的角度 */
		case 1:
 			temp = xValue / sqrt((yValue * yValue + zValue * zValue));
 			res = atan(temp);
 			break;

        /* 与自然Y轴的角度 */
 		case 2:
 			temp = yValue / sqrt((xValue * xValue + zValue * zValue));
 			res = atan(temp);
 			break;

        default:
            break;
 	}

    res = res * 1800 / 3.14; 

	return res;
}
void ADX345_pros()
{
	int16_t value;
    uint8_t num = 6;
	int16_t Xval, Yval, Zval, Xang, Yang, Zang;
	uint8_t ShowData[6] = {0, 0, 0, 0, 0, 0};
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	while(ADX345_Init())
    {
        GUI_Show12Char(60, 42, "ADX345 ERROR！", RED, BLACK);
        delay_ms(100);        
    }
	while(1)
	{
		if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>300&&TouchData.lcdy<355)
		{		

		    GUI_Show12Char(10, 10, "ADX345加速度传感器实验", RED, BLACK);
		    GUI_Show12Char(0, 84, "X VAL:", RED, BLACK);
		    GUI_Show12Char(0, 105, "Y VAL:", RED, BLACK);
		    GUI_Show12Char(0, 126, "Z VAL:", RED, BLACK);
		    GUI_Show12Char(0, 147, "X ANG:", RED, BLACK);
		    GUI_Show12Char(0, 168, "Y ANG:", RED, BLACK);
		    GUI_Show12Char(0, 189, "Z ANG:", RED, BLACK);
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);		
		}
			
		ADX345_Adjust();

		/* 读取X,Y,Z的加速度值 */
        ADX_GetXYZData(&Xval, &Yval, &Zval);

        /* 将读取到的加速度值转换为角度值 */
        Xang=ADX_GetAngle(Xval, Yval, Zval,1);    
		Yang=ADX_GetAngle(Xval, Yval, Zval,2);   
		Zang=ADX_GetAngle(Xval, Yval, Zval,0);

		while(num)
	    {
	        switch(num)
	        {
	            case(1):
	                value = Zang;
	                break;
	            case(2):
	                value = Yang;
	                break;
	            case(3):
	                value = Xang;
	                break;
	            case(4):
	                value = Zval;
	                break;
	            case(5):
	                value = Yval;
	                break;
	            case(6):
	                value = Xval;
	                break;
	            default:
	               break;
	        }
			TOUCH_Scan();
			if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
			{
				GUI_DisplayInit();
				break;					
			}
	        if(value > 0)
	        {
	            ShowData[0] = '+';
	        }
	        else
	        {
	            ShowData[0] = '-';
	            value = abs(value);//求出绝对值
	        }
	        ShowData[1] = (value % 10000 /1000) + '0';
	        ShowData[2] = (value % 1000 /100) + '0';
	        ShowData[3] = (value % 100 /10) + '0';
	        ShowData[4] = (value % 10) + '0';
	        
	        GUI_Show12Char(48, (210 - num*21), ShowData, RED, BLACK);
	        num--;
	    }
		num=6;
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}	
}


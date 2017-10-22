#include "adx345.h"
#include "iic.h"
#include <stdlib.h>
#include "math.h"

#define ADX345_DelayMs(x)     {delay_ms(x);}  //��ʱ����

/****************************************************************************
* Function Name  : ADX345_WriteReg
* Description    : ����ADX345�Ĵ���
* Input          : addr���Ĵ�����ַ
*                * dat����������
* Output         : None
* Return         : None
****************************************************************************/

static int8_t ADX345_WriteReg(uint8_t addr, uint8_t dat)
{
	I2C_Start();
	I2C_Send_Byte(ADX345_ADDR); //24C02д��ַ
    if(I2C_Wait_Ack())          //�����Ӧ�𣬱�ʾ����ʧ��
    {
        return 0xFF;
    }
    I2C_Send_Byte(addr); 
    if(I2C_Wait_Ack())         //�����Ӧ�𣬱�ʾ����ʧ��
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
* Description    : ��ȡADX345�Ĵ���
* Input          : addr����ȡ��ַ
* Output         : None
* Return         : ��ȡ����8λ����
****************************************************************************/

static uint8_t ADX345_ReadReg(uint8_t addr)
{
    uint8_t readValue = 0xFF;

    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR);    //24C02д��ַ
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
	I2C_Send_Byte(ADX345_ADDR + 1); //24C02����ַ
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
* Description    : ��ȡX,Y,Z��ADֵ
* Input          : xValue��X��ı����ַ
*                * yValue��Y��ı����ַ
*                * zValue��Z��ı����ַ
* Output         : None
* Return         : 0����ȡ�ɹ���0xFF����ȡʧ��
****************************************************************************/

static int8_t ADX345_ReadXYZ(int16_t *xValue, int16_t *yValue, int16_t *zValue)
{
    uint8_t readValue[6], i;
    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR);    //24C02д��ַ
    if(I2C_Wait_Ack())
    {
        return 0xFF;
    }
    I2C_Send_Byte(0x32);           //���Ͷ���ַ��X���׵�ַ��
    if(I2C_Wait_Ack())
    {
        return 0xFF;
    }

    I2C_Start();
	I2C_Send_Byte(ADX345_ADDR + 1); //24C02����ַ
    if(I2C_Wait_Ack())
    {
        return 0xFF;       
    }

    /* ��ȡ�����ֽ����� */
    for(i=0; i<6; i++)
    {
        
        if(i == 5)        //�������һ���ֽ�ʱ������NACK
        {
            readValue[i] = I2C_Read_Byte(0);
        }
        else             //���ͽ��պ�Ӧ��
        {
            readValue[i] = I2C_Read_Byte(1); 
        }
    }
    I2C_Stop();
    /* �����ȡ�������� */
	*xValue = (uint16_t)(readValue[1] << 8) + readValue[0]; 	    
	*yValue = (uint16_t)(readValue[3] << 8) + readValue[2]; 	    
	*zValue = (uint16_t)(readValue[5] << 8) + readValue[4]; 	   

   return 0;
}

/****************************************************************************
* Function Name  : ADX345_Init
* Description    : ��ʼ��ADX345�����˶�ADX��ID
* Input          : None
* Output         : None
* Return         : 0���ɹ���0xFF��ʧ��
****************************************************************************/

int ADX345_Init(void)
{
    I2C_INIT();
    if(ADX345_ReadReg(ADX_DEVID) == 0xE5)
    {
        ADX345_WriteReg(ADX_DATA_FORMAT,0x2B);//13λȫ�ֱ���,��������Ҷ���,16g���� 
		ADX345_WriteReg(ADX_BW_RATE,0x0A);	  //��������ٶ�Ϊ100Hz
		ADX345_WriteReg(ADX_POWER_CTL,0x28);  //����ʹ��,����ģʽ
		ADX345_WriteReg(ADX_INT_ENABLE,0x00); //��ʹ���ж�		 
	 	ADX345_WriteReg(ADX_OFSX,0x00);       //�û���ֵ
		ADX345_WriteReg(ADX_OFSY,0x00);       //X��ƫ��
		ADX345_WriteReg(ADX_OFSZ,0x00);       //Y��ƫ��
        return 0;
    }
   
    return 0xFF; //���س�ʼ��ʧ��
}

/****************************************************************************
* Function Name  : ADX345_Adjust
* Description    : ADX345����У����
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void ADX345_Adjust(void)
{
    int32_t offx = 0, offy = 0, offz = 0;
    int16_t xValue, yValue, zValue;
    uint8_t i;

	ADX345_WriteReg(ADX_POWER_CTL, 0x00);	 //�Ƚ�������ģʽ.
	ADX345_DelayMs(100);
	ADX345_Init(); 
    ADX345_DelayMs(20);
    
    /* ��ȡʮ����ֵ */
    for(i=0; i<10; i++)
    {
        ADX345_ReadXYZ(&xValue, &yValue, &zValue);
        offx += xValue;
        offy += yValue;
        offz += zValue;
        ADX345_DelayMs(10);   //����Ƶ����100HZ��10ms����һ�����         
    }
    
    /* ���ƽ��ֵ */
    offx /= 10;
    offy /= 10;
    offz /= 10;
    
    /* ȫ�ֱ����£�ÿ�����LSBΪ3.9 mg��ƫ�ƼĴ���LSB���ķ�֮һ�����Գ���4 */
    xValue = -(offx / 4);
	yValue = -(offy / 4);
	zValue = -((offz - 256) / 4);
    
    /* ����ƫ���� */
	ADX345_WriteReg(ADX_OFSX, xValue);
	ADX345_WriteReg(ADX_OFSY, yValue);
	ADX345_WriteReg(ADX_OFSZ, zValue); 
            
}

/****************************************************************************
* Function Name  : ADX_GetXYZData
* Description    : ��ȡADX��XYZ���ֵ�����й����ݴ���
* Input          : xValue��X��ı����ַ
*                * yValue��Y��ı����ַ
*                * zValue��Z��ı����ַ
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

    /* ��ȡʮ�β���ֵ */
    for(i=0; i<10; i++)
    {
        ADX345_ReadXYZ(xValue, yValue, zValue);
        xTotal += *xValue;
        yTotal += *yValue;
        zTotal += *zValue;
        ADX345_DelayMs(10);  //����Ƶ����100HZ��10ms����һ����� 
    }
    
    /* ���ƽ��ֵ */
    *xValue = xTotal / 10;
    *yValue = yTotal / 10;
    *zValue = zTotal / 10;       
}

/****************************************************************************
* Function Name  : ADX_GetAngle
* Description    : ��ADֵת���ɽǶ�ֵ
* Input          : xValue��x��ֵ
*                * yValue��y��ֵ
*                * zValue��z��ֵ
*                * dir��0����Z��ĽǶ�;1����X��ĽǶ�;2����Y��ĽǶ�.
* Output         : None
* Return         : None
****************************************************************************/

int16_t ADX_GetAngle(float xValue, float yValue, float zValue, uint8_t dir)
{
	float temp;
 	float res = 0;

	switch(dir)
	{   
        /* ����ȻZ��ĽǶ� */
		case 0:
 			temp = sqrt((xValue * xValue + yValue * yValue)) / zValue;
 			res = atan(temp);
 			break;
        
        /* ����ȻX��ĽǶ� */
		case 1:
 			temp = xValue / sqrt((yValue * yValue + zValue * zValue));
 			res = atan(temp);
 			break;

        /* ����ȻY��ĽǶ� */
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
        GUI_Show12Char(60, 42, "ADX345 ERROR��", RED, BLACK);
        delay_ms(100);        
    }
	while(1)
	{
		if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>300&&TouchData.lcdy<355)
		{		

		    GUI_Show12Char(10, 10, "ADX345���ٶȴ�����ʵ��", RED, BLACK);
		    GUI_Show12Char(0, 84, "X VAL:", RED, BLACK);
		    GUI_Show12Char(0, 105, "Y VAL:", RED, BLACK);
		    GUI_Show12Char(0, 126, "Z VAL:", RED, BLACK);
		    GUI_Show12Char(0, 147, "X ANG:", RED, BLACK);
		    GUI_Show12Char(0, 168, "Y ANG:", RED, BLACK);
		    GUI_Show12Char(0, 189, "Z ANG:", RED, BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);		
		}
			
		ADX345_Adjust();

		/* ��ȡX,Y,Z�ļ��ٶ�ֵ */
        ADX_GetXYZData(&Xval, &Yval, &Zval);

        /* ����ȡ���ļ��ٶ�ֵת��Ϊ�Ƕ�ֵ */
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
	            value = abs(value);//�������ֵ
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


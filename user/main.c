#include "public.h"
#include "led.h"
#include "beep.h"
#include "smg.h"
#include "adc.h"
#include "key.h"
#include "ds18b20.h"
#include "AT24CXX.h"
#include "hwjs.h"
#include "lowpower.h"

#include "sd.h"
#include "adx345.h"
#include "sram.h"
#include "dac.h"
#include "usart.h"
#include "rs485.h"
#include "can.h"


int main()			  
{	
	u8 start=0;
	
	TFT_Init();	 //TFT������ʼ��
	FLASH_Init();
	TOUCH_Init();
	GUI_DisplayInit();	
	key_init();
	SRAM_Config();		
	while(1)
	{
		if(TOUCH_Scan() == 0)
		{		
			start=1;	
		}
		
		if(start==1)
		{
			if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>35&&TouchData.lcdy<90)	
			{
				led_display();	//LED��˸	
			}
			if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>35&&TouchData.lcdy<90)	
			{
				sound2();  //�綯������	
			}
			if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>35&&TouchData.lcdy<90)	
			{
				static_smg_display();	//��̬�������ʾ	
			}
			if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>35&&TouchData.lcdy<90)	
			{
				adc_change();   //ADת��			
			}

			if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>125&&TouchData.lcdy<175)	
			{
				key_pros();   //����			
			}
			if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>125&&TouchData.lcdy<175)	
			{
				DS18B20_pros();   //DS18B20�¶ȴ�����			
			}
			if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>125&&TouchData.lcdy<175)	
			{
				IIC_pros();   //eeprom			
			}
			if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>125&&TouchData.lcdy<175)	
			{
				HW_pros();   //����ң��			
			}
		
			if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>215&&TouchData.lcdy<265)	
			{
				low_pros();   //�͹��� ����ģʽ			
			}
			if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>215&&TouchData.lcdy<265)	
			{
				flash_pros();   //FLASH-EN25Q64			
			}
			if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>215&&TouchData.lcdy<265)	
			{
				SD_pros();   //SD Card����			
			}
			
			if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>300&&TouchData.lcdy<355)	
			{
				ADX345_pros();   //ADX345���ٶȴ���������			
			}
			if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>300&&TouchData.lcdy<355)	
			{
				SRAM_pros();   //SRAM����			
			}
			if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>300&&TouchData.lcdy<355)	
			{
				DAC_pros();   //DA����			
			}
			if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>300&&TouchData.lcdy<355)	
			{
				COM_pros();   //COM1ͨ�Ų���			
			}

			if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>410&&TouchData.lcdy<460)	
			{
				RS485_pros();   //485ͨ�Ų���			
			}
			if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>410&&TouchData.lcdy<460)	
			{
				CAN_pros();   //CANͨ�Ų���			
			}	
		}				
	}
}




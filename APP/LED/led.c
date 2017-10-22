#include "led.h"

void LED_Init()	  //端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=LED;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_Write(GPIOC,(u16)(0xff)); 	
}

void led_display()	//LED闪烁
{
	u8 i;
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	LED_Init();
	while(1)
	{	
		if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>35&&TouchData.lcdy<90)
		{				
			GUI_Show12Char(10,10,"LED灯实验",YELLOW,BLACK);	
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);	
		}
		if(i<=10)
		{
			GPIO_SetBits(GPIOC,LED);
		}
		if(i>=20&&i<40)
		{
			GPIO_ResetBits(GPIOC,LED);	
		}
		if(i>=40) i=0;		
		delay_ms(10);
		i++;				
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			GPIO_SetBits(GPIOC,LED);
			break;					
		}
		TOUCH_Scan();	
	}
	
}

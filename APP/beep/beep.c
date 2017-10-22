#include "beep.h"
void BEEP_Init()	  //端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin=BZ;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	
}

void sound2()  //电动车报警
{
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	BEEP_Init();
	while(1)
	{
		if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>35&&TouchData.lcdy<90)
		{		
			GUI_Show12Char(10,10,"蜂鸣器实验",YELLOW,BLACK);
		
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);
				
		}
		TOUCH_Scan();
		GPIO_SetBits(GPIOB,BZ);
		delay_ms(1);
		GPIO_ResetBits(GPIOB,BZ);
		delay_ms(1);	
		
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		
	}	
}


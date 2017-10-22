#include "public.h"

/*******************************************************************************
* 函 数 名         : iwdg_init
* 函数功能		   : 独立看门狗模式配置初始化	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void iwdg_init()	//独立看门狗初始化配置
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能寄存器，写功能
	IWDG_SetPrescaler(IWDG_Prescaler_64);//设置IWDG预分频值//40K/64=1.6ms
	IWDG_SetReload(800);//设置IWDG重装载值  12位的（0~4095）//800*1.6ms=1.28s
	IWDG_ReloadCounter();//按照IWDG重装载寄存器的值重装载IWDG计数器
	IWDG_Enable();//使能IWDG
}
void iwdg_pros()
{
	iwdg_init();   //独立看门狗初始化配置
	while(1)
	{
		if(TOUCH_Scan() == 0)
		{		
			TFT_ClearScreen(BLACK);	
			GUI_Show12Char(10,10,"独立看门狗实验",YELLOW,BLACK);
			GUI_Show12Char(200,380,"返回",YELLOW,BLACK);		
		}	
		IWDG_ReloadCounter();	   //喂狗的时间是1.28s
		GUI_Show12Char(10,100,"进入喂狗...",YELLOW,BLACK);
		delay_ms(10);	//只要在喂狗时间内喂狗就不会让系统以为死机进入复位状态
		if(TouchData.lcdx>200&&TouchData.lcdx<240&&TouchData.lcdy>380&&TouchData.lcdy<400)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}	
}

#include "smg.h"
u8 smgduan[16]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
             0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};//0~F 数码管段选数据


/*******************************************************************************
* 函 数 名         : smg_init
* 函数功能		   : 数码管端口初始化函数	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void smg_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;  //声明一个结构体变量，用来初始化GPIO
	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=smg_duan;	  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);		/* 初始化GPIO */
}

void static_smg_display()	//静态数码管显示
{	
	u8 i,j;
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	smg_init();
	while(1)
	{
		if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>35&&TouchData.lcdy<90)
		{		
			GUI_Show12Char(10,10,"数码管实验",YELLOW,BLACK);
			GUI_Show12Char(10,50,"数码管循环显示0-F",YELLOW,BLACK);
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);	
		}
		GPIO_Write(GPIOC,(u16)(~smgduan[i]));
		if(j>=30)
		{
			j=0;
			i++;
			if(i==16)i=0;
		}
		j++;
		delay_ms(10);
		TOUCH_Scan();
		
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			GPIO_SetBits(GPIOC,GPIO_Pin_All);
			break;					
		}			
	}
		
}

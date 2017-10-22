#include "hwjs.h"
u32 hw_jsm;
u8  hw_jsbz;
void hwjs_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;//红外接收
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOG,&GPIO_InitStructure);


	EXTI_ClearITPendingBit(EXTI_Line15);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource15); 
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure); 


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

}

void EXTI15_10_IRQHandler(void)	  //红外遥控外部中断
{
	u8 Tim=0,Ok=0,Data,Num=0;

   while(1)
   {
	   	if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==1)
		{
			 Tim=HW_jssj();//获得此次高电平时间

			 if(Tim>=250) break;//不是有用的信号

			 if(Tim>=200 && Tim<250)
			 {
			 	Ok=1;//收到起始信号
			 }
			 else if(Tim>=60 && Tim<90)
			 {
			 	Data=1;//收到数据 1
			 }
			 else if(Tim>=10 && Tim<50)
			 {
			 	Data=0;//收到数据 0
			 }

			 if(Ok==1)
			 {
			 	hw_jsm<<=1;
				hw_jsm+=Data;

				if(Num>=32)
				{
					hw_jsbz=1;
				  	break;
				}
			 }

			 Num++;
		}
   }

   EXTI_ClearITPendingBit(EXTI_Line15);	
}

u8 HW_jssj()
{
	u8 t=0;
	while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==1)//高电平
	{
		t++;
		delay_us(20);

		if(t>=250) return t;//超时溢出
	}

	return t;
}
void HW_pros()
{
	u8 dat[4],irdisp[9];
	u8 num[16]="0123456789ABCDEF";
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	hwjs_init();
	while(1)
	{
		if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>125&&TouchData.lcdy<175)
		{		
		
			GUI_Show12Char(10,10,"红外遥控实验",YELLOW,BLACK);
			GUI_Show12Char(10,30,"红外遥控键码：",YELLOW,BLACK);
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);		
		}
		if(hw_jsbz==1)	//如果红外接收到
		{
			hw_jsbz=0;	   //清零
			dat[0]=hw_jsm>>24;
			dat[1]=(hw_jsm>>16)&0xff;
			dat[2]=(hw_jsm>>8)&0xff;
			dat[3]=hw_jsm&0xff;
			hw_jsm=0;					//接收码清零
			irdisp[0]=num[dat[0]/16];//显示高位
			irdisp[1]=num[dat[0]%16];//显示低位  
			irdisp[2]=num[dat[1]/16];
			irdisp[3]=num[dat[1]%16];	 //同一个遥控器此2个字节的引导码数据是不会改变的，改变的只是数据码及反码
			irdisp[4]=num[dat[2]/16];
			irdisp[5]=num[dat[2]%16];
			irdisp[6]=num[dat[3]/16];
			irdisp[7]=num[dat[3]%16];
			irdisp[8]='\0';
			GUI_Show12Char(131,30,irdisp,YELLOW,BLACK);
		}
		
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)		
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();	
	}		
}

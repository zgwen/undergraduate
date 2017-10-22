#include "hwjs.h"
u32 hw_jsm;
u8  hw_jsbz;
void hwjs_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;//�������
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

void EXTI15_10_IRQHandler(void)	  //����ң���ⲿ�ж�
{
	u8 Tim=0,Ok=0,Data,Num=0;

   while(1)
   {
	   	if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==1)
		{
			 Tim=HW_jssj();//��ô˴θߵ�ƽʱ��

			 if(Tim>=250) break;//�������õ��ź�

			 if(Tim>=200 && Tim<250)
			 {
			 	Ok=1;//�յ���ʼ�ź�
			 }
			 else if(Tim>=60 && Tim<90)
			 {
			 	Data=1;//�յ����� 1
			 }
			 else if(Tim>=10 && Tim<50)
			 {
			 	Data=0;//�յ����� 0
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
	while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==1)//�ߵ�ƽ
	{
		t++;
		delay_us(20);

		if(t>=250) return t;//��ʱ���
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
		
			GUI_Show12Char(10,10,"����ң��ʵ��",YELLOW,BLACK);
			GUI_Show12Char(10,30,"����ң�ؼ��룺",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);		
		}
		if(hw_jsbz==1)	//���������յ�
		{
			hw_jsbz=0;	   //����
			dat[0]=hw_jsm>>24;
			dat[1]=(hw_jsm>>16)&0xff;
			dat[2]=(hw_jsm>>8)&0xff;
			dat[3]=hw_jsm&0xff;
			hw_jsm=0;					//����������
			irdisp[0]=num[dat[0]/16];//��ʾ��λ
			irdisp[1]=num[dat[0]%16];//��ʾ��λ  
			irdisp[2]=num[dat[1]/16];
			irdisp[3]=num[dat[1]%16];	 //ͬһ��ң������2���ֽڵ������������ǲ���ı�ģ��ı��ֻ�������뼰����
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

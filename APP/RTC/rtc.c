#include "public.h"
u8 timebz;
void rtc_init()
{
   	NVIC_InitTypeDef NVIC_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//�򿪵�Դʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);//�򿪴洢��ʱ��

	PWR_BackupAccessCmd(ENABLE);//ʹ�ܻ���ʧ��RTC�ͺ󱸼Ĵ�������
	BKP_DeInit();//������BKP��ȫ���Ĵ�������Ϊȱʡֵ
	RCC_LSEConfig(RCC_LSE_ON);//�����ⲿ���پ���LSE��
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//���ָ����RCC��־λ�������
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//����RTCʱ�ӣ�RTCCLK��
	RCC_RTCCLKCmd(ENABLE);//ʹ�ܻ���ʧ��RTCʱ��
	RTC_WaitForSynchro(); //�ȴ�RTC�Ĵ���ͬ��
	RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//ʹ�ܻ���ʧ��ָ����RTC�ж�
	RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_SetPrescaler(32767);//����Ԥ��Ƶ ʹ���ⲿ����Ϊ32.768K��Ҫ��1s�ж���Ԥ��Ƶ������Ϊ32767��ϵͳ���ڴ����ֻ����ϼ�1
	RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
		
}

void clockinit_RTC()
{
	if(BKP_ReadBackupRegister(BKP_DR1)!=0XA5A5)//��ָ���ĺ󱸼Ĵ����ж�������
	{
		//��һ������  ��ʼ������
		//RTC��ʼ��
		rtc_init();
		RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_SetCounter(0x9f92);//����RTC��������ֵ 11:20��50
		RTC_WaitForLastTask();
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
	}
	else
	{	
		RTC_WaitForSynchro();//�ȴ�RTC�Ĵ���ͬ��	
		RTC_WaitForLastTask();//�ȴ�дRTC�Ĵ������		
	    RTC_ITConfig(RTC_IT_SEC,ENABLE);//ʹ��RTC���ж�    
		RTC_WaitForLastTask();//�ȴ�дRTC�Ĵ������	
	}
	RCC_ClearFlag();//�����λ��־��		
}
void RTC_pros()
{
	u16 hour=0,min=0,sec=0;
	u32 timedata=0;
	u8 dat[9];
	rtc_init();
	clockinit_RTC();
	while(1)
	{
		if(TOUCH_Scan() == 0)
		{		
			TFT_ClearScreen(BLACK);	
			GUI_Show12Char(10,10,"RTCʱ��ʵ��",YELLOW,BLACK);
			GUI_Show12Char(200,380,"����",YELLOW,BLACK);	
		}
		
		if(timebz==1)
		{
			timebz=0;
			timedata=RTC_GetCounter();//��ȡRTC��������ֵ
			hour=timedata/3600;
			min=(timedata%3600)/60;
			sec=timedata%60;
			dat[0]=hour/10+'0';
			dat[1]=hour%10+'0';
			dat[2]=':';
			dat[3]=min/10+'0';
			dat[4]=min%10+'0';
			dat[5]=':';
			dat[6]=sec/10+'0';
			dat[7]=sec%10+'0';
			dat[8]='\0';
			
		}
		GUI_Show12Char(80,100,dat,YELLOW,BLACK);				
	
	
		if(TouchData.lcdx>200&&TouchData.lcdx<240&&TouchData.lcdy>380&&TouchData.lcdy<400)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}	
}




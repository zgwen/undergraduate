#include "public.h"
u8 timebz;
void rtc_init()
{
   	NVIC_InitTypeDef NVIC_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//打开电源时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);//打开存储器时钟

	PWR_BackupAccessCmd(ENABLE);//使能或者失能RTC和后备寄存器访问
	BKP_DeInit();//将外设BKP的全部寄存器重设为缺省值
	RCC_LSEConfig(RCC_LSE_ON);//设置外部低速晶振（LSE）
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//检查指定的RCC标志位设置与否
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//设置RTC时钟（RTCCLK）
	RCC_RTCCLKCmd(ENABLE);//使能或者失能RTC时钟
	RTC_WaitForSynchro(); //等待RTC寄存器同步
	RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//使能或者失能指定的RTC中断
	RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
	RTC_SetPrescaler(32767);//设置预分频 使用外部晶振为32.768K，要想1s中断则预分频数设置为32767，系统会在此数字基础上加1
	RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
		
}

void clockinit_RTC()
{
	if(BKP_ReadBackupRegister(BKP_DR1)!=0XA5A5)//从指定的后备寄存器中读出数据
	{
		//第一次运行  初始化设置
		//RTC初始化
		rtc_init();
		RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
		RTC_SetCounter(0x9f92);//设置RTC计数器的值 11:20：50
		RTC_WaitForLastTask();
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
	}
	else
	{	
		RTC_WaitForSynchro();//等待RTC寄存器同步	
		RTC_WaitForLastTask();//等待写RTC寄存器完成		
	    RTC_ITConfig(RTC_IT_SEC,ENABLE);//使能RTC秒中断    
		RTC_WaitForLastTask();//等待写RTC寄存器完成	
	}
	RCC_ClearFlag();//清除复位标志；		
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
			GUI_Show12Char(10,10,"RTC时钟实验",YELLOW,BLACK);
			GUI_Show12Char(200,380,"返回",YELLOW,BLACK);	
		}
		
		if(timebz==1)
		{
			timebz=0;
			timedata=RTC_GetCounter();//获取RTC计数器的值
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




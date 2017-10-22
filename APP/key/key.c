#include "key.h"
void key_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pin=K_UP;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//��������  //����ѡ�񸡿������������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=K_DOWN|K_LEFT|K_RIGHT;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
				
	GPIO_ResetBits(GPIOA,K_UP);	//��K_UP��ʼ�����0
}
void key_pros()	  //����������
{
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	key_init();
	while(1)
	{
		if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>125&&TouchData.lcdy<175)
		{		
			GUI_Show12Char(100,10,"��������",YELLOW,BLACK);
			GUI_Show12Char(10,40,"ͨ�����¿���������İ������ڲ����ϵķ���任��ɫ",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);
			GUI_Box(10, 180, 70,220, YELLOW);	
			GUI_Box(90, 180, 150,220, YELLOW);
			GUI_Box(10, 240, 70,280, YELLOW);	
			GUI_Box(90, 240, 150,280, YELLOW);

			TouchData.lcdx=0;
			TouchData.lcdy=0;
			
		}
		if(k_up==1)	  //�жϰ���k_up�Ƿ���
		{
			delay_ms(10); //��������
			if(k_up==1)	 //�ٴ��жϰ���k_up�Ƿ���
			{
				GUI_Box(10, 180, 70,220, BLUE);					
			}
			while(k_up); //�ȴ������ɿ�
		}
		if(k_down==0)
		{
			delay_ms(10);
			if(k_down==0)
			{
				GUI_Box(90, 180, 150,220, BLUE);			
			}
			while(!k_down);
		}
		if(k_left==0)
		{
			delay_ms(10);
			if(k_left==0)
			{
				GUI_Box(10, 240, 70,280, BLUE);			
			}
			while(!k_left);
		}
		if(k_right==0)
		{
			delay_ms(10);
			if(k_right==0)
			{
				GUI_Box(90, 240, 150,280, BLUE);			
			}
			while(!k_right);
		}			
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}
	
}


#include "lowpower.h"
#include "key.h"

void standmodeinit()   //����ģʽ
{	
	NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP,ENABLE);//ѡ��ϵͳ����͹���ģʽ������	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//ʹ��PWR����ʱ��	
	PWR_WakeUpPinCmd(ENABLE);//ʹ�ܻ��ѹܽ�	ʹ�ܻ���ʧ�ܻ��ѹܽŹ���
	PWR_EnterSTANDBYMode();//�������ģʽ		
}
void low_pros()
{
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	while(1)
	{
		if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>215&&TouchData.lcdy<265)
		{		
		
			GUI_Show12Char(10,10,"�͹���-����ģʽʵ��",YELLOW,BLACK);	
			GUI_Show12Char(10,30,"K_LEFT: �������ģʽ��",YELLOW,BLACK);
			GUI_Show12Char(10,50,"K_UP: �˳�����ģʽ��",YELLOW,BLACK);
			GUI_Show12Char(10,130,"�ȴ��������ģʽ...",YELLOW,BLACK);
			GUI_Show12Char(10,180,"�������ģʽ�Ժ���K_UP���˳�����������",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);		
		}	
		if(k_left==0)
		{
			delay_ms(10);
			if(k_left==0)
			{
				GUI_Show12Char(10,130,"***�������ģʽ��***",YELLOW,BLACK);
				standmodeinit();   //����ģʽ
			}
		}		

		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}	
}

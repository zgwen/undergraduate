#include "public.h"

/*******************************************************************************
* �� �� ��         : iwdg_init
* ��������		   : �������Ź�ģʽ���ó�ʼ��	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void iwdg_init()	//�������Ź���ʼ������
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ�ܼĴ�����д����
	IWDG_SetPrescaler(IWDG_Prescaler_64);//����IWDGԤ��Ƶֵ//40K/64=1.6ms
	IWDG_SetReload(800);//����IWDG��װ��ֵ  12λ�ģ�0~4095��//800*1.6ms=1.28s
	IWDG_ReloadCounter();//����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
	IWDG_Enable();//ʹ��IWDG
}
void iwdg_pros()
{
	iwdg_init();   //�������Ź���ʼ������
	while(1)
	{
		if(TOUCH_Scan() == 0)
		{		
			TFT_ClearScreen(BLACK);	
			GUI_Show12Char(10,10,"�������Ź�ʵ��",YELLOW,BLACK);
			GUI_Show12Char(200,380,"����",YELLOW,BLACK);		
		}	
		IWDG_ReloadCounter();	   //ι����ʱ����1.28s
		GUI_Show12Char(10,100,"����ι��...",YELLOW,BLACK);
		delay_ms(10);	//ֻҪ��ι��ʱ����ι���Ͳ�����ϵͳ��Ϊ�������븴λ״̬
		if(TouchData.lcdx>200&&TouchData.lcdx<240&&TouchData.lcdy>380&&TouchData.lcdy<400)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}	
}

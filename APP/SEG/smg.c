#include "smg.h"
u8 smgduan[16]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
             0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};//0~F ����ܶ�ѡ����


/*******************************************************************************
* �� �� ��         : smg_init
* ��������		   : ����ܶ˿ڳ�ʼ������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void smg_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;  //����һ���ṹ�������������ʼ��GPIO
	/* ����GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=smg_duan;	  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);		/* ��ʼ��GPIO */
}

void static_smg_display()	//��̬�������ʾ
{	
	u8 i,j;
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	smg_init();
	while(1)
	{
		if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>35&&TouchData.lcdy<90)
		{		
			GUI_Show12Char(10,10,"�����ʵ��",YELLOW,BLACK);
			GUI_Show12Char(10,50,"�����ѭ����ʾ0-F",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);	
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

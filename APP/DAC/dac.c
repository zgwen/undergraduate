#include "dac.h"
void dac_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;//DAC_1
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//ģ��������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);//�����
	

	DAC_InitStructure.DAC_Trigger=DAC_Trigger_None;//��ʹ�ô�������
    DAC_InitStructure.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�����ǲ�
   //���� ��ֵ����
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
   //�رջ���
    DAC_InitStructure.DAC_OutputBuffer=DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1,&DAC_InitStructure);//��ʼ��DACͨ��1
    DAC_Cmd(DAC_Channel_1,ENABLE);//ʹ��DAC1
    DAC_SetChannel1Data(DAC_Align_12b_R,0);//12λ �Ҷ��� д0����
}
void DAC_pros()
{
	u16 value;
	float da;
	u8 i=0,j,dat[7];
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	dac_init();
	
	while(1)
	{	
		if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>300&&TouchData.lcdy<355)
		{
			GUI_Show12Char(10,10,"DAC����...",YELLOW,BLACK);
			GUI_Show12Char(10,100,"The Output Vologe is:",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);
		}				
		value=0;
		for(i=0;i<=10;i++)	
		{
			for(j=0;j<20;j++)
			{
				value=i*400;
				DAC_SetChannel1Data(DAC_Align_12b_R,value);//12λ �Ҷ��� PA4 �˿����	
				da=3.3*value/4095;
				value=da*100;
				dat[0]=value/100+0x30;
				dat[1]='.';
				dat[2]=value%100/10+0x30;
				dat[3]=value%100%10+0x30;
				dat[4]='V';
				dat[5]='\0';
				GUI_Show12Char(180,100,dat,YELLOW,BLACK);
				delay_ms(10);
				TOUCH_Scan();
				if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)
				{
					break;					
				}			
			}
			if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)
			{
				break;					
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
 



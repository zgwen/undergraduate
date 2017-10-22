#include "adc.h"

void adc_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_ADC1,ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M  ���14M ����ADCʱ�ӣ�ADCCLK��
	ADC_DeInit(ADC1);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADC
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//ģ������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 1; 
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);
	
	ADC_Cmd(ADC1,ENABLE);	

	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
	
	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������

}
void adc_change(void)
{
	float ad;
	u8 dat[6];
	u8 i;
	u16 ad_value;
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	adc_init();
	while(1)
	{
		if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>35&&TouchData.lcdy<90)
		{		
			GUI_Show12Char(10,10,"ADת��ʵ��",YELLOW,BLACK);
			GUI_Show12Char(10,50,"ͨ������ADCģ���ڵĵ�λ�����Ըı����ѹ",YELLOW,BLACK);	
			GUI_Show12Char(10,100,"AD���ֵ�ǣ�",YELLOW,BLACK);
		
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);		
		}			
		ad_value=0;
		for(i=0;i<10;i++)//��ȡ50�ε�AD��ֵȡ��ƽ������Ϊ׼ȷ	
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//ת��������־λ
			ad_value=ad_value+ADC_GetConversionValue(ADC1);//�������һ��ADCx�������ת�����		
		}
		ad_value=ad_value/10;
		ad=ad_value*3.3/4096;
		ad_value=(u16)(ad*100);
		dat[0]=ad_value/100+0x30;
		dat[1]='.';
		dat[2]=ad_value%100/10+0x30;
		dat[3]=ad_value%100%10+0x30;
		dat[4]='V';
		dat[5]='\0';
		
		GUI_Show12Char(107,100,dat,YELLOW,BLACK);
				
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();	
	}				
}

#include "adc.h"

void adc_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_ADC1,ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M  最大14M 设置ADC时钟（ADCCLK）
	ADC_DeInit(ADC1);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADC
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//模拟输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 1; 
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);
	
	ADC_Cmd(ADC1,ENABLE);	

	ADC_ResetCalibration(ADC1);//重置指定的ADC的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态
	
	ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC的校准程序

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的ADC的软件转换启动功能

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
			GUI_Show12Char(10,10,"AD转换实验",YELLOW,BLACK);
			GUI_Show12Char(10,50,"通过调节ADC模块内的电位器可以改变检测电压",YELLOW,BLACK);	
			GUI_Show12Char(10,100,"AD检测值是：",YELLOW,BLACK);
		
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);		
		}			
		ad_value=0;
		for(i=0;i<10;i++)//读取50次的AD数值取其平均数较为准确	
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//转换结束标志位
			ad_value=ad_value+ADC_GetConversionValue(ADC1);//返回最近一次ADCx规则组的转换结果		
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

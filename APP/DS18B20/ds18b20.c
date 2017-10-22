#include "ds18b20.h"

/*******************************************************************************
* �� �� ��         : ds18b20_init
* ��������		   : IO�˿�ʱ�ӳ�ʼ������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ds18b20_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);

	GPIO_InitStructure.GPIO_Pin=dq;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_ds18b20,&GPIO_InitStructure);
}

/*******************************************************************************
* �� �� ��         : DQININT
* ��������		   : ��������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void DQININT()	 //��������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=dq;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_ds18b20,&GPIO_InitStructure);	
}

/*******************************************************************************
* �� �� ��         : DQOUTINT
* ��������		   : �������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void DQOUTINT()	 //�������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=dq;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_ds18b20,&GPIO_InitStructure);	
}

/*******************************************************************************
* �� �� ��         : ds18b20init
* ��������		   : DS18B20��ʼ��ʱ��	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ds18b20init()
{
	DQOUTINT();//���
	ds18b20_dq_L;
	delay_us(480);//��ʱ480΢��	
	ds18b20_dq_H;
	delay_us(480);//��ʱ480΢��
}

/*******************************************************************************
* �� �� ��         : ds18b20wr
* ��������		   : DS18B20д����ʱ��	   
* ��    ��         : dat
* ��    ��         : ��
*******************************************************************************/
void ds18b20wr(u8 dat)
{
	u8 i=0;
	DQOUTINT();//���

	for(i=0;i<8;i++)
	{
		ds18b20_dq_L;	 //����
		delay_us(15);//��ʱ15΢��
		
		if((dat&0x01)==1)
		{
			ds18b20_dq_H;
		}
		else
		{
			ds18b20_dq_L;
		}
		delay_us(60);//��ʱ60΢��
		ds18b20_dq_H;
		
		dat>>=1;//׼����һλ���ݵķ���	
	}
}

/*******************************************************************************
* �� �� ��         : DS18b20rd
* ��������		   : DS18B20������ʱ��	   
* ��    ��         : ��
* ��    ��         : value
*******************************************************************************/
u8 DS18b20rd()
{
	u8 i=0,value=0;

	for(i=0;i<8;i++)
	{
		value>>=1;
		DQOUTINT();//���
		ds18b20_dq_L;	 //����
		delay_us(4);//��ʱ4΢��
		ds18b20_dq_H;
		delay_us(10);//��ʱ10΢��
		DQININT();	 //��������

		if(GPIO_ReadInputDataBit(GPIO_ds18b20,dq)==1)
		{
		   value|=0x80;//������ �ӵ�λ��ʼ
		}

		delay_us(45);//��ʱ45΢��
	}

	return value;	
}

/*******************************************************************************
* �� �� ��         : readtemp
* ��������		   : DS18B2�¶ȼĴ������ã��¶ȶ�ȡ	   
* ��    ��         : ��
* ��    ��         : value
*******************************************************************************/
double readtemp()			  //��ȡ�¶�����Ҫ��λ��
{
	u16 temp;
	u8 a,b;
	double value;
	ds18b20init();		//��ʼ��
	ds18b20wr(0xcc);   //���ͺ���ROMָ��
	ds18b20wr(0x44);   //�����¶�ת��ָ��
	delay_ms(10);	 
	ds18b20init();	   //��ʼ��
	ds18b20wr(0xcc);   //���ͺ���ROMָ��
	ds18b20wr(0xbe);   //�����ݴ���ָ��
	a=DS18b20rd();	 //�¶ȵĵͰ�λ
	b=DS18b20rd();	 //�¶ȵĸ߰�λ
	temp=b;
	temp=(temp<<8)+a;
	if((temp&0xf800)==0xf800)
	{
		temp=(~temp)+1;
		value=temp*(-0.0625);
	}
	else
	{
		value=temp*0.0625;	
	}
	return value;
}
void DS18B20_pros()
{
	double temp;
	u16 tem_dat;
	u8 dat[8];
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	ds18b20_init();
	while(1)
	{

		if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>125&&TouchData.lcdy<175)
		{		
			GUI_Show12Char(10,10,"DS18B20�¶ȼ�⣺",YELLOW,BLACK);
			GUI_Show12Char(10,40,"���¶ȴ�������������ӿ��ϣ�ע���¶ȴ������ķ��򣬰������ж�Ӧ��˿ӡָʾ",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);	
		}
			
		temp=readtemp();
		tem_dat=temp*100;
		dat[0]=tem_dat/1000+0x30;
		dat[1]=tem_dat%1000/100+0x30;
		dat[2]='.';
		dat[3]=tem_dat%1000%100/10+0x30;
		dat[4]=tem_dat%1000%100%10+0x30;
		dat[5]='C';
		dat[6]='\0';
		GUI_Show12Char(150,10,dat,YELLOW,BLACK);				
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();	
	}
			
}


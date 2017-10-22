#include "AT24CXX.h"
#include "key.h"

u8 AT24Cxx_ReadOneByte(u16 addr)
{
	u8 temp=0;

	I2C_Start();
	
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1);
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(0); //  0   ���� NACK
	I2C_NAck();
	I2C_Stop();
	
	return temp;	
}

u16 AT24Cxx_ReadTwoByte(u16 addr)
{
	u16 temp=0;

	I2C_Start();
	
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1);
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(1); //  1   ���� ACK
	temp<<=8;
	temp|=I2C_Read_Byte(0); //  0  ���� NACK

	I2C_Stop();
	
	return temp;	
}


void AT24Cxx_WriteOneByte(u16 addr,u8 dt)
{
	I2C_Start();

	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Send_Byte(dt);
	I2C_Wait_Ack();
	I2C_Stop();

	delay_ms(10);
}

void AT24Cxx_WriteTwoByte(u16 addr,u16 dt)
{
	I2C_Start();

	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Send_Byte(dt>>8);
	I2C_Wait_Ack();

	I2C_Send_Byte(dt&0xFF);
	I2C_Wait_Ack();

	I2C_Stop();

	delay_ms(10);
}
void IIC_pros()
{
	u8 wdata=0,value;
	u8 dat[6],dat1[6];
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	I2C_INIT();
	while(1)
	{
		if(TouchData.lcdx>170&&TouchData.lcdx<230&&TouchData.lcdy>125&&TouchData.lcdy<175)
		{			
			GUI_Show12Char(10,10,"AT24C02�洢����дʵ��",YELLOW,BLACK);
			GUI_Show12Char(10,27,"PB10 PB11 ��IIC�ӿ�",YELLOW,BLACK);
			GUI_Show12Char(10,44,"��K_UP��:��AT24C02д����",YELLOW,BLACK);
			GUI_Show12Char(10,61,"��K_DOWM��:��AT24C02������",YELLOW,BLACK);
			GUI_Show12Char(10,100,"д���ݵķ�Χ��: 0-255",YELLOW,BLACK);
			GUI_Show12Char(10,150,"Write data is:",YELLOW,BLACK);
			GUI_Show12Char(10,167,"Read data is:",YELLOW,BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);
			TOUCH_Scan();	
		}
		if(k_up==1)		   //����k_up����д������
		{
			delay_ms(10);
			if(k_up==1)
			{	
				AT24Cxx_WriteOneByte(0,++wdata);//д����ǵ��ֽڣ�0-255ȡֵ��Χ
				dat[0]=wdata/100+0x30;
				dat[1]=wdata%100/10+0x30;
				dat[2]=wdata%100%10+0x30;
				dat[3]='\0';
				GUI_Show12Char(160,150,dat,YELLOW,BLACK);	
			}
			while(k_up);
		}
		if(k_down==0)	   //��ȡ����
		{
			delay_ms(10);
			if(k_down==0)
			{
				value=AT24Cxx_ReadOneByte(0);
				dat1[0]=value/100+0x30;
				dat1[1]=value%100/10+0x30;
				dat1[2]=value%100%10+0x30;
				dat1[3]='\0';
				GUI_Show12Char(160,167,dat1,YELLOW,BLACK);
			}
			while(!k_down);	
		}
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();
	}	
}


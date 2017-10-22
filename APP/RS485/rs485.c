#include "rs485.h"
void rs485_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_InitTypeDef USART_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;	//TX-485
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;	//CS-485
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;	//RX-485
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);	
	USART_Cmd(USART2,ENABLE);	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ClearFlag(USART2,USART_FLAG_TC);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
}

void USART2_IRQHandler(void)	//485ͨ���жϺ���
{
	static u8 k;
	USART_ClearFlag(USART2,USART_FLAG_TC);
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//���ָ����USART�жϷ������	
	{
		k=USART_ReceiveData(USART2);
		GPIO_SetBits(GPIOG,GPIO_Pin_3);
		delay_ms(1);
		USART_SendData(USART2,k);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
		delay_ms(2);		
		GPIO_ResetBits(GPIOG,GPIO_Pin_3);
	}
}


void RS485_pros()
{
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	rs485_init();
	GPIO_ResetBits(GPIOG,GPIO_Pin_3);//����Ҫ��485�������ݵĿ��ƶ�
	while(1)
	{
		if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>410&&TouchData.lcdy<460)
		{		
			
			GUI_Show12Char(10,10,"RS485����COM2���ԣ�ͨ���������ֲ鿴���ͺͽ�������",YELLOW,BLACK);
			GUI_Show12Char(10,50,"ע�⣺���������ϵ�A B���Ӷ�Ӧ�ӵ�USBת485���ӵ�A B",YELLOW,BLACK);
			GUI_Show12Char(10,100,"�ڴ��������ϲ������趨Ϊ9600�����ݸ�ʽΪ8������żУ��",YELLOW,BLACK);
			GUI_Show12Char(10,180,"���Ҫʹ��WIFI�����Խ�RS485ģ��̽�Ƭ�̽ӵ�WIFI�ˣ����ʱ��485���ܾ�ʧЧ�������Ҫ����485��Ҫ���̽�Ƭ��ԭ����",YELLOW,BLACK);
			

			GUI_Show12Char(280,450,"����",YELLOW,BLACK);		
		}
		
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();		
	}	
}


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

void USART2_IRQHandler(void)	//485通信中断函数
{
	static u8 k;
	USART_ClearFlag(USART2,USART_FLAG_TC);
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//检查指定的USART中断发生与否	
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
	GPIO_ResetBits(GPIOG,GPIO_Pin_3);//首先要打开485接收数据的控制端
	while(1)
	{
		if(TouchData.lcdx>20&&TouchData.lcdx<75&&TouchData.lcdy>410&&TouchData.lcdy<460)
		{		
			
			GUI_Show12Char(10,10,"RS485串口COM2测试，通过串口助手查看发送和接收数据",YELLOW,BLACK);
			GUI_Show12Char(10,50,"注意：将开发板上的A B端子对应接到USB转485端子的A B",YELLOW,BLACK);
			GUI_Show12Char(10,100,"在串口助手上波特率设定为9600，数据格式为8，无奇偶校验",YELLOW,BLACK);
			GUI_Show12Char(10,180,"如果要使用WIFI，可以将RS485模块短接片短接到WIFI端，这个时候485功能就失效，如果还要测试485就要将短接片还原回来",YELLOW,BLACK);
			

			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);		
		}
		
		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();		
	}	
}


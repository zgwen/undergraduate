#include "usart.h"
void usart_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;	//IO口结构体定义

	USART_InitTypeDef  USART_InitStructure;	  //串口结构体定义

	NVIC_InitTypeDef NVIC_InitStructure;//中断结构体定义
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate=9600;   //波特率设置为9600
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断 接收中断
	USART_ClearFlag(USART1,USART_FLAG_TC);//清除USARTx的待处理标志位


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

}

void USART1_IRQHandler(void)	//串口1中断函数
{
	static u8 k;
	USART_ClearFlag(USART1,USART_FLAG_TC);
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
	{
		k=USART_ReceiveData(USART1);	
		USART_SendData(USART1,k);//通过外设USARTx发送单个数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);	
	}
}


void COM_pros()
{
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	usart_init();
	while(1)
	{
		if(TouchData.lcdx>245&&TouchData.lcdx<305&&TouchData.lcdy>300&&TouchData.lcdy<355)
		{		
			
			GUI_Show12Char(10,10,"串口COM1测试，通过串口助手查看发送和接收数据",YELLOW,BLACK);
			GUI_Show12Char(10,50,"在串口助手上波特率设定为9600，数据格式为8，无奇偶校验",YELLOW,BLACK);
			GUI_Show12Char(10,100,"如果要使用COM3，可以将COM3模块短接片P232短接到COM3端，这个时候IIC功能就失效，如果还要测试IIC就要将短接片还原回来",YELLOW,BLACK);
			
			TOUCH_Scan();
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

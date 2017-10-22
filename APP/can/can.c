#include "can.h"
#include "key.h"

#ifdef CAN_RX0_INT_ENABLE

uint8_t CAN_RX_BUFF[8];

#endif

uint8_t ShowData[5] = {'0', 'x', '0', '0', 0};
uint8_t CAN_SendData[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}; //最后两位用来存放发送ID和接收ID
uint8_t *Mode;
const uint8_t CAN_ModeNormal[9] = {"正常模式"};
const uint8_t CAN_ModeLoopBack[9] = {"环回模式"};

/****************************************************************************
* Function Name  : CAN1_NVIC_Config
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

static void CAN1_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 使能接收的中断和中断优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);   //FIFO0消息挂号中断允许.	
}

/****************************************************************************
* Function Name  : CAN1_Config
* Description    : 初始化CAN，波特率设置为450K
* Input          : mode：用来选择要使用的工作模式：主要有四种工作模式：1、正常
*                * 模式：CAN_Mode_Normal；2、CAN_Mode_Silent ：静默模式；3、环
*                * 回模式：CAN_Mode_LoopBack；4、静默环回模式：CAN_Mode_Silent
*                * _LoopBack。
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_Config(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;

	/* 初始化IO口 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //PA12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;        //PA11

	GPIO_Init(GPIOA, &GPIO_InitStructure);

/***************************************************************************/
/********************* CAN设置和初始化 *************************************/
/***************************************************************************/

	/* 打开时钟使能 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    /* 初始化CAN的参数 */

	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN 参数初始化 */
	CAN_InitStructure.CAN_TTCM = DISABLE;    //失能时间触发模式
	CAN_InitStructure.CAN_ABOM = DISABLE;    //失能自动离线管理
	CAN_InitStructure.CAN_AWUM = DISABLE;    //失能睡眠模式通过软件唤醒
	CAN_InitStructure.CAN_NART = DISABLE;    //失能非自动重传输模式（也就是会自动重传输）
	CAN_InitStructure.CAN_RFLM = DISABLE;    //失能接收FIFO锁定模式，新数据会覆盖旧数据
	CAN_InitStructure.CAN_TXFP = DISABLE;    //优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode = mode;       //有普通模式和拓展模式
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //重新同步跳跃宽度 1 个时间单位

    /* 波特率设置, 当APB1的时钟频率是36MHZ的时候。 波特率的公式为： */
    /* 波特率(Kpbs) = 36M / ((CAN_BS1 + CAN_BS2 + 1) *  CAN_Prescaler) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq; //时间段 1 为8 个时间单位 
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq; //时间段 2 为7 个时间单位
	CAN_InitStructure.CAN_Prescaler = 5;	 

	CAN_Init(CAN1, &CAN_InitStructure);

#ifdef CAN_RX0_INT_ENABLE
	CAN1_NVIC_Config();
#endif    

}

/****************************************************************************
* Function Name  : CAN1_SendMesg
* Description    : 发送一个报文
* Input          : id：发送的ID。
*                * len：发送的数据长度(注意发送数据长度不能超过8个字节)
*                * dat：存放数据的指针
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_SendMesg(uint32_t id, uint8_t len, uint8_t *dat)
{
	uint16_t i = 0;
	CanTxMsg TxMessage;

    /* 一次发送只能发送8个字节 */
    if(len > 8)
    {
        return ;
    }
	/* 配置邮箱：设置标识符，数据长度和待发送数据 */
	TxMessage.StdId = (id & 0x7FF); //标准帧ID11位
	TxMessage.ExtId = (id >> 11);   //设置扩展标示符（拓展标示符有29位）
	TxMessage.RTR = CAN_RTR_DATA;   //设置为数据帧（或远程帧为CAN_RTR_Remote）
    if((id & 0x7FF) == 0x7FF)       //检测是标准帧还是拓展帧（拓展帧大于11位）
    {
    	TxMessage.IDE = CAN_ID_STD;	//拓展ID   
    }
    else
    {
    	TxMessage.IDE = CAN_ID_EXT;	//标准ID
    }
	TxMessage.DLC = len;	        //发送的数据长度

	/* 将数据放入到邮箱中 */
	for(i=0; i<len; i++)	         
	{
		TxMessage.Data[i] = *dat;
		dat++;	
	}
    
    /* 开始传送数据 */
	CAN_Transmit(CAN1, &TxMessage); 
}

/****************************************************************************
* Function Name  : CAN1_Config16BitFilter
* Description    : 设置CAN接收16两个标准ID（设置ID位数全部相同才能够通过）
* Input          : id1：要接收的一个ID
*                * id2：要接收的一个ID
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_Config16BitFilter(uint16_t id1, uint16_t id2)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    uint16_t mask = 0xFFFF;

	/* CAN filter init 屏蔽寄存器初始化 */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;	               //过滤器1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//ID模式

	/* 寄存器组设置为16位 */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (id1 << 5);    //要接收的ID标示符1		
    CAN_FilterInitStructure.CAN_FilterIdLow =  (id2 << 5);	  //要接收的ID标示符2

	/* 设置为所有ID位都要相同才接收 */	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (mask << 5); //MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = (mask << 5);
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //使能过滤器1

	CAN_FilterInit(&CAN_FilterInitStructure);
}

/****************************************************************************
* Function Name  : CAN1_Config32BitFilter
* Description    : 设置一个拓展ID的接收
* Input          : id：要接收的ID
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_Config32BitFilter(uint32_t id)
{
    uint32_t mask = 0xFFFFFFFF;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* CAN filter init 屏蔽寄存器初始化 */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;	               //过滤器1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//ID模式

	/* 寄存器组设置为32位 */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (id >> 13);    //要接收的ID标示符1		
    CAN_FilterInitStructure.CAN_FilterIdLow =  (id << 3 ) | 4;//要接收的ID标示符2

	/* 设置为所有ID位都要相同才接收 */	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = mask >> 13;     //MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = (mask << 3) | 4;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //使能过滤器1

	CAN_FilterInit(&CAN_FilterInitStructure);
}

/****************************************************************************
* Function Name  : CAN1_ReceiveMesg
* Description    : 接收一个报文
* Input          : receiveBuff：接收数据的数组指针
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_ReceiveMesg(uint8_t *receiveBuff)
{
	uint8_t i = 0;

	CanRxMsg RxMessage;	//设置接收邮箱

	if((CAN_MessagePending(CAN1, CAN_FIFO0) != 0)) //检查FIFO0里面是否有数据
	{
    	CAN_Receive(CAN1,CAN_FIFO0,&RxMessage); //读取FIFO0里面的数据
    	for(i=0; i<RxMessage.DLC; i++)          //将读取到的数据位赋给CAN_RXSBUF
    	{
    		*receiveBuff = RxMessage.Data[i];
    		receiveBuff++;
    	}
    }			
}

/****************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : FIFO0接收一个报文数据数据（最大8个字节）
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

#ifdef CAN_RX0_INT_ENABLE

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN1_ReceiveMesg(CAN_RX_BUFF);	
}

#endif

void GUI_DisplayData(uint8_t num)
{
    uint8_t addr;
    
    for(addr=0; addr<10; addr++)
    {   
        if(addr < 8)
        {
            /* 如果接收到数据，显示数据 */
            /* 将读取到的16位数据转换成可以显示的ASCII码 */
            ShowData[2] = CAN_RX_BUFF[addr] >> 4;
            if(ShowData[2] > 9)
            {
                ShowData[2] += '7';  //当大于9时，用ABCDEF来表示
            }
            else
            {
                ShowData[2] += '0';    
            }
            ShowData[3] = CAN_RX_BUFF[addr] & 0x0F;
            if(ShowData[3] > 9)
            {
                ShowData[3] += '7';  //当大于9时，用ABCDEF来表示
            }
            else
            {
                ShowData[3] += '0';    
            }
            /* 显示接收到的数据 */
            GUI_Show12Char(208, (126 + addr * 21), ShowData, RED, BLACK);
        }
        
        /* 将要发送的16位数据转换成可以显示的ASCII码 */
        ShowData[2] = CAN_SendData[addr] >> 4;
        if(ShowData[2] > 9)
        {
            ShowData[2] += '7';    //当大于9时，用ABCDEF来表示
        }
        else
        {
            ShowData[2] += '0';    
        }
        ShowData[3] = CAN_SendData[addr] & 0x0F;
        if(ShowData[3] > 9)
        {
            ShowData[3] += '7';    //当大于9时，用ABCDEF来表示
        }
        else
        {
            ShowData[3] += '0';    
        }

        if(addr < 8)
        {
            /* 显示发送数据，选择高亮位置 */
            if(num == addr)
            {
                GUI_Show12Char(88, (126 + addr * 21), ShowData, GREEN, BLACK);
            }
            else
            {
                GUI_Show12Char(88, (126 + addr * 21), ShowData, RED, BLACK);
            }
        }
        else //显示ID部分
        {
            if(addr == 8)
            {
                if(num == 8)
                {
                    GUI_Show12Char(52, 105, ShowData, GREEN, BLACK); 
                }
                else
                {
                    GUI_Show12Char(52, 105, ShowData, RED, BLACK);
                }
            }
            if(addr == 9)
            {
                if(num == 9)
                {
                    GUI_Show12Char(176, 105, ShowData, GREEN, BLACK); 
                }
                else
                {
                    GUI_Show12Char(176, 105, ShowData, RED, BLACK);
                }
            }
        }        
    }
    if(num > 9)
    {
        GUI_Show12Char(104, 84, Mode, GREEN, BLACK);
    }
    else
    {
        GUI_Show12Char(104, 84, Mode, RED, BLACK);        
    }
}

void CAN_pros()
{
	uint8_t m, receiveId,canMode;
	m = 10;
    canMode = 1; //要设置canMode不等于CAN_SendData[10],以便进入循环一开始就初始化
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	while(1)
	{
		if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>410&&TouchData.lcdy<460)
		{		
		
			GUI_Show12Char(10, 10, "注：默认接收ID0,波特率为450KHZ", BLUE, BLACK);

		    GUI_Show12Char(76, 63, "CAN通信实验", RED, BLACK);
		    /* 显示工作模式 */
		    GUI_Show12Char(0, 84, "CAN工作模式：", RED, BLACK);
		
		    /* 显示发送接收ID */
		    GUI_Show12Char(0, 105, "发送ID:", RED, BLACK);
		    GUI_Show12Char(120, 105, "接收ID:", RED, BLACK);
		    
		    /* 显示发送和接收数据的8位数 */    
		    GUI_Show12Char(0, 126, "发送第一位：", RED, BLACK);
		    GUI_Show12Char(120, 126, "接收第一位：", RED, BLACK);
		    GUI_Show12Char(0, 147, "发送第二位：", RED, BLACK);
		    GUI_Show12Char(120, 147, "接收第二位：", RED, BLACK);
		    GUI_Show12Char(0, 168, "发送第三位：", RED, BLACK);
		    GUI_Show12Char(120, 168, "接收第三位：", RED, BLACK);
		    GUI_Show12Char(0, 189, "发送第四位：", RED, BLACK);
		    GUI_Show12Char(120, 189, "接收第四位：", RED, BLACK);
		    GUI_Show12Char(0, 210, "发送第五位：", RED, BLACK);
		    GUI_Show12Char(120, 210, "接收第五位：", RED, BLACK);
		    GUI_Show12Char(0, 231, "发送第六位：", RED, BLACK);
		    GUI_Show12Char(120, 231, "接收第六位：", RED, BLACK);
		    GUI_Show12Char(0, 252, "发送第七位：", RED, BLACK);
		    GUI_Show12Char(120, 252, "接收第七位：", RED, BLACK);
		    GUI_Show12Char(0, 273, "发送第八位：", RED, BLACK);
		    GUI_Show12Char(120, 273, "接收第八位：", RED, BLACK);
		    GUI_Show12Char(0, 294, "上键：高亮部分加一", BLUE, BLACK);
		    GUI_Show12Char(0, 315, "下键：高亮部分减一", BLUE, BLACK);
		    GUI_Show12Char(0, 336, "左键：高亮部分向下移动一位", BLUE, BLACK);
		    GUI_Show12Char(0, 357, "右键：发送数据", BLUE, BLACK);
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);
			TouchData.lcdx=0;
			TouchData.lcdy=0;		
		}
		TOUCH_Scan();
		/* 修改模式 */
        if(canMode != CAN_SendData[10] )
        {
            canMode = CAN_SendData[10];
            if(CAN_SendData[10])
            {
               CAN1_Config(CAN_Mode_Normal);
               Mode = (uint8_t *)CAN_ModeNormal;    
            }
            else
            {
                CAN1_Config(CAN_Mode_LoopBack);
                Mode = (uint8_t *)CAN_ModeLoopBack;
            }
            CAN1_Config16BitFilter(CAN_SendData[9], 0x00);    
        }
        /* 如果接收ID改变，就重新设置接收ID */
        if(receiveId != CAN_SendData[9])
        {
            receiveId = CAN_SendData[9];
            if(receiveId != 0x00)
            {
                CAN1_Config16BitFilter(CAN_SendData[9], 0x00);
            }                
        }
		/* 显示数据 */
        GUI_DisplayData(m);

		if(k_up==1)	  //判断按键k_up是否按下
		{
			delay_ms(10); //消抖处理
			if(k_up==1)	 //再次判断按键k_up是否按下
			{
				if(m == 10)
                {
                    CAN_SendData[10] = ~CAN_SendData[10];
                }
                else
                {
                    CAN_SendData[m]++;   
                }
                GUI_Show12Char(175, 84, "等待发送", RED, BLACK);					
			}
			while(k_up); //等待按键松开
		}
		if(k_down==0)
		{
			delay_ms(10);
			if(k_down==0)
			{
				if(m == 10)
                {
                    CAN_SendData[10] = ~CAN_SendData[10];
                }
                else
                {
                    CAN_SendData[m]--;    
                }
                GUI_Show12Char(175, 84, "等待发送", RED, BLACK);			
			}
			while(!k_down);
		}
		if(k_left==0)
		{
			delay_ms(10);
			if(k_left==0)
			{
				if(m == 10)
                {
                    m = 0;
                }
                else
                {
                    m++;
                }			
			}
			while(!k_left);
		}
		if(k_right==0)
		{
			delay_ms(10);
			if(k_right==0)
			{
				CAN1_SendMesg(CAN_SendData[8], 8, CAN_SendData);//发送数据
                GUI_Show12Char(175, 84, "发送成功", BLUE, BLACK);			
			}
			while(!k_right);
		}		


		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		TOUCH_Scan();	
	}	
}

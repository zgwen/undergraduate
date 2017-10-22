#include "can.h"
#include "key.h"

#ifdef CAN_RX0_INT_ENABLE

uint8_t CAN_RX_BUFF[8];

#endif

uint8_t ShowData[5] = {'0', 'x', '0', '0', 0};
uint8_t CAN_SendData[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}; //�����λ������ŷ���ID�ͽ���ID
uint8_t *Mode;
const uint8_t CAN_ModeNormal[9] = {"����ģʽ"};
const uint8_t CAN_ModeLoopBack[9] = {"����ģʽ"};

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

	/* ʹ�ܽ��յ��жϺ��ж����ȼ� */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);   //FIFO0��Ϣ�Һ��ж�����.	
}

/****************************************************************************
* Function Name  : CAN1_Config
* Description    : ��ʼ��CAN������������Ϊ450K
* Input          : mode������ѡ��Ҫʹ�õĹ���ģʽ����Ҫ�����ֹ���ģʽ��1������
*                * ģʽ��CAN_Mode_Normal��2��CAN_Mode_Silent ����Ĭģʽ��3����
*                * ��ģʽ��CAN_Mode_LoopBack��4����Ĭ����ģʽ��CAN_Mode_Silent
*                * _LoopBack��
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_Config(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;

	/* ��ʼ��IO�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //PA12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;        //PA11

	GPIO_Init(GPIOA, &GPIO_InitStructure);

/***************************************************************************/
/********************* CAN���úͳ�ʼ�� *************************************/
/***************************************************************************/

	/* ��ʱ��ʹ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    /* ��ʼ��CAN�Ĳ��� */

	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN ������ʼ�� */
	CAN_InitStructure.CAN_TTCM = DISABLE;    //ʧ��ʱ�䴥��ģʽ
	CAN_InitStructure.CAN_ABOM = DISABLE;    //ʧ���Զ����߹���
	CAN_InitStructure.CAN_AWUM = DISABLE;    //ʧ��˯��ģʽͨ���������
	CAN_InitStructure.CAN_NART = DISABLE;    //ʧ�ܷ��Զ��ش���ģʽ��Ҳ���ǻ��Զ��ش��䣩
	CAN_InitStructure.CAN_RFLM = DISABLE;    //ʧ�ܽ���FIFO����ģʽ�������ݻḲ�Ǿ�����
	CAN_InitStructure.CAN_TXFP = DISABLE;    //���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode = mode;       //����ͨģʽ����չģʽ
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //����ͬ����Ծ��� 1 ��ʱ�䵥λ

    /* ����������, ��APB1��ʱ��Ƶ����36MHZ��ʱ�� �����ʵĹ�ʽΪ�� */
    /* ������(Kpbs) = 36M / ((CAN_BS1 + CAN_BS2 + 1) *  CAN_Prescaler) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq; //ʱ��� 1 Ϊ8 ��ʱ�䵥λ 
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq; //ʱ��� 2 Ϊ7 ��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler = 5;	 

	CAN_Init(CAN1, &CAN_InitStructure);

#ifdef CAN_RX0_INT_ENABLE
	CAN1_NVIC_Config();
#endif    

}

/****************************************************************************
* Function Name  : CAN1_SendMesg
* Description    : ����һ������
* Input          : id�����͵�ID��
*                * len�����͵����ݳ���(ע�ⷢ�����ݳ��Ȳ��ܳ���8���ֽ�)
*                * dat��������ݵ�ָ��
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_SendMesg(uint32_t id, uint8_t len, uint8_t *dat)
{
	uint16_t i = 0;
	CanTxMsg TxMessage;

    /* һ�η���ֻ�ܷ���8���ֽ� */
    if(len > 8)
    {
        return ;
    }
	/* �������䣺���ñ�ʶ�������ݳ��Ⱥʹ��������� */
	TxMessage.StdId = (id & 0x7FF); //��׼֡ID11λ
	TxMessage.ExtId = (id >> 11);   //������չ��ʾ������չ��ʾ����29λ��
	TxMessage.RTR = CAN_RTR_DATA;   //����Ϊ����֡����Զ��֡ΪCAN_RTR_Remote��
    if((id & 0x7FF) == 0x7FF)       //����Ǳ�׼֡������չ֡����չ֡����11λ��
    {
    	TxMessage.IDE = CAN_ID_STD;	//��չID   
    }
    else
    {
    	TxMessage.IDE = CAN_ID_EXT;	//��׼ID
    }
	TxMessage.DLC = len;	        //���͵����ݳ���

	/* �����ݷ��뵽������ */
	for(i=0; i<len; i++)	         
	{
		TxMessage.Data[i] = *dat;
		dat++;	
	}
    
    /* ��ʼ�������� */
	CAN_Transmit(CAN1, &TxMessage); 
}

/****************************************************************************
* Function Name  : CAN1_Config16BitFilter
* Description    : ����CAN����16������׼ID������IDλ��ȫ����ͬ���ܹ�ͨ����
* Input          : id1��Ҫ���յ�һ��ID
*                * id2��Ҫ���յ�һ��ID
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_Config16BitFilter(uint16_t id1, uint16_t id2)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    uint16_t mask = 0xFFFF;

	/* CAN filter init ���μĴ�����ʼ�� */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;	               //������1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//IDģʽ

	/* �Ĵ���������Ϊ16λ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (id1 << 5);    //Ҫ���յ�ID��ʾ��1		
    CAN_FilterInitStructure.CAN_FilterIdLow =  (id2 << 5);	  //Ҫ���յ�ID��ʾ��2

	/* ����Ϊ����IDλ��Ҫ��ͬ�Ž��� */	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (mask << 5); //MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = (mask << 5);
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //ʹ�ܹ�����1

	CAN_FilterInit(&CAN_FilterInitStructure);
}

/****************************************************************************
* Function Name  : CAN1_Config32BitFilter
* Description    : ����һ����չID�Ľ���
* Input          : id��Ҫ���յ�ID
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_Config32BitFilter(uint32_t id)
{
    uint32_t mask = 0xFFFFFFFF;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* CAN filter init ���μĴ�����ʼ�� */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;	               //������1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//IDģʽ

	/* �Ĵ���������Ϊ32λ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (id >> 13);    //Ҫ���յ�ID��ʾ��1		
    CAN_FilterInitStructure.CAN_FilterIdLow =  (id << 3 ) | 4;//Ҫ���յ�ID��ʾ��2

	/* ����Ϊ����IDλ��Ҫ��ͬ�Ž��� */	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = mask >> 13;     //MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = (mask << 3) | 4;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //ʹ�ܹ�����1

	CAN_FilterInit(&CAN_FilterInitStructure);
}

/****************************************************************************
* Function Name  : CAN1_ReceiveMesg
* Description    : ����һ������
* Input          : receiveBuff���������ݵ�����ָ��
* Output         : None
* Return         : None
****************************************************************************/

void CAN1_ReceiveMesg(uint8_t *receiveBuff)
{
	uint8_t i = 0;

	CanRxMsg RxMessage;	//���ý�������

	if((CAN_MessagePending(CAN1, CAN_FIFO0) != 0)) //���FIFO0�����Ƿ�������
	{
    	CAN_Receive(CAN1,CAN_FIFO0,&RxMessage); //��ȡFIFO0���������
    	for(i=0; i<RxMessage.DLC; i++)          //����ȡ��������λ����CAN_RXSBUF
    	{
    		*receiveBuff = RxMessage.Data[i];
    		receiveBuff++;
    	}
    }			
}

/****************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : FIFO0����һ�������������ݣ����8���ֽڣ�
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
            /* ������յ����ݣ���ʾ���� */
            /* ����ȡ����16λ����ת���ɿ�����ʾ��ASCII�� */
            ShowData[2] = CAN_RX_BUFF[addr] >> 4;
            if(ShowData[2] > 9)
            {
                ShowData[2] += '7';  //������9ʱ����ABCDEF����ʾ
            }
            else
            {
                ShowData[2] += '0';    
            }
            ShowData[3] = CAN_RX_BUFF[addr] & 0x0F;
            if(ShowData[3] > 9)
            {
                ShowData[3] += '7';  //������9ʱ����ABCDEF����ʾ
            }
            else
            {
                ShowData[3] += '0';    
            }
            /* ��ʾ���յ������� */
            GUI_Show12Char(208, (126 + addr * 21), ShowData, RED, BLACK);
        }
        
        /* ��Ҫ���͵�16λ����ת���ɿ�����ʾ��ASCII�� */
        ShowData[2] = CAN_SendData[addr] >> 4;
        if(ShowData[2] > 9)
        {
            ShowData[2] += '7';    //������9ʱ����ABCDEF����ʾ
        }
        else
        {
            ShowData[2] += '0';    
        }
        ShowData[3] = CAN_SendData[addr] & 0x0F;
        if(ShowData[3] > 9)
        {
            ShowData[3] += '7';    //������9ʱ����ABCDEF����ʾ
        }
        else
        {
            ShowData[3] += '0';    
        }

        if(addr < 8)
        {
            /* ��ʾ�������ݣ�ѡ�����λ�� */
            if(num == addr)
            {
                GUI_Show12Char(88, (126 + addr * 21), ShowData, GREEN, BLACK);
            }
            else
            {
                GUI_Show12Char(88, (126 + addr * 21), ShowData, RED, BLACK);
            }
        }
        else //��ʾID����
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
    canMode = 1; //Ҫ����canMode������CAN_SendData[10],�Ա����ѭ��һ��ʼ�ͳ�ʼ��
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	while(1)
	{
		if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>410&&TouchData.lcdy<460)
		{		
		
			GUI_Show12Char(10, 10, "ע��Ĭ�Ͻ���ID0,������Ϊ450KHZ", BLUE, BLACK);

		    GUI_Show12Char(76, 63, "CANͨ��ʵ��", RED, BLACK);
		    /* ��ʾ����ģʽ */
		    GUI_Show12Char(0, 84, "CAN����ģʽ��", RED, BLACK);
		
		    /* ��ʾ���ͽ���ID */
		    GUI_Show12Char(0, 105, "����ID:", RED, BLACK);
		    GUI_Show12Char(120, 105, "����ID:", RED, BLACK);
		    
		    /* ��ʾ���ͺͽ������ݵ�8λ�� */    
		    GUI_Show12Char(0, 126, "���͵�һλ��", RED, BLACK);
		    GUI_Show12Char(120, 126, "���յ�һλ��", RED, BLACK);
		    GUI_Show12Char(0, 147, "���͵ڶ�λ��", RED, BLACK);
		    GUI_Show12Char(120, 147, "���յڶ�λ��", RED, BLACK);
		    GUI_Show12Char(0, 168, "���͵���λ��", RED, BLACK);
		    GUI_Show12Char(120, 168, "���յ���λ��", RED, BLACK);
		    GUI_Show12Char(0, 189, "���͵���λ��", RED, BLACK);
		    GUI_Show12Char(120, 189, "���յ���λ��", RED, BLACK);
		    GUI_Show12Char(0, 210, "���͵���λ��", RED, BLACK);
		    GUI_Show12Char(120, 210, "���յ���λ��", RED, BLACK);
		    GUI_Show12Char(0, 231, "���͵���λ��", RED, BLACK);
		    GUI_Show12Char(120, 231, "���յ���λ��", RED, BLACK);
		    GUI_Show12Char(0, 252, "���͵���λ��", RED, BLACK);
		    GUI_Show12Char(120, 252, "���յ���λ��", RED, BLACK);
		    GUI_Show12Char(0, 273, "���͵ڰ�λ��", RED, BLACK);
		    GUI_Show12Char(120, 273, "���յڰ�λ��", RED, BLACK);
		    GUI_Show12Char(0, 294, "�ϼ����������ּ�һ", BLUE, BLACK);
		    GUI_Show12Char(0, 315, "�¼����������ּ�һ", BLUE, BLACK);
		    GUI_Show12Char(0, 336, "������������������ƶ�һλ", BLUE, BLACK);
		    GUI_Show12Char(0, 357, "�Ҽ�����������", BLUE, BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);
			TouchData.lcdx=0;
			TouchData.lcdy=0;		
		}
		TOUCH_Scan();
		/* �޸�ģʽ */
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
        /* �������ID�ı䣬���������ý���ID */
        if(receiveId != CAN_SendData[9])
        {
            receiveId = CAN_SendData[9];
            if(receiveId != 0x00)
            {
                CAN1_Config16BitFilter(CAN_SendData[9], 0x00);
            }                
        }
		/* ��ʾ���� */
        GUI_DisplayData(m);

		if(k_up==1)	  //�жϰ���k_up�Ƿ���
		{
			delay_ms(10); //��������
			if(k_up==1)	 //�ٴ��жϰ���k_up�Ƿ���
			{
				if(m == 10)
                {
                    CAN_SendData[10] = ~CAN_SendData[10];
                }
                else
                {
                    CAN_SendData[m]++;   
                }
                GUI_Show12Char(175, 84, "�ȴ�����", RED, BLACK);					
			}
			while(k_up); //�ȴ������ɿ�
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
                GUI_Show12Char(175, 84, "�ȴ�����", RED, BLACK);			
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
				CAN1_SendMesg(CAN_SendData[8], 8, CAN_SendData);//��������
                GUI_Show12Char(175, 84, "���ͳɹ�", BLUE, BLACK);			
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

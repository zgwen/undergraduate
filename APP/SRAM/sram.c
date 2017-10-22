#include "sram.h"
#include "key.h"

/****************************************************************************
* Function Name  : SRAM_Config
* Description    : ��ʼ��FSMC
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void SRAM_Config(void)
{  	
	/* ��ʼ������ */
	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTiming;

/*--------------------------------------------------------------------------------*/
/*------------------- GPIO Config ------------------------------------------------*/
/*--------------------------------------------------------------------------------*/	
	/* ��ʱ��ʹ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD  | RCC_APB2Periph_GPIOE
	                      | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG,
						   ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5
	                            | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 
								| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
								
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8
	                            | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
								| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
								
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
	                            | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 
								| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	
    GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 
	                            | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_10; 

	GPIO_Init(GPIOG, &GPIO_InitStructure);

/* ------------------------------------------------------------------------------ */
/* ------------ FSMC Config ----------------------------------------------------- */
/* ------------------------------------------------------------------------------ */
	/* ���ö�дʱ�򣬸�FSMC_NORSRAMInitStructure���� */
	/* ��ַ����ʱ�䣬1��HCLK����, 1/36M = 27ns */
	FSMC_NORSRAMTiming.FSMC_AddressSetupTime = 0x00;

	/* ��ַ����ʱ�䣬1��HCLK���� */
	FSMC_NORSRAMTiming.FSMC_AddressHoldTime = 0x00;

	/* ���ݽ���ʱ�䣬63��HCLK���� 4/72 = 55ns */
	FSMC_NORSRAMTiming.FSMC_DataSetupTime = 0x03;

	/* ���ݱ���ʱ�䣬1��HCLK���� */
	FSMC_NORSRAMTiming.FSMC_DataLatency = 0x00;

	/* ���߻ָ�ʱ������ */
	FSMC_NORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	/* ʱ�ӷ�Ƶ���� */
	FSMC_NORSRAMTiming.FSMC_CLKDivision = 0x00;

	/* ����ģʽ������ڵ�ַ/���ݲ�����ʱ��ABCDģʽ�����𲻴� */
	FSMC_NORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	/* ����FSMC_NORSRAMInitStructure������ */
	/* FSMC���ĸ��洢�飨bank��������ʹ�õ�һ����bank1�� */
	/* ͬʱ����ʹ�õ���bank����ĵ� 3��RAM�� */
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;

	/* ��������ʹ��SRAMģʽ */
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;

	/* ʹ�õ����ݿ��Ϊ16λ */
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	
	// FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;	 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	/* ����дʹ�ܴ� */
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;

	/* ѡ����չģʽʹ�ܣ������ö���д����ͬ��ʱ�� */
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	
	/* ���õ�ַ�����ݸ���ʹ�ܲ��� */
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	
	/* ���ö�дʱ�� */
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTiming;
	
	/* ����дʱ�� */
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTiming;

	/* ��FSMC��ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	 
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

	/*!< Enable FSMC Bank1_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

/****************************************************************************
* Function Name  : SRAM_WriteBuffer
* Description    : ��SRAM����д��һ�����ȵ�����
* Input          : writeBuf��д�뻺��
*                * writeAddr��д����ʼ��ַ
*                * length��д�����ݳ���
* Output         : None
* Return         : None
****************************************************************************/

void SRAM_WriteBuffer(uint16_t *writeBuf, uint32_t writeAddr, uint32_t length)
{
  while(length--) /*!< while there is data to write */
  {
    /*!< Transfer data to the memory */
    *(uint16_t *) (Bank1_SRAM3_ADDR + writeAddr) = *writeBuf++;
    
    /*  ʮ��λ���ȵ��ǵ�ַ+2 */  
    writeAddr += 2;
  }   
}

/****************************************************************************
* Function Name  : SRAM_ReadBuffer
* Description    : ��ȡSRAM����
* Input          : readBuff����ȡ����
*                * readAddr����ȡ��ʼ��ַ
*                * length����ȡ���ݳ���
* Output         : None
* Return         : None
****************************************************************************/

void SRAM_ReadBuffer(uint16_t* readBuff, uint32_t readAddr, uint32_t length)
{
  while(length--) /*!< while there is data to read */
  {
    /*!< Read a half-word from the memory */
    *readBuff++ = *(__IO uint16_t*) (Bank1_SRAM3_ADDR + readAddr);

    /*  ʮ��λ���ȵ��ǵ�ַ+2 */  
    readAddr += 2;
  }  
}


/****************************************************************************
* Function Name  : SRAM_Test
* Description    : ����SRAM
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

u16 SRAM_Test(void)
{
    uint16_t writeData = 0xf0f0, readData, cap = 0;
    uint32_t addr;
    
    /* ÿ��һ��1KBд��һ�����ݣ�Ȼ���ȡ���ݣ��������ͬ����ô������ */
    addr = 1024; //��1KBλ�ÿ�ʼ����
    while(1)
    {
        SRAM_WriteBuffer(&writeData, addr, 1); //д��
        SRAM_ReadBuffer(&readData, addr, 1);   //��ȡ

        /* �鿴��ȡ���������Ƿ��д������һ�� */
        if(readData == writeData)
        {
            cap++;
            addr += 1024;
            readData = 0;
            if(addr > 1024 * 1024) //SRAM�������Ϊ1MB
            {
                break;
            }    
        }
        else
        {
            break;
        }        
    }
	return cap;
}
void SRAM_pros()
{
	uint8_t showData[5]={0,0,0,0,0};
	u16 cap;
	u8 sbuf[15]="www.prechin.com";
	TFT_ClearScreen(BLACK);
	delay_ms(10);
	SRAM_Config();
	while(1)
	{
		if(TouchData.lcdx>95&&TouchData.lcdx<150&&TouchData.lcdy>300&&TouchData.lcdy<355)
		{		
			
			GUI_Show12Char(10, 0, "SRAM ����...", RED, BLACK);
			GUI_Show12Char(10, 18, "����K_up д����", RED, BLACK);
			GUI_Show12Char(10, 36, "����K_down ������", RED, BLACK);
			GUI_Show12Char(10, 64, "SRAM Cap:     KB", RED, BLACK);
			GUI_Show12Char(280,450,"����",YELLOW,BLACK);

			cap=SRAM_Test();
			/* ��ʾ��ȡ�������� */
		    showData[0] = (cap % 10000 /1000) + '0';
		    showData[1] = (cap % 1000 /100) + '0';
		    showData[2] = (cap % 100 /10) + '0';
		    showData[3] = (cap % 10) + '0';
		    GUI_Show12Char(83, 64, showData, RED, BLACK);
			
			TouchData.lcdx=0;
			TouchData.lcdy=0;			
		}	
		TOUCH_Scan();
		
		if(k_up==1)
		{
			delay_ms(10);
			if(k_up==1)	
			{	
				SRAM_WriteBuffer((uint16_t *)sbuf, 2, 15);
				GUI_Show12Char(10, 86, "Write OK!", RED, BLACK);		
			}
			while(k_up);
		}
		
		else if(k_down==0)
		{
			delay_ms(10);
			if(k_down==0)
			{
				GUI_Show12Char(10, 106, "Read OK!", RED, BLACK);
				SRAM_ReadBuffer((uint16_t *)sbuf, 2, 15);
				GUI_Show12Char(10, 126, sbuf, RED, BLACK);
			}
			while(!k_down);			
		}

		if(TouchData.lcdx>280&&TouchData.lcdx<320&&TouchData.lcdy>450&&TouchData.lcdy<480)	
		{
			GUI_DisplayInit();
			break;					
		}
		
	}	
}

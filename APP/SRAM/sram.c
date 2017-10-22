#include "sram.h"
#include "key.h"

/****************************************************************************
* Function Name  : SRAM_Config
* Description    : 初始化FSMC
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void SRAM_Config(void)
{  	
	/* 初始化函数 */
	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTiming;

/*--------------------------------------------------------------------------------*/
/*------------------- GPIO Config ------------------------------------------------*/
/*--------------------------------------------------------------------------------*/	
	/* 打开时钟使能 */
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
	/* 设置读写时序，给FSMC_NORSRAMInitStructure调用 */
	/* 地址建立时间，1个HCLK周期, 1/36M = 27ns */
	FSMC_NORSRAMTiming.FSMC_AddressSetupTime = 0x00;

	/* 地址保持时间，1个HCLK周期 */
	FSMC_NORSRAMTiming.FSMC_AddressHoldTime = 0x00;

	/* 数据建立时间，63个HCLK周期 4/72 = 55ns */
	FSMC_NORSRAMTiming.FSMC_DataSetupTime = 0x03;

	/* 数据保持时间，1个HCLK周期 */
	FSMC_NORSRAMTiming.FSMC_DataLatency = 0x00;

	/* 总线恢复时间设置 */
	FSMC_NORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	/* 时钟分频设置 */
	FSMC_NORSRAMTiming.FSMC_CLKDivision = 0x00;

	/* 设置模式，如果在地址/数据不复用时，ABCD模式都区别不大 */
	FSMC_NORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	/* 设置FSMC_NORSRAMInitStructure的数据 */
	/* FSMC有四个存储块（bank），我们使用第一个（bank1） */
	/* 同时我们使用的是bank里面的第 3个RAM区 */
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;

	/* 这里我们使用SRAM模式 */
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;

	/* 使用的数据宽度为16位 */
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	
	// FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;	 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	/* 设置写使能打开 */
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;

	/* 选择拓展模式使能，即设置读和写用相同的时序 */
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	
	/* 设置地址和数据复用使能不打开 */
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	
	/* 设置读写时序 */
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTiming;
	
	/* 设置写时序 */
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTiming;

	/* 打开FSMC的时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	 
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

	/*!< Enable FSMC Bank1_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

/****************************************************************************
* Function Name  : SRAM_WriteBuffer
* Description    : 向SRAM里面写入一定长度的数据
* Input          : writeBuf：写入缓存
*                * writeAddr：写入起始地址
*                * length：写入数据长度
* Output         : None
* Return         : None
****************************************************************************/

void SRAM_WriteBuffer(uint16_t *writeBuf, uint32_t writeAddr, uint32_t length)
{
  while(length--) /*!< while there is data to write */
  {
    /*!< Transfer data to the memory */
    *(uint16_t *) (Bank1_SRAM3_ADDR + writeAddr) = *writeBuf++;
    
    /*  十六位长度的是地址+2 */  
    writeAddr += 2;
  }   
}

/****************************************************************************
* Function Name  : SRAM_ReadBuffer
* Description    : 读取SRAM数据
* Input          : readBuff：读取缓存
*                * readAddr：读取起始地址
*                * length：读取数据长度
* Output         : None
* Return         : None
****************************************************************************/

void SRAM_ReadBuffer(uint16_t* readBuff, uint32_t readAddr, uint32_t length)
{
  while(length--) /*!< while there is data to read */
  {
    /*!< Read a half-word from the memory */
    *readBuff++ = *(__IO uint16_t*) (Bank1_SRAM3_ADDR + readAddr);

    /*  十六位长度的是地址+2 */  
    readAddr += 2;
  }  
}


/****************************************************************************
* Function Name  : SRAM_Test
* Description    : 测试SRAM
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

u16 SRAM_Test(void)
{
    uint16_t writeData = 0xf0f0, readData, cap = 0;
    uint32_t addr;
    
    /* 每隔一个1KB写入一个数据，然后读取数据，如果不相同，那么就满了 */
    addr = 1024; //从1KB位置开始算起
    while(1)
    {
        SRAM_WriteBuffer(&writeData, addr, 1); //写入
        SRAM_ReadBuffer(&readData, addr, 1);   //读取

        /* 查看读取到的数据是否跟写入数据一样 */
        if(readData == writeData)
        {
            cap++;
            addr += 1024;
            readData = 0;
            if(addr > 1024 * 1024) //SRAM容量最大为1MB
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
			
			GUI_Show12Char(10, 0, "SRAM 测试...", RED, BLACK);
			GUI_Show12Char(10, 18, "按键K_up 写数据", RED, BLACK);
			GUI_Show12Char(10, 36, "按键K_down 读数据", RED, BLACK);
			GUI_Show12Char(10, 64, "SRAM Cap:     KB", RED, BLACK);
			GUI_Show12Char(280,450,"返回",YELLOW,BLACK);

			cap=SRAM_Test();
			/* 显示读取到的容量 */
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

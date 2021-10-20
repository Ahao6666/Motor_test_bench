 #include "adc.h"
 #include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

vu16 ADC_ConvertedValue[Sample_Num][Channel_Num];

void ADC_DMA_Config(void)
{
	 DMA_InitTypeDef DMA_InitStructure;
				   
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	 DMA_DeInit(DMA1_Channel1);												//将DMA的通道1寄存器重设为缺省值
	 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;		//DMA外设ADC基地址
	 DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;		//DMA内存基地址
	 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//内存作为数据传输的目的地
	 DMA_InitStructure.DMA_BufferSize = Sample_Num*Channel_Num;				//DMA通道的DMA缓存的大小
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址不变
	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址递增
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//DMA通道的DMA缓存的大小
	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//DMA通道的DMA缓存的大小
	 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//工作在循环缓存模式
	 DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA通道 x拥有高优先级
	 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;			//DMA通道x没有设置为内存到内存传输
	 DMA_Init(DMA1_Channel1, &DMA_InitStructure);			//根据DMA_InitStruct中指定的参数初始化DMA的通道
	 DMA_Cmd(DMA1_Channel1,ENABLE);
}

		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入引脚
	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	 ADC_DMA_Config();
	   
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	 RCC_ADCCLKConfig(RCC_PCLK2_Div6); 		//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
	
	// ADC_DeInit(ADC1);
	 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						//ADC1和ADC2工作在独立模式
	 ADC_InitStructure.ADC_ScanConvMode = ENABLE;							//多通道
	 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						//连续转换
	 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件启动转换
	 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					//转换结果右对齐
	 ADC_InitStructure.ADC_NbrOfChannel = Channel_Num;						//通道数目
	 ADC_Init(ADC1, &ADC_InitStructure); 
 

	 ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);	//通道，转换次序，转换时间
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);
 
	   
	 ADC_DMACmd(ADC1, ENABLE);	   
	 ADC_Cmd(ADC1, ENABLE);
	 
	 ADC_ResetCalibration(ADC1);					//使能复位校准
	 while(ADC_GetResetCalibrationStatus(ADC1));  	//等待复位校准结束
	 ADC_StartCalibration(ADC1);					//开始AD校准
	 while(ADC_GetCalibrationStatus(ADC1));			//等待校准结束
	 
	 ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能ADC的软件转换启动功能

}

//获得ADC值
//ch:通道值 0~3 
u16 ReadADCAverageValue(u16 Channel)
{
	u8 i;
	u32 sum = 0;
	for(i=0; i<Sample_Num; i++)
	{
		sum+=ADC_ConvertedValue[i][Channel];		//取平均值
	}
	return (sum/Sample_Num);
}



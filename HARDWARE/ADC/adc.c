 #include "adc.h"
 #include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

vu16 ADC_ConvertedValue[Sample_Num][Channel_Num];

void ADC_DMA_Config(void)
{
	 DMA_InitTypeDef DMA_InitStructure;
				   
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	 DMA_DeInit(DMA1_Channel1);												//��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;		//DMA����ADC����ַ
	 DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;		//DMA�ڴ����ַ
	 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//�ڴ���Ϊ���ݴ����Ŀ�ĵ�
	 DMA_InitStructure.DMA_BufferSize = Sample_Num*Channel_Num;				//DMAͨ����DMA����Ĵ�С
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//�����ַ����
	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//�ڴ��ַ����
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//DMAͨ����DMA����Ĵ�С
	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//DMAͨ����DMA����Ĵ�С
	 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//������ѭ������ģʽ
	 DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMAͨ�� xӵ�и����ȼ�
	 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;			//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	 DMA_Init(DMA1_Channel1, &DMA_InitStructure);			//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
	 DMA_Cmd(DMA1_Channel1,ENABLE);
}

		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//ģ����������
	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	 ADC_DMA_Config();
	   
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	 RCC_ADCCLKConfig(RCC_PCLK2_Div6); 		//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
	// ADC_DeInit(ADC1);
	 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						//ADC1��ADC2�����ڶ���ģʽ
	 ADC_InitStructure.ADC_ScanConvMode = ENABLE;							//��ͨ��
	 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						//����ת��
	 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�������ת��
	 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					//ת������Ҷ���
	 ADC_InitStructure.ADC_NbrOfChannel = Channel_Num;						//ͨ����Ŀ
	 ADC_Init(ADC1, &ADC_InitStructure); 
 

	 ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);	//ͨ����ת������ת��ʱ��
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);
 
	   
	 ADC_DMACmd(ADC1, ENABLE);	   
	 ADC_Cmd(ADC1, ENABLE);
	 
	 ADC_ResetCalibration(ADC1);					//ʹ�ܸ�λУ׼
	 while(ADC_GetResetCalibrationStatus(ADC1));  	//�ȴ���λУ׼����
	 ADC_StartCalibration(ADC1);					//��ʼADУ׼
	 while(ADC_GetCalibrationStatus(ADC1));			//�ȴ�У׼����
	 
	 ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ADC�����ת����������

}

//���ADCֵ
//ch:ͨ��ֵ 0~3 
u16 ReadADCAverageValue(u16 Channel)
{
	u8 i;
	u32 sum = 0;
	for(i=0; i<Sample_Num; i++)
	{
		sum+=ADC_ConvertedValue[i][Channel];		//ȡƽ��ֵ
	}
	return (sum/Sample_Num);
}



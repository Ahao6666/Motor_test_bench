#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define Channel_Num  6			//ת��ͨ������Ŀ
#define Sample_Num  10			//ÿ��ͨ��ת���Ĵ���
extern vu16 ADC_ConvertedValue[Sample_Num][Channel_Num];

void Adc_Init(void);
void ADC_DMA_Config(void);

u16 ReadADCAverageValue(u16 Channel);
#endif 

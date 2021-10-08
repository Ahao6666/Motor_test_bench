#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "pwm.h"
#include "MMC_SD.h"
#include "malloc.h"

//ALIENTEK Mini STM32�����巶������15
//ADCʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
u16 SpeedNow  =0;
extern u8  TIM2CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//���벶��ֵ
int main(void)
 {
	u16 adcx[6];
	int i;
	int count=0;
	u16 adcx_tx[6];
	u8 High, Low;
	u16 pwm_val=21600;
	u8 pwm_show;
	float temp_adc;
	u32 temp_speed=0;
	u16 motor_speed=0;
	u8 dir=1;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600	PA9,PA10
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�,PA8,PD2
 	Adc_Init();		  		//ADC��ʼ��	PA123,PC023
	TIM1_PWMout_Init(35999,4);//����Ƶ��PWMƵ��=57600/4/(35999+1)=400hz,����Ϊ2.5ms----PA8
	TIM2_Cap_Init(0XFFFF,576-1);		//��100Khz��Ƶ�ʼ���----PA0----������Ҫ�޸�72-1
	LCD_Init();				//PC6-10,PBA11
	SD_Initialize();
	POINT_COLOR=BLACK;
	LCD_ShowString(30,30,500,16,16,"PWM output:00%");	
	LCD_ShowString(30,50,500,16,16,"Motor Speed:0000r/s");
	for(i=0;i<6;i++){
			//-----------LCD---------
			POINT_COLOR=BLACK;
			LCD_ShowString(10,70+40*i,200,16,16,"ADC:");
			LCD_ShowxNum(40,70+40*i,i,2,16,0);			//��ʾAD�� 
			POINT_COLOR=BLUE;//��������Ϊ��ɫ
			LCD_ShowString(10,90+40*i,100,16,16,"ADC_VAL:");
			POINT_COLOR=RED;//��������Ϊ��ɫ
			LCD_ShowString(120,90+40*i,200,16,16,"ADC_VOL:0.000V");
		}
	//��ʾ��ʾ��Ϣ
	while(1)
	{
		//-----adrc----------
		adcx[0]=Get_Adc_Average(ADC_Channel_1,10);		//Ŀǰ������ʾ0-3.3V��ѹ�仯��Χ
		adcx[1]=Get_Adc_Average(ADC_Channel_2,10);
		adcx[2]=Get_Adc_Average(ADC_Channel_3,10);
		adcx[3]=Get_Adc_Average(ADC_Channel_10,10);
		adcx[4]=Get_Adc_Average(ADC_Channel_12,10);
		adcx[5]=Get_Adc_Average(ADC_Channel_13,10);
		for(i=0;i<6;i++){
			//-----------LCD---------
			adcx_tx[i]=adcx[i];
			POINT_COLOR=BLUE;//��������Ϊ��ɫ
			LCD_ShowxNum(80,90+40*i,adcx[i],4,16,0);	//��ʾADC��������
			POINT_COLOR=RED;//��������Ϊ��ɫ
			temp_adc=(float)adcx[i]*(3.3/4095);
			adcx[i]=temp_adc;
			LCD_ShowxNum(184,90+40*i,adcx[i],1,16,0);//��ʾ��ѹֵ(integer part)
			temp_adc-=adcx[i];
			temp_adc*=1000;
			LCD_ShowxNum(200,90+40*i,temp_adc,3,16,0X80);//��ʾ��ѹֵ(decimal part)
			//----------usart----------
			High = (adcx_tx[i]&0XFF00)>>8;
			Low = adcx_tx[i]&0XFF;
			USART1->DR=i;				//�������
			while((USART1->SR&0X40)==0);
			USART1->DR=High;		//���͸�8λ
			while((USART1->SR&0X40)==0);
			USART1->DR=Low;			//���͵�8λ
			while((USART1->SR&0X40)==0);
		}
		
		
		//--------PWM output----------
		if(count>10){
			if(dir)pwm_val = pwm_val+500;
			else pwm_val = pwm_val-500;
			if(pwm_val>=21600)dir=0;
			if(pwm_val<=10000)dir=1;
			TIM_SetCompare1(TIM1,pwm_val);
			pwm_show=100*(36000-pwm_val)/36000;
			LCD_ShowxNum(118,30,pwm_show,2,16,0);
			count=0;
		}
		count++;
				
		USART1->DR=pwm_show;
		while((USART1->SR&0X40)==0);
		//-----------motor speed---------	 
		if(TIM2CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
		{
			temp_speed=TIM2CH1_CAPTURE_STA&0X3F;
			temp_speed*=65536;					//���ʱ���ܺ�
			temp_speed+=TIM2CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
			motor_speed=1/(temp_speed*0.001);			//����ת�٣���դÿȦ100���㣬ʱ��Ϊus,*0.00001*100
			LCD_ShowxNum(110,50,motor_speed,6,16,0);
 			TIM2CH1_CAPTURE_STA=0;			//������һ�β���
 		}
		
		High = (motor_speed&0XFF00)>>8;
		Low = motor_speed&0XFF;
		USART1->DR=High;		//���͸�8λ
		while((USART1->SR&0X40)==0);
		USART1->DR=Low;			//���͵�8λ
		while((USART1->SR&0X40)==0);
		USART1->DR=0xFF;
		while((USART1->SR&0X40)==0);
	}
}
 

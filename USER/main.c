#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "pwm.h"
#include "MMC_SD.h"
#include "malloc.h"

//ALIENTEK Mini STM32开发板范例代码15
//ADC实验  

extern u8  TIM2CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值

int main(void)
 {
	u16 adcx[6];
	int i;
	int count=0;
	int TIM2_count=0;	//电机转速测速计数
	int total_time=0;
	u16 adcx_tx[6];
	u8 High, Low;
	u16 pwm_val=21600;
	u8 pwm_show;
	float temp_adc;
	u32 temp_speed=0;
	u16 motor_speed=0;
	u8 dir=1;
	delay_init();	    	 		//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);	 			//串口初始化为115200	PA9,PA10
	LED_Init();		  					//初始化与LED连接的硬件接口,PA8,PD2
 	Adc_Init();		  					//ADC初始化	PA145,PC023
	TIM1_PWMout_Init(35999,4);//不分频。PWM频率=72000K/(4+1)/(35999+1)=400hz,周期为2.5ms----PA8
	TIM2_Cap_Init(0XFFFF,72-1);		//光栅电机转速采集，以1Mhz的频率计数----PA0----
	LCD_Init();								//PC6-10,PBA11
	SD_Initialize();
	POINT_COLOR=BLACK;
	LCD_ShowString(30,30,500,16,16,"PWM output:00%");	
	LCD_ShowString(30,50,500,16,16,"Motor Speed:0000r/s");
	for(i=0;i<6;i++){
			//-----------LCD---------
			POINT_COLOR=BLACK;
			LCD_ShowString(10,70+40*i,200,16,16,"ADC:");
			LCD_ShowxNum(40,70+40*i,i,2,16,0);			//显示AD口 
			POINT_COLOR=BLUE;//设置字体为蓝色
			LCD_ShowString(10,90+40*i,100,16,16,"ADC_VAL:");
			POINT_COLOR=RED;//设置字体为红色
			LCD_ShowString(120,90+40*i,200,16,16,"ADC_VOL:0.000V");
		}
	//显示提示信息
	while(1)
	{
		USART1->DR=0x08;				//串口发送数据首位
		while((USART1->SR&0X40)==0);		//等待发送完成
		//-----adc----------
		for(i=0;i<6;i++){
			adcx[i] = ReadADCAverageValue(i);
			//-----------LCD---------
			adcx_tx[i]=adcx[i];
			POINT_COLOR=BLUE;//设置字体为蓝色
			LCD_ShowxNum(80,90+40*i,adcx[i],4,16,0);	//显示ADC的数字量
			POINT_COLOR=RED;//设置字体为红色
			temp_adc=(float)adcx[i]*(3.3/4095);				//ADC数字量转化为模拟量
			adcx[i]=temp_adc;
			LCD_ShowxNum(184,90+40*i,adcx[i],1,16,0);//显示电压值(integer part)
			temp_adc-=adcx[i];
			temp_adc*=1000;
			LCD_ShowxNum(200,90+40*i,temp_adc,3,16,0X80);//显示电压值(decimal part)
			//----------usart----------
			High = (adcx_tx[i]&0XFF00)>>8;
			Low = adcx_tx[i]&0XFF;
			USART1->DR=i;				//发送序号
			while((USART1->SR&0X40)==0);		//等待发送完成
			USART1->DR=High;		//发送高8位
			while((USART1->SR&0X40)==0);
			USART1->DR=Low;			//发送低8位
			while((USART1->SR&0X40)==0);
		}
		
		//--------PWM output----------
		if(count>10){
			if(dir)pwm_val = pwm_val+500;
			else pwm_val = pwm_val-500;
			if(pwm_val>=21600)dir=0;
			if(pwm_val<=15000)dir=1;
			TIM_SetCompare1(TIM1,pwm_val);
			pwm_show=100*(36000-pwm_val)/36000;
			LCD_ShowxNum(118,30,pwm_show,2,16,0);
			count=0;
		}
		count++;
				
		USART1->DR=pwm_show;
		while((USART1->SR&0X40)==0);
		
		//-----------motor speed---------	 
		if(TIM2CH1_CAPTURE_STA&0X80)//成功捕获到了一次高电平
		{
			if(TIM2_count<10){
				temp_speed=TIM2CH1_CAPTURE_STA&0X3F;
				temp_speed*=65536;					//溢出时间总和
				temp_speed+=TIM2CH1_CAPTURE_VAL;		//得到总的高电平时间
				total_time += temp_speed;
				TIM2_count++;
			}
			else{
				motor_speed=10/(total_time*0.0001);			//计算转速，光栅每圈100个点，时间为us,*0.000001*100,total_time为10个脉冲的时间和
				LCD_ShowxNum(110,50,motor_speed,6,16,0);
				TIM2CH1_CAPTURE_STA=0;			//开启下一次捕获
				TIM2_count = 0;
				total_time = 0;
			}
 		}
		LCD_ShowxNum(110,50,motor_speed,6,16,0);
		High = (motor_speed&0XFF00)>>8;
		Low = motor_speed&0XFF;
		USART1->DR=High;		//发送高8位
		while((USART1->SR&0X40)==0);
		USART1->DR=Low;			//发送低8位
		while((USART1->SR&0X40)==0);
		USART1->DR=0xFF;
		while((USART1->SR&0X40)==0);
	}
}
 

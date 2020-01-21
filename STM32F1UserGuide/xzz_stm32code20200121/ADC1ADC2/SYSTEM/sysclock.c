#include "stm32f10x.h"
#include "sysclock.h"
void RCC_configuration(void)
{
	ErrorStatus HSEStarUpstatus;//定义外部晶振是否正常启动的状态标志
	RCC_DeInit(); //复位RCC外部晶振
	RCC_HSEConfig(RCC_HSE_ON);//打开外部HSE高速晶振
	HSEStarUpstatus = RCC_WaitForHSEStartUp();
	if(HSEStarUpstatus == SUCCESS)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)!=SET);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		if(RCC_GetSYSCLKSource()!=0x08);	
	}
}



void delay_us(u32 nus)
{
 u32 temp;
 SysTick->LOAD = 9*nus;
 SysTick->VAL=0X00;
 SysTick->CTRL=0X01;
 do
 {
  temp=SysTick->CTRL;
 }while((temp&0x01)&&(!(temp&(1<<16))));
     SysTick->CTRL=0x00; 
    SysTick->VAL =0X00; 
}
void delay_ms(u16 nms)
{
 u32 temp;
 SysTick->LOAD = 9000*nms;
 SysTick->VAL=0X00;
 SysTick->CTRL=0X01;
 do
 {
  temp=SysTick->CTRL;
 }while((temp&0x01)&&(!(temp&(1<<16))));
    SysTick->CTRL=0x00; 
    SysTick->VAL =0X00; 
}

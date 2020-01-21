#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"


void key_gpio_interrupt_init(void); //PB8 IO口外部输入中断，唤醒__WFI
void RTC_Configuration(void);
void RTC_NVIC_init(void);


int main(void)
{
	uint32_t RTCCounterValue; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);/* 使能电源管理单元的时钟,必须要使能时钟才能进入待机模式 */
	RCC_configuration();//初始化时钟	
	USART_config(115200);//初始化串口
	
	RTC_Configuration();//初始化RTC
	RTC_NVIC_init();//设置RTC中断优先级
	RTC_SetCounter(100);//设置RTC当前时间 填入uint_32的数
	RTC_WaitForLastTask();//等待时间设置完
	
	
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
	
	RTC_WaitForLastTask();//设置闹钟值之前必须等待RTOFF置位
	RTC_SetAlarm(110);//设置RTC闹钟值，RTC秒计数到110就执行闹钟中断
	
	while(1)
	{
		RTCCounterValue = RTC_GetCounter(); //获取实时时钟时间
		printf("RTC sec = %d\r\n",RTCCounterValue);
		delay_ms(1000);
		
		
	}
	return 0;
	
}




void RTC_Configuration(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);//RTC需要打开电源管理和后备电池管理
	PWR_BackupAccessCmd(ENABLE);//打开RTC后备电源供电，这样纽扣电池才能供电,这行代码一定放在RCC_APB1PeriphClockCmd之后执行	
	BKP_DeInit();//备份区域复位
	
	RCC_LSEConfig(RCC_LSE_ON);//开启LSE晶振
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//等待LSE晶振开启
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择LSE时钟为RTC时钟源
	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
	
	RTC_WaitForSynchro();//等待RTC时钟和APB1总线时钟同步
	RTC_WaitForLastTask();//为了安全起见，等待对RTC寄存器操作完成
	
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//开启RTC秒中断
	RTC_WaitForLastTask();//等待秒中断开启完成
	RTC_ITConfig(RTC_IT_ALR, ENABLE);//开启闹钟中断
	RTC_WaitForLastTask();//为了安全起见，等待秒中断对RTC寄存器操作完成
	
	RTC_SetPrescaler(32767);//分频频率为(32.768khz)/(32767+1)=1Hz ，(32768hz)/(32767+1)=1Hz，所以RTC 1hz计数一次
							//为什么写32767，这是因为RTC_SetPrescaler函数内部会自动+1，然后写入分频寄存器
							//32768分频还有个原因是外部晶振是32768hz，如果你想计数快点，你可以修改外部晶振或者分频系数
	
	RTC_WaitForLastTask();//为了安全起见，等待分频寄存器操作完成
		
}


void RTC_NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//设置中断组
	/*秒中断*/
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;  //RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //使能该通道中断
	NVIC_Init(&NVIC_InitStructure);  

	/*加入闹钟中断*/
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;  //闹钟中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //比RTC全局中断的优先级高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
}



void key_gpio_interrupt_init(void) //PB8 IO口外部输入中断，唤醒__WFI
{
	GPIO_InitTypeDef GPIO_initStructure;
	EXTI_InitTypeDef EXTI_initStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_initStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
		
	EXTI_ClearITPendingBit(EXTI_Line8);
	
	EXTI_initStructure.EXTI_Line = EXTI_Line8;
	EXTI_initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_initStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_initStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_initStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
	
	
	
	
	
	
}


void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearFlag(EXTI_Line8);
	EXTI_ClearITPendingBit(EXTI_Line8);
}


void RTC_IRQHandler(void) {
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);
		printf("sec interrupt \r\n");
       
    }
	
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
    {
		printf("alarm interrupt \r\n");
        RTC_ClearITPendingBit(RTC_IT_ALR);//清闹钟中断    
			
	}   

	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);//清除闹钟中断之后还有清除秒中断
    RTC_WaitForLastTask();              
}










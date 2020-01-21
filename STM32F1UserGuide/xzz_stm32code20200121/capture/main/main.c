#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

uint16_t Capture_Period = 0;
uint16_t Capture_CcrValue = 0;
uint8_t Capture_StartFlag = 0;
uint8_t Capture_FinishFlag = 0;


void Input_GPIO_Init(void);
void TIM3_Mode_Init(void);
void TIM3_NVIC_Init();

int main(void)
{
	uint32_t time;
	
	uint32_t TIM_PscCLK = 72000000 / (72);//计数器时钟是1M，也就是1us计数一次
	
	RCC_configuration();//初始化时钟	
	USART_config(115200);//初始化串口
	
	Input_GPIO_Init();//PA6输入捕获IO口初始化
	TIM3_Mode_Init();//定时器和输入捕获通道CH1初始化
	TIM3_NVIC_Init();//TIM3中断优先级
		
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
		
	while(1)
	{
		if (Capture_FinishFlag == 1) {
			// 计算高电平时间的计数器的值
			time = Capture_Period * (0xffff+1) +(Capture_CcrValue+1);
			Capture_FinishFlag = 0;	
			
			printf ( " hight level time length: %d.%d s\r\n",time/TIM_PscCLK,time%TIM_PscCLK );		
		}			
	}
	return 0;
	
}



void Input_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void TIM3_Mode_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/*--------------------输入捕获结构体初始化-------------------*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);// 2～7定时器都挂在APB1总线,开启定时器时钟,即内部时钟 CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=0xffff;// 自动重装载寄存器的值，累计 TIM_Period+1 个频率后产生一个更新或者中断
	
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);// 驱动 CNT 计数器的时钟 = Fck_int/(psc+1)
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// 时钟分频因子 ，配置死区时间时需要用到,不使用死区时间也要保留代码
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// 计数器计数模式，设置为向上计数
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// 重复计数器的值，没用到不用管
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);// 初始化定时器3
	
	
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;// 配置输入捕获的通道，需要根据具体的 GPIO 来配置
 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;// 输入捕获信号的极性配置
 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// 输入通道和捕获通道的映射关系，有直连和非直连两种
 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;// 输入的需要被捕获的信号的分频系数
 
	TIM_ICInitStructure.TIM_ICFilter = 0;// 输入的需要被捕获的信号的滤波系数
 
	TIM_ICInit(TIM3, &TIM_ICInitStructure);// 定时器输入捕获初始化

 
	TIM_ClearFlag(TIM3, TIM_FLAG_Update|TIM_IT_CC1);// 清除更新和捕获中断标志位
 
	TIM_ITConfig (TIM3, TIM_IT_Update | TIM_IT_CC1, ENABLE );// 开启更新和捕获中断

	TIM_Cmd(TIM3, ENABLE);// 使能计数器
}


void TIM3_NVIC_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 设置中断组为 0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;
	// 设置主优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// 设置抢占优先级为 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



void TIM3_IRQHandler(void)
{
	// 当要被捕获的信号的周期大于定时器的最长定时时，定时器就会溢出，产生更新中断
	// 这个时候我们需要把这个最长的定时周期加到捕获信号的时间里面去
	if ( TIM_GetITStatus ( TIM3, TIM_IT_Update) != RESET ) {
		Capture_Period ++;
		TIM_ClearITPendingBit ( TIM3, TIM_FLAG_Update );
	}

	// 上升沿捕获中断
	if ( TIM_GetITStatus (TIM3, TIM_IT_CC1 ) != RESET) {
		// 第一次捕获
		if ( Capture_StartFlag == 0 ) {
			// 计数器清 0
			TIM_SetCounter ( TIM3, 0 );
			// 自动重装载寄存器更新标志清 0
			Capture_Period = 0;
			// 存捕获比较寄存器的值的变量的值清 0
			Capture_CcrValue = 0;

			// 当第一次捕获到上升沿之后，就把捕获边沿配置为下降沿
			TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Falling);
			// 开始捕获标准置 1
			Capture_StartFlag = 1;
		}
			// 下降沿捕获中断
		else { // 第二次捕获
				// 获取捕获比较寄存器的值，这个值就是捕获到的高电平的时间的值
				Capture_CcrValue = TIM_GetCapture1(TIM3);

				// 当第二次捕获到下降沿之后，就把捕获边沿配置为上升沿，好开启新的一轮捕获
				TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Rising);
				// 开始捕获标志清 0
				Capture_StartFlag = 0;
				// 捕获完成标志置 1
				Capture_FinishFlag = 1;
			}

			TIM_ClearITPendingBit (TIM3,TIM_IT_CC1);
	}
}





static void SetSysClockTo72(void)
{
 __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

 // ① 使能 HSE，并等待 HSE 稳定
 RCC->CR |= ((uint32_t)RCC_CR_HSEON);

 // 等待 HSE 启动稳定，并做超时处理
 do {
 HSEStatus = RCC->CR & RCC_CR_HSERDY;
 StartUpCounter++;
 } while ((HSEStatus == 0)
&&(StartUpCounter !=HSE_STARTUP_TIMEOUT));

 if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
 HSEStatus = (uint32_t)0x01;
 } else {
 HSEStatus = (uint32_t)0x00;
 }
 // HSE 启动成功，则继续往下处理
 if (HSEStatus == (uint32_t)0x01) {

 //-----------------------------------------------------------
 // 使能 FLASH 预存取缓冲区 */
 FLASH->ACR |= FLASH_ACR_PRFTBE;

 // SYSCLK 周期与闪存访问时间的比例设置，这里统一设置成 2
 // 设置成 2 的时候， SYSCLK 低于 48M 也可以工作，如果设置成 0 或者 1 的时候，
 // 如果配置的 SYSCLK 超出了范围的话，则会进入硬件错误，程序就死了
 // 0： 0 < SYSCLK <= 24M
 // 1： 24< SYSCLK <= 48M
 // 2： 48< SYSCLK <= 72M */
 FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
 FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
 //------------------------------------------------------------


 // ② 设置 AHB、 APB2、 APB1 预分频因子
 // HCLK = SYSCLK
 RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
 //PCLK2 = HCLK
 RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
 //PCLK1 = HCLK/2
 RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

 //③ 设置 PLL 时钟来源，设置 PLL 倍频因子， PLLCLK = HSE * 9 = 72 MHz
 RCC->CFGR &= (uint32_t)((uint32_t)
 ~(RCC_CFGR_PLLSRC
 | RCC_CFGR_PLLXTPRE
 | RCC_CFGR_PLLMULL));
 RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE
 | RCC_CFGR_PLLMULL9);

 // ④ 使能 PLL
 RCC->CR |= RCC_CR_PLLON;

 // ⑤ 等待 PLL 稳定
 while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
 }

 // ⑥ 选择 PLL 作为系统时钟来源
 RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
 RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

 // ⑦ 读取时钟切换状态位，确保 PLLCLK 被选为系统时钟
 while ((RCC->CFGR&(uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08){
 }
 } else {// 如果 HSE 启动失败，用户可以在这里添加错误代码出来
 }
 }













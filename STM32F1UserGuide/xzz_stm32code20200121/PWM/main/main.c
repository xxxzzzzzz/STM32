#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

static void PWM_IO_Init(void);
static void TIM1_PWM_Mode(void);


int main(void)
{
	
	RCC_configuration();//初始化时钟	
	USART_config(115200);//初始化串口
	
	PWM_IO_Init();//PB8,PB13,PB12 PWM功能引脚初始化
	TIM1_PWM_Mode();//启动PWM PB8 PB13输出PWM信号
	
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
		
	while(1)
	{
				
	}
	return 0;
	
}



static void PWM_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 输出比较通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使用PA8输出PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 输出比较通道互补通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使用PB13输出互补的PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 输出比较通道刹车通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// BKIN 引脚默认先输出低电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}



static void TIM1_PWM_Mode(void)
{
	/*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/*--------------------输出比较结构体初始化-------------------*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/*-------------------刹车和死区结构体初始化-------------------*/
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);// 开启定时器时钟,即内部时钟 CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=(8-1);// 自动重装载寄存器的值，累计 TIM_Period+1 个频率后产生一个更新或者中断
	
	TIM_TimeBaseStructure.TIM_Prescaler= (9-1);// 驱动 CNT 计数器的时钟 = Fck_int/(psc+1)
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// 时钟分频因子 ，配置死区时间时需要用到,不使用死区时间也要保留代码
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// 计数器计数模式，设置为向上计数
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// 重复计数器的值，没用到不用管
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);// 初始化定时器

	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 配置为 PWM 模式 1
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// PA8输出使能
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;// PB13互补输出使能
	
	TIM_OCInitStructure.TIM_Pulse = 4;// 设置占空比大小
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// 输出通道电平极性配置
	
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;// 互补输出通道电平极性配置
	
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;// 输出通道空闲电平极性配置
	
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;// 互补输出通道空闲电平极性配置
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	
	// 有关刹车和死区结构体的成员具体可参考 BDTR 寄存器的描述
	
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	// 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
	// 这里配置的死区时间为 152ns
	TIM_BDTRInitStructure.TIM_DeadTime = 11;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	// 当 BKIN 引脚检测到高电平的时候，输出比较信号被禁止，就好像是刹车一样
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

	// 使能计数器
	TIM_Cmd(TIM1, ENABLE);
	// 主输出使能，当使用的是通用定时器时，这句不需要
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
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













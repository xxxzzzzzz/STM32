#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

uint16_t Capture_Period = 0;
uint16_t Capture_CcrValue = 0;
uint8_t Capture_StartFlag = 0;
uint8_t Capture_FinishFlag = 0;


void Input_GPIO_Init(void);
void TIM1_Mode_Init(void);
void TIM1_NVIC_Init();

static void PWM_IO_Init(void);
static void TIM3_PWM_Mode(void);



int main(void)
{
	
	
	RCC_configuration();//初始化时钟	
	USART_config(115200);//初始化串口
	
	
	PWM_IO_Init();//PWM IO口初始化
	TIM3_PWM_Mode();//PWM定时器初始化
	
	Input_GPIO_Init();//PA8输入捕获IO口初始化
	TIM1_Mode_Init();//定时器1和输入捕获通道CH1初始化
	TIM1_NVIC_Init();//TIM1中断优先级
	
		
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
		
	while(1)
	{
				
	}
	return 0;
	
}



void Input_GPIO_Init(void)//PA8作为输入捕获引脚
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void TIM1_Mode_Init(void)//TIM1高级定时器设置输入捕获功能
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/*--------------------输入捕获结构体初始化-------------------*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);// 定时器1挂在APB2总线,开启定时器时钟,即内部时钟 CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=(1000-1);// 自动重装载寄存器是计数1000us才溢出，也就是1ms溢出
	//这样周期小于1ms的PWM信号都能捕获比如100khz方波
	
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);// 计数器是72M/72=1M的时钟，也就是1us计数一次
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// 时钟分频因子 ，配置死区时间时需要用到,不使用死区时间也要保留代码
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// 计数器计数模式，设置为向上计数
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// 重复计数器的值，没用到不用管
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);// 初始化定时器1
	
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;// 配置输入捕获的通道，需要根据具体的 GPIO 来配置
 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;// 输入捕获信号的极性配置，上升沿开始捕获
 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// 输入通道和捕获通道的映射关系，有直连和非直连两种
 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;// 输入的需要被捕获的信号的分频系数
 
	TIM_ICInitStructure.TIM_ICFilter = 0;// 输入的需要被捕获的信号的滤波系数
 
	TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);// 定时器输入捕获初始化改为PWM输入捕获初始化模式

 
	TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);//输入捕获触发信号通道选择，就是上升沿来了开始触发，计算为周期
	TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);//选择从模式: 复位模式
	
	// PWM 输入模式时,从模式必须工作在复位模式，当捕获开始时,计数器 CNT 会被复位
	TIM_SelectMasterSlaveMode(TIM1,TIM_MasterSlaveMode_Enable);
		
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
 
	TIM_ITConfig (TIM1, TIM_IT_Update | TIM_IT_CC1, ENABLE );// 开启更新和捕获中断
		
	TIM_Cmd(TIM1, ENABLE);// 使能计数器
}


void TIM1_NVIC_Init()//TIM3中断优先级
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 设置中断组为 0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn ;
	// 设置主优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// 设置抢占优先级为 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



uint16_t IC1Value = 0 ,IC2Value = 0;
float DutyCycle = 0;
float Frequency = 0;

void TIM1_CC_IRQHandler(void)
{
	/* 清除中断标志位 */
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
	
	/* 获取输入捕获值 */
	IC1Value = TIM_GetCapture1(TIM1);
	IC2Value = TIM_GetCapture2(TIM1);
	
	// 注意：捕获寄存器 CCR1 和 CCR2 的值在计算占空比和频率的时候必须加 1	
	if (IC1Value != 0) {
		
		/* 占空比计算 */
		DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);
		
		/* 频率计算 */
		Frequency = (72000000/9)/(float)(IC1Value+1);
		printf("DutyCycle: %0.2f%% Frequency: %0.2fHz\r\n",DutyCycle,Frequency);
		
	} 
	else {
		DutyCycle = 0;
		Frequency = 0;
	}
			
}


static void PWM_IO_Init(void)//PA6 PWM信号输出引脚
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 输出比较通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); //使用PA6输出PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);

}



static void TIM3_PWM_Mode(void) //TIM3设置 PWM输出100Khz方波，占空比50%
{
	/*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/*--------------------输出比较结构体初始化-------------------*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);// 开启定时器时钟,记住TIM3是在APB1总线,即内部时钟 CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=(10-1);// 自动重装载寄存器的值，改成100Khz PWM输出
	
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);// 驱动 CNT 计数器的时钟 = Fck_int/(psc+1) 改成100Khz PWM输出
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// 时钟分频因子 ，配置死区时间时需要用到,不使用死区时间也要保留代码
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// 计数器计数模式，设置为向上计数
	
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// 重复计数器的值，没用到不用管
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);// 初始化定时器
	
//	TIM_ClearITPendingBit(TIM3,  TIM_IT_Update);
//	TIM_ITConfig(TIM3,  TIM_IT_Update, ENABLE);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 配置为 PWM 模式 1
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// PA6输出使能
	
	
	TIM_OCInitStructure.TIM_Pulse = 5;// 设置占空比大小
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// 输出通道电平极性配置
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);        //使能预装载寄存器
//	
//	TIM_CtrlPWMOutputs(TIM3,ENABLE); 
//	TIM_ARRPreloadConfig(TIM3, ENABLE);
	// 使能计数器
	TIM_Cmd(TIM3, ENABLE);
	// 主输出使能，当使用的是通用定时器时，这句不需要
	//TIM_CtrlPWMOutputs(TIM3, ENABLE);
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













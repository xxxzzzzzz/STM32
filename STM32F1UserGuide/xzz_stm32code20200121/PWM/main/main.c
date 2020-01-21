#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

static void PWM_IO_Init(void);
static void TIM1_PWM_Mode(void);


int main(void)
{
	
	RCC_configuration();//��ʼ��ʱ��	
	USART_config(115200);//��ʼ������
	
	PWM_IO_Init();//PB8,PB13,PB12 PWM�������ų�ʼ��
	TIM1_PWM_Mode();//����PWM PB8 PB13���PWM�ź�
	
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

	// ����Ƚ�ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PA8���PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// ����Ƚ�ͨ������ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PB13���������PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ����Ƚ�ͨ��ɲ��ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// BKIN ����Ĭ��������͵�ƽ
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}



static void TIM1_PWM_Mode(void)
{
	/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/*-------------------ɲ���������ṹ���ʼ��-------------------*/
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);// ������ʱ��ʱ��,���ڲ�ʱ�� CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=(8-1);// �Զ���װ�ؼĴ�����ֵ���ۼ� TIM_Period+1 ��Ƶ�ʺ����һ�����»����ж�
	
	TIM_TimeBaseStructure.TIM_Prescaler= (9-1);// ���� CNT ��������ʱ�� = Fck_int/(psc+1)
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�,��ʹ������ʱ��ҲҪ��������
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// ����������ģʽ������Ϊ���ϼ���
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// �ظ���������ֵ��û�õ����ù�
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);// ��ʼ����ʱ��

	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// ����Ϊ PWM ģʽ 1
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// PA8���ʹ��
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;// PB13�������ʹ��
	
	TIM_OCInitStructure.TIM_Pulse = 4;// ����ռ�ձȴ�С
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// ���ͨ����ƽ��������
	
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;// �������ͨ����ƽ��������
	
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;// ���ͨ�����е�ƽ��������
	
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;// �������ͨ�����е�ƽ��������
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	
	// �й�ɲ���������ṹ��ĳ�Ա����ɲο� BDTR �Ĵ���������
	
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	// ����Ƚ��ź�����ʱ�����ã�������μ���ɲο� BDTR:UTG[7:0]������
	// �������õ�����ʱ��Ϊ 152ns
	TIM_BDTRInitStructure.TIM_DeadTime = 11;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	// �� BKIN ���ż�⵽�ߵ�ƽ��ʱ������Ƚ��źű���ֹ���ͺ�����ɲ��һ��
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

	// ʹ�ܼ�����
	TIM_Cmd(TIM1, ENABLE);
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}



static void SetSysClockTo72(void)
{
 __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

 // �� ʹ�� HSE�����ȴ� HSE �ȶ�
 RCC->CR |= ((uint32_t)RCC_CR_HSEON);

 // �ȴ� HSE �����ȶ���������ʱ����
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
 // HSE �����ɹ�����������´���
 if (HSEStatus == (uint32_t)0x01) {

 //-----------------------------------------------------------
 // ʹ�� FLASH Ԥ��ȡ������ */
 FLASH->ACR |= FLASH_ACR_PRFTBE;

 // SYSCLK �������������ʱ��ı������ã�����ͳһ���ó� 2
 // ���ó� 2 ��ʱ�� SYSCLK ���� 48M Ҳ���Թ�����������ó� 0 ���� 1 ��ʱ��
 // ������õ� SYSCLK �����˷�Χ�Ļ���������Ӳ�����󣬳��������
 // 0�� 0 < SYSCLK <= 24M
 // 1�� 24< SYSCLK <= 48M
 // 2�� 48< SYSCLK <= 72M */
 FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
 FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
 //------------------------------------------------------------


 // �� ���� AHB�� APB2�� APB1 Ԥ��Ƶ����
 // HCLK = SYSCLK
 RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
 //PCLK2 = HCLK
 RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
 //PCLK1 = HCLK/2
 RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

 //�� ���� PLL ʱ����Դ������ PLL ��Ƶ���ӣ� PLLCLK = HSE * 9 = 72 MHz
 RCC->CFGR &= (uint32_t)((uint32_t)
 ~(RCC_CFGR_PLLSRC
 | RCC_CFGR_PLLXTPRE
 | RCC_CFGR_PLLMULL));
 RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE
 | RCC_CFGR_PLLMULL9);

 // �� ʹ�� PLL
 RCC->CR |= RCC_CR_PLLON;

 // �� �ȴ� PLL �ȶ�
 while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
 }

 // �� ѡ�� PLL ��Ϊϵͳʱ����Դ
 RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
 RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

 // �� ��ȡʱ���л�״̬λ��ȷ�� PLLCLK ��ѡΪϵͳʱ��
 while ((RCC->CFGR&(uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08){
 }
 } else {// ��� HSE ����ʧ�ܣ��û�������������Ӵ���������
 }
 }













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
	
	
	RCC_configuration();//��ʼ��ʱ��	
	USART_config(115200);//��ʼ������
	
	
	PWM_IO_Init();//PWM IO�ڳ�ʼ��
	TIM3_PWM_Mode();//PWM��ʱ����ʼ��
	
	Input_GPIO_Init();//PA8���벶��IO�ڳ�ʼ��
	TIM1_Mode_Init();//��ʱ��1�����벶��ͨ��CH1��ʼ��
	TIM1_NVIC_Init();//TIM1�ж����ȼ�
	
		
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
		
	while(1)
	{
				
	}
	return 0;
	
}



void Input_GPIO_Init(void)//PA8��Ϊ���벶������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void TIM1_Mode_Init(void)//TIM1�߼���ʱ���������벶����
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/*--------------------���벶��ṹ���ʼ��-------------------*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);// ��ʱ��1����APB2����,������ʱ��ʱ��,���ڲ�ʱ�� CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=(1000-1);// �Զ���װ�ؼĴ����Ǽ���1000us�������Ҳ����1ms���
	//��������С��1ms��PWM�źŶ��ܲ������100khz����
	
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);// ��������72M/72=1M��ʱ�ӣ�Ҳ����1us����һ��
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�,��ʹ������ʱ��ҲҪ��������
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// ����������ģʽ������Ϊ���ϼ���
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// �ظ���������ֵ��û�õ����ù�
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);// ��ʼ����ʱ��1
	
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;// �������벶���ͨ������Ҫ���ݾ���� GPIO ������
 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;// ���벶���źŵļ������ã������ؿ�ʼ����
 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// ����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;// �������Ҫ��������źŵķ�Ƶϵ��
 
	TIM_ICInitStructure.TIM_ICFilter = 0;// �������Ҫ��������źŵ��˲�ϵ��
 
	TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);// ��ʱ�����벶���ʼ����ΪPWM���벶���ʼ��ģʽ

 
	TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);//���벶�񴥷��ź�ͨ��ѡ�񣬾������������˿�ʼ����������Ϊ����
	TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);//ѡ���ģʽ: ��λģʽ
	
	// PWM ����ģʽʱ,��ģʽ���빤���ڸ�λģʽ��������ʼʱ,������ CNT �ᱻ��λ
	TIM_SelectMasterSlaveMode(TIM1,TIM_MasterSlaveMode_Enable);
		
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
 
	TIM_ITConfig (TIM1, TIM_IT_Update | TIM_IT_CC1, ENABLE );// �������ºͲ����ж�
		
	TIM_Cmd(TIM1, ENABLE);// ʹ�ܼ�����
}


void TIM1_NVIC_Init()//TIM3�ж����ȼ�
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// �����ж���Ϊ 0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn ;
	// ���������ȼ�Ϊ 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// ������ռ���ȼ�Ϊ 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



uint16_t IC1Value = 0 ,IC2Value = 0;
float DutyCycle = 0;
float Frequency = 0;

void TIM1_CC_IRQHandler(void)
{
	/* ����жϱ�־λ */
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
	
	/* ��ȡ���벶��ֵ */
	IC1Value = TIM_GetCapture1(TIM1);
	IC2Value = TIM_GetCapture2(TIM1);
	
	// ע�⣺����Ĵ��� CCR1 �� CCR2 ��ֵ�ڼ���ռ�ձȺ�Ƶ�ʵ�ʱ������ 1	
	if (IC1Value != 0) {
		
		/* ռ�ձȼ��� */
		DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);
		
		/* Ƶ�ʼ��� */
		Frequency = (72000000/9)/(float)(IC1Value+1);
		printf("DutyCycle: %0.2f%% Frequency: %0.2fHz\r\n",DutyCycle,Frequency);
		
	} 
	else {
		DutyCycle = 0;
		Frequency = 0;
	}
			
}


static void PWM_IO_Init(void)//PA6 PWM�ź��������
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// ����Ƚ�ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); //ʹ��PA6���PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);

}



static void TIM3_PWM_Mode(void) //TIM3���� PWM���100Khz������ռ�ձ�50%
{
	/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);// ������ʱ��ʱ��,��סTIM3����APB1����,���ڲ�ʱ�� CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=(10-1);// �Զ���װ�ؼĴ�����ֵ���ĳ�100Khz PWM���
	
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);// ���� CNT ��������ʱ�� = Fck_int/(psc+1) �ĳ�100Khz PWM���
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�,��ʹ������ʱ��ҲҪ��������
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// ����������ģʽ������Ϊ���ϼ���
	
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// �ظ���������ֵ��û�õ����ù�
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);// ��ʼ����ʱ��
	
//	TIM_ClearITPendingBit(TIM3,  TIM_IT_Update);
//	TIM_ITConfig(TIM3,  TIM_IT_Update, ENABLE);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// ����Ϊ PWM ģʽ 1
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// PA6���ʹ��
	
	
	TIM_OCInitStructure.TIM_Pulse = 5;// ����ռ�ձȴ�С
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// ���ͨ����ƽ��������
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);        //ʹ��Ԥװ�ؼĴ���
//	
//	TIM_CtrlPWMOutputs(TIM3,ENABLE); 
//	TIM_ARRPreloadConfig(TIM3, ENABLE);
	// ʹ�ܼ�����
	TIM_Cmd(TIM3, ENABLE);
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
	//TIM_CtrlPWMOutputs(TIM3, ENABLE);
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













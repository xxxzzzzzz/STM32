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
	
	uint32_t TIM_PscCLK = 72000000 / (72);//������ʱ����1M��Ҳ����1us����һ��
	
	RCC_configuration();//��ʼ��ʱ��	
	USART_config(115200);//��ʼ������
	
	Input_GPIO_Init();//PA6���벶��IO�ڳ�ʼ��
	TIM3_Mode_Init();//��ʱ�������벶��ͨ��CH1��ʼ��
	TIM3_NVIC_Init();//TIM3�ж����ȼ�
		
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
		
	while(1)
	{
		if (Capture_FinishFlag == 1) {
			// ����ߵ�ƽʱ��ļ�������ֵ
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

	/*--------------------���벶��ṹ���ʼ��-------------------*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);// 2��7��ʱ��������APB1����,������ʱ��ʱ��,���ڲ�ʱ�� CK_INT=72M
	
	TIM_TimeBaseStructure.TIM_Period=0xffff;// �Զ���װ�ؼĴ�����ֵ���ۼ� TIM_Period+1 ��Ƶ�ʺ����һ�����»����ж�
	
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);// ���� CNT ��������ʱ�� = Fck_int/(psc+1)
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�,��ʹ������ʱ��ҲҪ��������
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;// ����������ģʽ������Ϊ���ϼ���
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// �ظ���������ֵ��û�õ����ù�
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);// ��ʼ����ʱ��3
	
	
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;// �������벶���ͨ������Ҫ���ݾ���� GPIO ������
 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;// ���벶���źŵļ�������
 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// ����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;// �������Ҫ��������źŵķ�Ƶϵ��
 
	TIM_ICInitStructure.TIM_ICFilter = 0;// �������Ҫ��������źŵ��˲�ϵ��
 
	TIM_ICInit(TIM3, &TIM_ICInitStructure);// ��ʱ�����벶���ʼ��

 
	TIM_ClearFlag(TIM3, TIM_FLAG_Update|TIM_IT_CC1);// ������ºͲ����жϱ�־λ
 
	TIM_ITConfig (TIM3, TIM_IT_Update | TIM_IT_CC1, ENABLE );// �������ºͲ����ж�

	TIM_Cmd(TIM3, ENABLE);// ʹ�ܼ�����
}


void TIM3_NVIC_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// �����ж���Ϊ 0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;
	// ���������ȼ�Ϊ 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// ������ռ���ȼ�Ϊ 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



void TIM3_IRQHandler(void)
{
	// ��Ҫ��������źŵ����ڴ��ڶ�ʱ�������ʱʱ����ʱ���ͻ���������������ж�
	// ���ʱ��������Ҫ�������Ķ�ʱ���ڼӵ������źŵ�ʱ������ȥ
	if ( TIM_GetITStatus ( TIM3, TIM_IT_Update) != RESET ) {
		Capture_Period ++;
		TIM_ClearITPendingBit ( TIM3, TIM_FLAG_Update );
	}

	// �����ز����ж�
	if ( TIM_GetITStatus (TIM3, TIM_IT_CC1 ) != RESET) {
		// ��һ�β���
		if ( Capture_StartFlag == 0 ) {
			// �������� 0
			TIM_SetCounter ( TIM3, 0 );
			// �Զ���װ�ؼĴ������±�־�� 0
			Capture_Period = 0;
			// �沶��ȽϼĴ�����ֵ�ı�����ֵ�� 0
			Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Falling);
			// ��ʼ�����׼�� 1
			Capture_StartFlag = 1;
		}
			// �½��ز����ж�
		else { // �ڶ��β���
				// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
				Capture_CcrValue = TIM_GetCapture1(TIM3);

				// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
				TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Rising);
				// ��ʼ�����־�� 0
				Capture_StartFlag = 0;
				// ������ɱ�־�� 1
				Capture_FinishFlag = 1;
			}

			TIM_ClearITPendingBit (TIM3,TIM_IT_CC1);
	}
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













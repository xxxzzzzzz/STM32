#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"


void key_gpio_interrupt_init(void); //PB8 IO���ⲿ�����жϣ�����__WFI
void RTC_Configuration(void);
void RTC_NVIC_init(void);


int main(void)
{
	uint32_t RTCCounterValue; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);/* ʹ�ܵ�Դ����Ԫ��ʱ��,����Ҫʹ��ʱ�Ӳ��ܽ������ģʽ */
	RCC_configuration();//��ʼ��ʱ��	
	USART_config(115200);//��ʼ������
	
	RTC_Configuration();//��ʼ��RTC
	RTC_NVIC_init();//����RTC�ж����ȼ�
	RTC_SetCounter(100);//����RTC��ǰʱ�� ����uint_32����
	RTC_WaitForLastTask();//�ȴ�ʱ��������
	
	
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
	
	RTC_WaitForLastTask();//��������ֵ֮ǰ����ȴ�RTOFF��λ
	RTC_SetAlarm(110);//����RTC����ֵ��RTC�������110��ִ�������ж�
	
	while(1)
	{
		RTCCounterValue = RTC_GetCounter(); //��ȡʵʱʱ��ʱ��
		printf("RTC sec = %d\r\n",RTCCounterValue);
		delay_ms(1000);
		
		
	}
	return 0;
	
}




void RTC_Configuration(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);//RTC��Ҫ�򿪵�Դ����ͺ󱸵�ع���
	PWR_BackupAccessCmd(ENABLE);//��RTC�󱸵�Դ���磬����Ŧ�۵�ز��ܹ���,���д���һ������RCC_APB1PeriphClockCmd֮��ִ��	
	BKP_DeInit();//��������λ
	
	RCC_LSEConfig(RCC_LSE_ON);//����LSE����
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//�ȴ�LSE������
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//ѡ��LSEʱ��ΪRTCʱ��Դ
	RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��
	
	RTC_WaitForSynchro();//�ȴ�RTCʱ�Ӻ�APB1����ʱ��ͬ��
	RTC_WaitForLastTask();//Ϊ�˰�ȫ������ȴ���RTC�Ĵ����������
	
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//����RTC���ж�
	RTC_WaitForLastTask();//�ȴ����жϿ������
	RTC_ITConfig(RTC_IT_ALR, ENABLE);//���������ж�
	RTC_WaitForLastTask();//Ϊ�˰�ȫ������ȴ����ж϶�RTC�Ĵ����������
	
	RTC_SetPrescaler(32767);//��ƵƵ��Ϊ(32.768khz)/(32767+1)=1Hz ��(32768hz)/(32767+1)=1Hz������RTC 1hz����һ��
							//Ϊʲôд32767��������ΪRTC_SetPrescaler�����ڲ����Զ�+1��Ȼ��д���Ƶ�Ĵ���
							//32768��Ƶ���и�ԭ�����ⲿ������32768hz��������������㣬������޸��ⲿ������߷�Ƶϵ��
	
	RTC_WaitForLastTask();//Ϊ�˰�ȫ������ȴ���Ƶ�Ĵ����������
		
}


void RTC_NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�����ж���
	/*���ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;  //RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);  

	/*���������ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;  //�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��RTCȫ���жϵ����ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
}



void key_gpio_interrupt_init(void) //PB8 IO���ⲿ�����жϣ�����__WFI
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
	
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����ж�
    {
		printf("alarm interrupt \r\n");
        RTC_ClearITPendingBit(RTC_IT_ALR);//�������ж�    
			
	}   

	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);//��������ж�֮����������ж�
    RTC_WaitForLastTask();              
}










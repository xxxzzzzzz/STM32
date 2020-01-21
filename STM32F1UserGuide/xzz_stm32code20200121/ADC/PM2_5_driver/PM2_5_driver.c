#include "stm32f10x.h"
#include "pm2_5_driver.h"



/*��ʼ��ADC�ܽ�PA2*/
static void ADC_GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //����GPIO����ADC����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ADC������PA2,���Դ�APB2ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//ѡ��PA2  ADCIN2����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ѡ��PA2����Ϊģ������,��Ϊʹ��PA2ΪADC�ɼ�
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*ADC���ܳ�ʼ��*/
static void ADC_Mode_Config()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//����ʹ��ADC1,��ADC1ʱ��
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ADCΪ����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ;//�ر�ɨ��ģʽ����Ϊ�����ǵ�ͨ��ADC�ɼ�,���Ƕ�ͨ��ADC
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//������ͣ��ת��
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//ȡ���ⲿ����,(�ⲿ�����Ƕ�ʱ������GPIO),�����ô���ȥ����ADCת��
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	//����ֻ����PA2ͨ��ADC,����ѡ��1,���ADC_NbrOfChannelָ����ת��ͨ������,������1������1��ͨ��
	
	ADC_Init(ADC1, &ADC_InitStructure);//ʹ��ADC1����
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//����ADCʱ��72M/8=9M
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);//ָ������ͨ��
	
//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//ADCÿ��ת�����������ж�,��ʹ�ã���Ϊ�����õ�DMA�ж�
	
	ADC_DMACmd(ADC1,ENABLE);//����ADC DMA����
	ADC_Cmd(ADC1, ENABLE);//����ADC
	
	ADC_ResetCalibration(ADC1);//��ʼ��У׼ADC
	while (ADC_GetResetCalibrationStatus(ADC1));//�ȴ���ʼ��ADC1У׼���
	
	ADC_StartCalibration(ADC1);//��ʼ��ʽУ׼ADC
	while (ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//���������ADC			
}



extern __IO uint16_t ADC_ConvertedValue;//�洢DMA�õ�ADC DR�Ĵ���������

void ADC_DMA_Init(void)//��ʼ��
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//��DMAʱ��
	
	// ADC1 ��Ӧ DMA1 ͨ�� 1�� ADC3 ��Ӧ DMA2 ͨ�� 5�� ADC2 û�� DMA ����
	DMA_DeInit(DMA1_Channel1);// ��λ DMA1 ͨ��1 ������
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));// �����ַΪ�� ADC1 ���ݼĴ�����ַ
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;// �洢����ַ
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;// ����Դ��������
	
	DMA_InitStructure.DMA_BufferSize = 5;// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// ����Ĵ���ֻ��һ������ַ���õ���
	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;// �洢����ַ���õ�������Ϊ������ADC_ConvertedValue 1������
	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// �������ݴ�СΪ���֣��������ֽ�
	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;// �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ѭ������ģʽ DMA���ϵ��ظ���������
	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ�� DMA ͨ��ʱ�����ȼ����ò�Ӱ��
	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);// ��ʼ�� DMA1 ͨ��1	
	DMA_Cmd(DMA1_Channel1 , ENABLE);// ʹ�� DMA1 ͨ��
		
}


void ADC_init_Cfg(void)
{
	ADC_GPIO_init();
	ADC_Mode_Config();
	ADC_DMA_Init();
}
	







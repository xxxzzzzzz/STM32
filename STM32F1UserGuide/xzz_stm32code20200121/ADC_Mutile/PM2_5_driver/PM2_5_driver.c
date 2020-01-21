#include "stm32f10x.h"
#include "pm2_5_driver.h"



/*��ʼ��ADC���йܽ�*/
static void ADC_GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //����GPIO����ADC����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ADC������PA0��7,���Դ�APB2ʱ��	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	//����GPIOA0 �� 7ΪADCͨ�� 0 �� 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ѡ������Ϊģ������,��Ϊʹ��ADC�ɼ�
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//����ADC������PB0,PB1,���Դ�APB2ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	//����GPIOB0 �� 1ΪADCͨ�� 8 �� 9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ѡ������Ϊģ������,��Ϊʹ��ADC�ɼ�
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//����ADC������PC0��5,���Դ�APB2ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	//����GPIOB0 �� 5ΪADCͨ�� 10 �� 15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ѡ������Ϊģ������,��Ϊʹ��ADC�ɼ�
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

/*ADC���ܳ�ʼ��*/
static void ADC_Mode_Config()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//����ʹ��ADC1,��ADC1ʱ��
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ADCΪ����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ;//����ɨ��ģʽ����Ϊ�����Ƕ�ͨ��ADC�ɼ�
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//������ͣ��ת��
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//ȡ���ⲿ����,(�ⲿ�����Ƕ�ʱ������GPIO),�����ô���ȥ����ADCת��
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	
	ADC_InitStructure.ADC_NbrOfChannel = 16;
	//������16��ͨ��ADC,����ѡ��16,���ADC_NbrOfChannelָ����ת��ͨ������
	
	ADC_Init(ADC1, &ADC_InitStructure);//ʹ��ADC1����
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//����ADCʱ��72M/8=9M
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,6,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,7,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,8,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,9,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,10,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,11,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,12,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_12,13,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_13,14,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_14,15,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_15,16,ADC_SampleTime_55Cycles5);//ÿ��ͨ����Ҫָ�����򣬲���ʱ��
		
	
//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//ADCÿ��ת�����������ж�,��ʹ�ã���Ϊ�����õ�DMA�ж�
	
	ADC_DMACmd(ADC1,ENABLE);//����ADC DMA����
	ADC_Cmd(ADC1, ENABLE);//����ADC
	
	ADC_ResetCalibration(ADC1);//��ʼ��У׼ADC
	while (ADC_GetResetCalibrationStatus(ADC1));//�ȴ���ʼ��ADC1У׼���
	
	ADC_StartCalibration(ADC1);//��ʼ��ʽУ׼ADC
	while (ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//���������ADC			
}



extern __IO uint16_t ADC_ConvertedValue[16];
//�洢DMA�õ�ADC DR�Ĵ���������,��Ϊ��16�����ŵ�ADCͨ����Ҫ�ɼ�������DMA�洢�����ĳ�����

void ADC_DMA_Init(void)//��ʼ��
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//��DMAʱ��
	
	// ADC1 ��Ӧ DMA1 ͨ�� 1�� ADC3 ��Ӧ DMA2 ͨ�� 5�� ADC2 û�� DMA ����
	DMA_DeInit(DMA1_Channel1);// ��λ DMA1 ͨ��1 ������
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));// �����ַΪ�� ADC1 ���ݼĴ�����ַ
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;// �洢����ַ������
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;// ����Դ��������
	
	DMA_InitStructure.DMA_BufferSize = 16;// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С,������������С
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// ����Ĵ���ֻ��һ������ַ���õ���
	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//��ΪDMA_MemoryBaseAddr�õ��������Դ洢����ַ������ÿ��Ԫ��һ��ADCͨ��
	
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
	







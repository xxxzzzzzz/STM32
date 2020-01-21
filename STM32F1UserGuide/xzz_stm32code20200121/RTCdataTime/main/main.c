#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"


//ʱ��ṹ��
typedef struct 
{
    vu8 hour;
    vu8 min;
    vu8 sec;            
    //������������
    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;     
}_calendar_obj;                     

_calendar_obj calendar;


void RTC_Configuration(void);
void RTC_NVIC_init(void);
uint8_t RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec);
uint8_t RTC_Get(void);

int main(void)
{
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);/* ʹ�ܵ�Դ����Ԫ��ʱ��,����Ҫʹ��ʱ�Ӳ��ܽ������ģʽ */
	RCC_configuration();//��ʼ��ʱ��	
	USART_config(115200);//��ʼ������
	
	RTC_Configuration();//��ʼ��RTC
	RTC_NVIC_init();//����RTC�ж����ȼ�
	
	RTC_Set(2018,11,30,23,59,50);
	
	
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
	
	RTC_WaitForLastTask();//��������ֵ֮ǰ����ȴ�RTOFF��λ
	RTC_SetAlarm(110);//����RTC����ֵ��RTC�������110��ִ�������ж�
	
	while(1)
	{
		RTC_Get(); //��ȡʵʱʱ��ʱ��
		printf("RTC year month day hour min sec = %d %d %d - %d : %d : %d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
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



uint8_t Is_Leap_Year(u16 pyear)
{
    if(pyear%4==0)//�������ܱ�4����
    {
        if(pyear%100==0)
        {
            if(pyear%400==0)    return 1;//�����00��β����Ҫ�ܱ�400����
            else    return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}


const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};


/*
�õ���ǰ��ʱ��
�ɹ�����0�����󷵻�����
*/
uint8_t RTC_Get(void)
{
        static u16 dayCount=0;
        u32 secCount=0;
        u32 tmp=0;
        u16 tmp1=0;
        secCount=RTC_GetCounter();
        tmp=secCount/86400;//�õ�����
        if(dayCount!=tmp)//����һ��
        {
            dayCount=tmp;
            tmp1=1970;//��1970�꿪ʼ
            while(tmp>=365)
            {
                if(Is_Leap_Year(tmp1))//������
                {
                    if(tmp>=366)    
                        tmp-=366;//�������������
                    else
                    {
                    //    tmp1++;
                        break;
                    }
                }
                else
                    tmp-=365;//ƽ��
                tmp1++;
            }
            calendar.w_year=tmp1;//�õ����
            tmp1=0;
            while(tmp>=28)//����һ����
            {
                if(Is_Leap_Year(calendar.w_year)&&tmp1==1)//��������������ѭ��2��
                {
                    if(tmp>=29)    
                        tmp-=29;
                    else
                        break;
                }
                else
                {
                    if(tmp>=mon_table[tmp1])//ƽ��
                        tmp-=mon_table[tmp1];
                    else
                        break;
                }
                tmp1++;
            }
            calendar.w_month=tmp1+1;//�õ��·ݣ�tmp1=0��ʾ1�£�����Ҫ��1
            calendar.w_date=tmp+1;    //�õ����ڣ���Ϊ��һ�컹û���꣬����tmpֻ����ǰһ�죬������ʾ��ʱ��Ҫ��ʾ��������
        }
        tmp=secCount%86400;//�õ�������
        calendar.hour=tmp/3600;//Сʱ
        calendar.min=(tmp%3600)/60;//����
        calendar.sec=(tmp%3600)%60;//��
        return 0;
}




/*
*����ʱ��
*�������ʱ��ת��Ϊ����
*��1970��1��1��Ϊ��׼
*1970~2099��Ϊ�Ϸ����
����ֵ��0���ɹ�������������
*/
uint8_t RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 secCount=0;
    if(year<1970||year>2099)
        return 1;//3?�䨪
    for(t=1970;t<year;t++)    //��������ݵ��������
    {
        if(Is_Leap_Year(t))//����
            secCount+=31622400;//�����������
        else
            secCount+=31536000;    
    }
    mon-=1;//�ȼ���һ����������������������5��10�գ���ֻ��Ҫ��ǰ4���µ��������ټ���10�죬Ȼ�����������
    for(t=0;t<mon;t++)
    {
        secCount+=(u32)mon_table[t]*86400;//�·����������
        if(Is_Leap_Year(year)&&t==1)
            secCount+=86400;//���꣬2�·�����һ���������
    }
    
    secCount+=(u32)(day-1)*86400;//��ǰ�����ڵ���������ӣ���һ�컹û���꣬����-1��
    secCount+=(u32)hour*3600;//Сʱ������
    secCount+=(u32)min*60;//����������
    secCount+=sec;
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP,ENABLE);
//    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);//����RTC��������ֵ
    RTC_WaitForLastTask();    //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    RTC_Get();//����ʱ��
    return 0;
}










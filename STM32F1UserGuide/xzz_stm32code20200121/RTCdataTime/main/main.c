#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"


//时间结构体
typedef struct 
{
    vu8 hour;
    vu8 min;
    vu8 sec;            
    //公历年月日周
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
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);/* 使能电源管理单元的时钟,必须要使能时钟才能进入待机模式 */
	RCC_configuration();//初始化时钟	
	USART_config(115200);//初始化串口
	
	RTC_Configuration();//初始化RTC
	RTC_NVIC_init();//设置RTC中断优先级
	
	RTC_Set(2018,11,30,23,59,50);
	
	
	delay_ms(1000);
	printf("xxxzzzzzz\r\n");
	
	RTC_WaitForLastTask();//设置闹钟值之前必须等待RTOFF置位
	RTC_SetAlarm(110);//设置RTC闹钟值，RTC秒计数到110就执行闹钟中断
	
	while(1)
	{
		RTC_Get(); //获取实时时钟时间
		printf("RTC year month day hour min sec = %d %d %d - %d : %d : %d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
		delay_ms(1000);
			
	}
	return 0;
	
}


void RTC_Configuration(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);//RTC需要打开电源管理和后备电池管理
	PWR_BackupAccessCmd(ENABLE);//打开RTC后备电源供电，这样纽扣电池才能供电,这行代码一定放在RCC_APB1PeriphClockCmd之后执行	
	BKP_DeInit();//备份区域复位
	
	RCC_LSEConfig(RCC_LSE_ON);//开启LSE晶振
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//等待LSE晶振开启
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择LSE时钟为RTC时钟源
	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
	
	RTC_WaitForSynchro();//等待RTC时钟和APB1总线时钟同步
	RTC_WaitForLastTask();//为了安全起见，等待对RTC寄存器操作完成
	
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//开启RTC秒中断
	RTC_WaitForLastTask();//等待秒中断开启完成
	RTC_ITConfig(RTC_IT_ALR, ENABLE);//开启闹钟中断
	RTC_WaitForLastTask();//为了安全起见，等待秒中断对RTC寄存器操作完成
	
	RTC_SetPrescaler(32767);//分频频率为(32.768khz)/(32767+1)=1Hz ，(32768hz)/(32767+1)=1Hz，所以RTC 1hz计数一次
							//为什么写32767，这是因为RTC_SetPrescaler函数内部会自动+1，然后写入分频寄存器
							//32768分频还有个原因是外部晶振是32768hz，如果你想计数快点，你可以修改外部晶振或者分频系数
	
	RTC_WaitForLastTask();//为了安全起见，等待分频寄存器操作完成
		
}


void RTC_NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//设置中断组
	/*秒中断*/
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;  //RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //使能该通道中断
	NVIC_Init(&NVIC_InitStructure);  

	/*加入闹钟中断*/
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;  //闹钟中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //比RTC全局中断的优先级高
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
	
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
    {
		printf("alarm interrupt \r\n");
        RTC_ClearITPendingBit(RTC_IT_ALR);//清闹钟中断    
			
	}   

	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);//清除闹钟中断之后还有清除秒中断
    RTC_WaitForLastTask();              
}



uint8_t Is_Leap_Year(u16 pyear)
{
    if(pyear%4==0)//首先需能被4整除
    {
        if(pyear%100==0)
        {
            if(pyear%400==0)    return 1;//如果以00结尾，还要能被400整除
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
得到当前的时间
成功返回0，错误返回其它
*/
uint8_t RTC_Get(void)
{
        static u16 dayCount=0;
        u32 secCount=0;
        u32 tmp=0;
        u16 tmp1=0;
        secCount=RTC_GetCounter();
        tmp=secCount/86400;//得到天数
        if(dayCount!=tmp)//超过一天
        {
            dayCount=tmp;
            tmp1=1970;//从1970年开始
            while(tmp>=365)
            {
                if(Is_Leap_Year(tmp1))//是闰年
                {
                    if(tmp>=366)    
                        tmp-=366;//减掉闰年的天数
                    else
                    {
                    //    tmp1++;
                        break;
                    }
                }
                else
                    tmp-=365;//平年
                tmp1++;
            }
            calendar.w_year=tmp1;//得到年份
            tmp1=0;
            while(tmp>=28)//超过一个月
            {
                if(Is_Leap_Year(calendar.w_year)&&tmp1==1)//当年是闰年且轮循到2月
                {
                    if(tmp>=29)    
                        tmp-=29;
                    else
                        break;
                }
                else
                {
                    if(tmp>=mon_table[tmp1])//平年
                        tmp-=mon_table[tmp1];
                    else
                        break;
                }
                tmp1++;
            }
            calendar.w_month=tmp1+1;//得到月份，tmp1=0表示1月，所以要加1
            calendar.w_date=tmp+1;    //得到日期，因为这一天还没过完，所以tmp只到其前一天，但是显示的时候要显示正常日期
        }
        tmp=secCount%86400;//得到秒钟数
        calendar.hour=tmp/3600;//小时
        calendar.min=(tmp%3600)/60;//分钟
        calendar.sec=(tmp%3600)%60;//秒
        return 0;
}




/*
*设置时钟
*把输入的时钟转换为秒钟
*以1970年1月1日为基准
*1970~2099年为合法年份
返回值：0，成功；其它：错误
*/
uint8_t RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 secCount=0;
    if(year<1970||year>2099)
        return 1;//3?′í
    for(t=1970;t<year;t++)    //把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))//闰年
            secCount+=31622400;//闰年的秒钟数
        else
            secCount+=31536000;    
    }
    mon-=1;//先减掉一个月再算秒数（如现在是5月10日，则只需要算前4个月的天数，再加上10天，然后计算秒数）
    for(t=0;t<mon;t++)
    {
        secCount+=(u32)mon_table[t]*86400;//月份秒钟数相加
        if(Is_Leap_Year(year)&&t==1)
            secCount+=86400;//闰年，2月份增加一天的秒钟数
    }
    
    secCount+=(u32)(day-1)*86400;//把前面日期的秒钟数相加（这一天还没过完，所以-1）
    secCount+=(u32)hour*3600;//小时秒钟数
    secCount+=(u32)min*60;//分钟秒钟数
    secCount+=sec;
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP,ENABLE);
//    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);//设置RTC计数器的值
    RTC_WaitForLastTask();    //等待最近一次对RTC寄存器的写操作完成
    RTC_Get();//更新时间
    return 0;
}










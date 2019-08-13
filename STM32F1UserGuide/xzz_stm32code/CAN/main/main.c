#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"

#include "stdio.h"
#include "can.h"

CanTxMsg Can_Tx_data;//定义Can发送数据包

int main(void)
{
	uint8_t box;//判断邮箱是否发送完成
	   	  	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	
	CAN_init();//初始化CAN引脚
	CAN_Config();//初始化CAN波特率1M
	
	CAN_NVIC_Config();//CAN中断优先级配置
	CAN_Filter(0);//关闭CAN接受过滤器,任何CAN ID都接受,我默认填0
	
	printf("xxxzzzzzz\r\n");
	
	while(1)
	{
		
		
		delay_ms(1000);
		Can_Tx_data.StdId = 0;//11位ID我们不用默认写0
		Can_Tx_data.ExtId = 0;//我们发送19位扩展ID
		Can_Tx_data.RTR = CAN_RTR_Data;//我们要发数据所以选择数据帧
		Can_Tx_data.IDE = CAN_Id_Extended;//发送的是扩展19位ID的数据
		Can_Tx_data.DLC = 8;//我们发送的是8个数据
		
		Can_Tx_data.Data[0] = 0xff;
		Can_Tx_data.Data[1] = 0xff;
		Can_Tx_data.Data[2] = 0xff;
		Can_Tx_data.Data[3] = 0xff;
		Can_Tx_data.Data[4] = 0x01;
		Can_Tx_data.Data[5] = 0x02;
		Can_Tx_data.Data[6] = 0x03;
		Can_Tx_data.Data[7] = 0x04;
		
		printf("CAN send 0xff 0xff 0xff 0xff 0x01 0x02 0x03 0x04\r\n");
		
		box = CAN_Transmit(CAN1,&Can_Tx_data);//把我们写好的数据发送到CAN总线
		while(CAN_TransmitStatus(CAN1,box) == CAN_TxStatus_Failed);//为了安全起见发送数据后检查邮箱是否发送完成
		
		
	}
	return 0;
	
}

CanRxMsg CAN_Rx_Data;//CAN中断接受的数据就放在这里

/*CAN1 接受邮箱0的中断服务函数*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN1,CAN_FIFO0,&CAN_Rx_Data);//这个CAN_Receive函数会自动清除中断标志位,你不用手动清除
	printf("Receive CAN ID = %x data0 = %x data1 = %x data2 = %x data3 = %x data4 = %x\
 data5 = %x data6 = %x data7 = %x \r\n",CAN_Rx_Data.ExtId,CAN_Rx_Data.Data[0],CAN_Rx_Data.Data[1],\
	CAN_Rx_Data.Data[2],CAN_Rx_Data.Data[3],CAN_Rx_Data.Data[4],CAN_Rx_Data.Data[5],CAN_Rx_Data.Data[6],\
	CAN_Rx_Data.Data[7]);
}






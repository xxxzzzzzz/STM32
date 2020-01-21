#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"

#include "stdio.h"
#include "can.h"

CanTxMsg Can_Tx_data;//����Can�������ݰ�

int main(void)
{
	uint8_t box;//�ж������Ƿ������
	   	  	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	
	CAN_init();//��ʼ��CAN����
	CAN_Config();//��ʼ��CAN������1M
	
	CAN_NVIC_Config();//CAN�ж����ȼ�����
	CAN_Filter(0);//�ر�CAN���ܹ�����,�κ�CAN ID������,��Ĭ����0
	
	printf("xxxzzzzzz\r\n");
	
	while(1)
	{
		
		
		delay_ms(1000);
		Can_Tx_data.StdId = 0;//11λID���ǲ���Ĭ��д0
		Can_Tx_data.ExtId = 0;//���Ƿ���19λ��չID
		Can_Tx_data.RTR = CAN_RTR_Data;//����Ҫ����������ѡ������֡
		Can_Tx_data.IDE = CAN_Id_Extended;//���͵�����չ19λID������
		Can_Tx_data.DLC = 8;//���Ƿ��͵���8������
		
		Can_Tx_data.Data[0] = 0xff;
		Can_Tx_data.Data[1] = 0xff;
		Can_Tx_data.Data[2] = 0xff;
		Can_Tx_data.Data[3] = 0xff;
		Can_Tx_data.Data[4] = 0x01;
		Can_Tx_data.Data[5] = 0x02;
		Can_Tx_data.Data[6] = 0x03;
		Can_Tx_data.Data[7] = 0x04;
		
		printf("CAN send 0xff 0xff 0xff 0xff 0x01 0x02 0x03 0x04\r\n");
		
		box = CAN_Transmit(CAN1,&Can_Tx_data);//������д�õ����ݷ��͵�CAN����
		while(CAN_TransmitStatus(CAN1,box) == CAN_TxStatus_Failed);//Ϊ�˰�ȫ����������ݺ��������Ƿ������
		
		
	}
	return 0;
	
}

CanRxMsg CAN_Rx_Data;//CAN�жϽ��ܵ����ݾͷ�������

/*CAN1 ��������0���жϷ�����*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN1,CAN_FIFO0,&CAN_Rx_Data);//���CAN_Receive�������Զ�����жϱ�־λ,�㲻���ֶ����
	printf("Receive CAN ID = %x data0 = %x data1 = %x data2 = %x data3 = %x data4 = %x\
 data5 = %x data6 = %x data7 = %x \r\n",CAN_Rx_Data.ExtId,CAN_Rx_Data.Data[0],CAN_Rx_Data.Data[1],\
	CAN_Rx_Data.Data[2],CAN_Rx_Data.Data[3],CAN_Rx_Data.Data[4],CAN_Rx_Data.Data[5],CAN_Rx_Data.Data[6],\
	CAN_Rx_Data.Data[7]);
}






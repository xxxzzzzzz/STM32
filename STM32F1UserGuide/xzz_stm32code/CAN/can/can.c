#include "can.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"


/*CANÒı½Å³õÊ¼»¯*/
void CAN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);//³õÊ¼»¯CAN1µÄÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	//´ò¿ªPA11,PA12Ê±ÖÓºÍ¸´ÓÃÊ±ÖÓAFIO£¬CAN1ÎÒÃÇÓÃµÄPA11,PA12Òı½Å
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PA12 CAN TX 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//CAN TX·¢ËÍÒı½ÅÒªÉèÖÃ¸´ÓÃÄ£Ê½
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA11 CAN RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//CAN RX½ÓÊÜÒı½Å¿ÉÒÔÉèÖÃ³ÉÉÏÀ­ÊäÈë
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

/*CAN²¨ÌØÂÊºÍ¹¦ÄÜÉèÖÃ*/
void CAN_Config(void)
{
	CAN_InitTypeDef CAN_InitTypeStruct;
	
	CAN_InitTypeStruct.CAN_ABOM = ENABLE;
	CAN_InitTypeStruct.CAN_AWUM = ENABLE;//×Ô¶¯»½ĞÑ¹¦ÄÜ´ò¿ª
	
	/*²¨ÌØÂÊ1MÉèÖÃBS1,BS2,prescaler,SJW*/
	CAN_InitTypeStruct.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitTypeStruct.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitTypeStruct.CAN_Prescaler = 6;
	CAN_InitTypeStruct.CAN_SJW = CAN_SJW_1tq;
	
	CAN_InitTypeStruct.CAN_Mode = CAN_Mode_Normal;//ÉèÖÃCANÎª±ê×¼ÊÕ·¢Ä£Ê½
	CAN_InitTypeStruct.CAN_NART = ENABLE;//Ö§³Ö±¨ÎÄÖØ´«
	CAN_InitTypeStruct.CAN_TTCM = DISABLE;//Õâ¸ö¹¦ÄÜÎÒÃÇCAN2.0ÓÃ²»ÉÏ
	CAN_InitTypeStruct.CAN_TXFP = DISABLE;//°´ÕÕ±¨ÎÄIDÓÅÏÈ¼¶À´Ñ¡ÔñÏÈ·¢Ë
	CAN_InitTypeStruct.CAN_RFLM = ENABLE;//Ëø¶¨fifo¹¦ÄÜ
	
	CAN_Init(CAN1,&CAN_InitTypeStruct);//ÎÒÃÇÓÃCAN1,ÔÚSTM32F105ÀïÃæÓĞCAN2
}

/*CAN¹ıÂËÆ÷ÅäÖÃ*/
void CAN_Filter(uint32_t ID)
{
	CAN_FilterInitTypeDef CAN_FilterTypeStruct;
	
	CAN_FilterTypeStruct.CAN_FilterActivation = ENABLE;//ÊÇ·ñÆô¶¯¹ıÂËÆ÷,ÎÒÕâÀïÆô¶¯
	CAN_FilterTypeStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//¹ıÂË³öĞèÒªIDµÄÊı¾İ¾Í·ÅÔÚfifo0
	CAN_FilterTypeStruct.CAN_FilterNumber = 0;
	CAN_FilterTypeStruct.CAN_FilterScale = CAN_FilterScale_32bit;//ÓÃ32Î»¹ıÂËÆ÷
	CAN_FilterTypeStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	
	CAN_FilterTypeStruct.CAN_FilterIdHigh = (((uint32_t)(ID<<3|CAN_Id_Extended|CAN_RTR_Data))&0xFFFF0000)>>16;
	CAN_FilterTypeStruct.CAN_FilterIdLow =  ((uint32_t)(ID<<3|CAN_Id_Extended|CAN_RTR_Data))&0xFFFF;
	
	CAN_FilterTypeStruct.CAN_FilterMaskIdHigh = 0xffff;
	CAN_FilterTypeStruct.CAN_FilterMaskIdLow = 0xffff;
	
	CAN_FilterInit(&CAN_FilterTypeStruct);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//´ò¿ªCAN1½ÓÊÜÖĞ¶ÏÊ¹ÄÜ,FMP0¾ÍÊÇ½ÓÊÜÓÊÏä0²úÉúÖĞ¶Ï
}

/*CAN½ÓÊÜÖĞ¶ÏÅäÖÃ*/
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//¶¨ÒåCANÖĞ¶ÏÓÅÏÈ¼¶½á¹¹Ìå
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//ÖĞ¶Ï·ÖÅäµ½0×é
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;//CAN1½ÓÊÜÓÊÏä0ÖĞ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//ÇÀÕ¼ÓÅÏÈ¼¶Îª1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//ÏìÓ¦ÓÅÏÈ¼¶(×ÓÓÅÏÈ¼¶)Îª1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ÖĞ¶ÏÓÅÏÈ¼¶¿ªÆô
	NVIC_Init(&NVIC_InitStructure);//³õÊ¼»¯ÖĞ¶ÏÓÅÏÈ¼¶
	
	
}


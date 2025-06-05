#include "stm32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

void CAN_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能GPIOB和AFIO时
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    // 使能重映射，将CAN1映射到PB8/PB9
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

    // CAN RX (PB8) 输入上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CAN TX (PB9) 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CAN 单元初始化
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // 正常模式
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler = 12; // 500kbps@36MHz
    CAN_Init(CAN1, &CAN_InitStructure);

    // 过滤器配置，接收所有
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    // 中断优先级配置
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能 FIFO0 消息挂起中断
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void CAN_SendMessage(uint32_t id, uint8_t *data, uint8_t length)
{
    CanTxMsg TxMessage;
    TxMessage.StdId = id;
    TxMessage.ExtId = 0x00;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = length;
    for (uint8_t i = 0; i < length; i++)
        TxMessage.Data[i] = data[i];

    SEGGER_RTT_printf(0, "Send: %02X\r\n", TxMessage.StdId);
    // CAN_Transmit(CAN1, &TxMessage);
    uint8_t mailbox;
    mailbox = CAN_Transmit(CAN1, &TxMessage);
    while (CAN_TransmitStatus(CAN1, mailbox) != CANTXOK)
        ; // 等待发送完成
}

// 中断服务函数
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        // 这里可以处理接收到的数据，比如打印
        // 例如：SEGGER_RTT_printf(0, "CAN RX: %02X %02X ...\r\n", RxMessage.Data[0], ...);
        SEGGER_RTT_printf(0, "Receive: %02X\r\n", RxMessage.StdId);
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}

int main(void)
{
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
    systick_config();
    CAN_Config();

    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    while (1)
    {
        CAN_SendMessage(0x456, data, 8);
        delay_1ms(1000);
    }
}

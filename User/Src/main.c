/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2019/10/15
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "stm32f10x.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

/* Global define */
#define TASK1_TASK_PRIO 5
#define TASK1_STK_SIZE 256
#define TASK2_TASK_PRIO 5
#define TASK2_STK_SIZE 256

/* Global Variable */
TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;

/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOA.0/1
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      task1_task
 *
 * @brief   task1 program.
 *
 * @param  *pvParameters - Parameters point of task1
 *
 * @return  none
 */
void task1_task(void *pvParameters)
{
    UBaseType_t msticks = 0;
    msticks = pdMS_TO_TICKS(500);
    while (1)
    {
        // printf("task1 entry\r\n");
        GPIO_SetBits(GPIOC, GPIO_Pin_6);
        vTaskDelay(msticks);
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        vTaskDelay(msticks);
    }
}

/*********************************************************************
 * @fn      task2_task
 *
 * @brief   task2 program.
 *
 * @param  *pvParameters - Parameters point of task2
 *
 * @return  none
 */
void task2_task(void *pvParameters)
{
    UBaseType_t msticks = 0;
    msticks = pdMS_TO_TICKS(1000);
    while (1)
    {
        // printf("task2 entry\r\n");
        GPIO_ResetBits(GPIOC, GPIO_Pin_7);
        vTaskDelay(msticks);
        GPIO_SetBits(GPIOC, GPIO_Pin_7);
        vTaskDelay(msticks);
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n\r\n");

    SystemCoreClockUpdate();
    // USART_Printf_Init( 115200 );
    // printf( "SystemClk:%d\r\n", SystemCoreClock );
    // printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    // printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);

    GPIO_Toggle_INIT();
    /* create two task */
    xTaskCreate((TaskFunction_t)task2_task,
                (const char *)"task2",
                (uint16_t)TASK2_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK2_TASK_PRIO,
                (TaskHandle_t *)&Task2Task_Handler);

    xTaskCreate((TaskFunction_t)task1_task,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_TASK_PRIO,
                (TaskHandle_t *)&Task1Task_Handler);
    vTaskStartScheduler();

    while (1)
    {
        // printf("shouldn't run at here!!\n");
    }
}

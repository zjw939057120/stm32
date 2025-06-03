#include "stm32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

int main(void)
{
	SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
	systick_config();
	while (1)
	{
		delay_1ms(1000);
		SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
	}
}

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "FreeRTOS.h"
#include "task.h"

static void sysclk_config(void);
static void error_handler(void);

typedef struct ledTaskParam {
    Led_TypeDef led;
	uint32_t delay;
} ledTaskParam_t;

void ledTask(void *param)
{
	ledTaskParam_t *p = (ledTaskParam_t*)param;
    Led_TypeDef led = p->led;
	uint32_t delay = p->delay;
	while(1) {
		BSP_LED_Toggle(led);
		vTaskDelay(delay);
	}
}

int main(void)
{
	HAL_Init();
	sysclk_config();

	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);

	KEY_BUTTON_GPIO_CLK_ENABLE();

	// Init button
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = 1;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	TaskHandle_t task1_h;
	ledTaskParam_t p1;
	p1.led = LED3;
	p1.delay = 1000;

	TaskHandle_t task2_h;
	ledTaskParam_t p2;
	p2.led = LED4;
	p2.delay = 2000;

	xTaskCreate(ledTask, "LedTask1", configMINIMAL_STACK_SIZE, (void*)&p1, 1, &task1_h);
	xTaskCreate(ledTask, "LedTask2", configMINIMAL_STACK_SIZE, (void*)&p2, 1, &task2_h);
	vTaskStartScheduler();

	vTaskDelay(20000);

	vTaskDelete(task1_h);
	vTaskDelete(task2_h);
}

static void sysclk_config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	// Enable Power Control clock
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
	   clocked below the maximum system frequency, to update the voltage scaling value
	   regarding system frequency refer to product datasheet.	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		error_handler();
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	   clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		error_handler();
	}

	/* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
	if (HAL_GetREVID() == 0x1001)
	{
		/* Enable the Flash prefetch */
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
	}
}

static void error_handler(void)
{
	/* Turn LED5 on */
	BSP_LED_On(LED5);
	while(1)
	{
	}
}

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "ssd1306.h"

static void sysclk_config(void);
static void error_handler(void);
static void init_i2c();

I2C_HandleTypeDef hi2c1;

int main(void)
{
	HAL_Init();
	sysclk_config();

	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);

	KEY_BUTTON_GPIO_CLK_ENABLE();

	// Init button
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = 1;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Init interrupt
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	// Init I2C
	init_i2c();

	ssd1306_Init();
	ssd1306_Fill(White);
	ssd1306_WriteString("HELLO!!!", Font_16x26, Black);
	ssd1306_UpdateScreen();

	while (1) {
		BSP_LED_Toggle(LED3);
		HAL_Delay(1000);
		BSP_LED_Toggle(LED5);
		HAL_Delay(1000);
		BSP_LED_Toggle(LED6);
		HAL_Delay(1000);
		BSP_LED_Toggle(LED4);
		HAL_Delay(1000);
	}

}

void init_i2c()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();

	GPIO_InitTypeDef gpio_i2c;
	gpio_i2c.Pin = GPIO_PIN_8; // I2C1_SCL
	gpio_i2c.Pull = GPIO_NOPULL;
	gpio_i2c.Speed = GPIO_SPEED_FAST;
	gpio_i2c.Mode = GPIO_MODE_AF_OD;
	gpio_i2c.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &gpio_i2c);

	gpio_i2c.Pin = GPIO_PIN_9; // I2C1_SCL
	HAL_GPIO_Init(GPIOB, &gpio_i2c);

	__HAL_RCC_I2C1_FORCE_RESET();
	__HAL_RCC_I2C1_RELEASE_RESET();

	hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed      = BSP_I2C_SPEED;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;

	/* Set priority and enable I2Cx event Interrupt */
	HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

	/* Set priority and enable I2Cx error Interrupt */
	HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

	HAL_I2C_Init(&hi2c1);
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BSP_LED_Off(LED3);
	BSP_LED_Off(LED4);
	BSP_LED_Off(LED5);
	BSP_LED_Off(LED6);
}

/*
 * rekrutacja.c
 *
 *  Created on: 29 cze 2024
 *      Author: gbednarski
 */

#include "rekrutacja.h"
#include "rf_driver_hal_power_manager.h"
#include "rf_driver_ll_gpio.h"
#include "rf_driver_ll_exti.h"
#include "rf_driver_ll_bus.h"
#include "system_BlueNRG_LP.h"
#include "rf_driver_ll_utils.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "rf_driver_ll_tim.h"
#include "bluenrg_lp_com.h"

#define DEBUG_ON 1

#define TIMx                       TIM1
#define LL_EnableClock_TIMx()      LL_APB0_EnableClock(LL_APB0_PERIPH_TIM1);

#define IRQ_EDGE_IS_SET_RISING	1
#define IRQ_EDGE_IS_SET_FALLING	0

/* TIMx clock configuration */
static uint32_t tim_swtrigger_prescaler = 0;
static uint32_t tim_swtrigger_period = 0xFFFFFFFF;

/**/
struct Bits_s measurements[12];

uint8_t measur_number = 0;
uint8_t measur_stat = 0;

volatile uint8_t int_edge = 0;
volatile uint8_t loop = 0;
volatile uint32_t min_period_value = 0;

uint32_t prev_counter_value = 0;

/*
 *@brief UART RX pin setup for interrupt handling
 *@param none
 *@retval none
 */
void rekrut_RX_IRQ_pin_set(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	INT_UART_RX_GPIO_CLK_ENABLE();
	INT_UART_RX_SYSCFG_CLK_ENABLE();

	LL_GPIO_SetOutputPin(INT_UART_RX_GPIO_PORT, INT_UART_RX_GPIO_PIN);
	GPIO_InitStruct.Pin = INT_UART_RX_GPIO_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(INT_UART_RX_GPIO_PORT, &GPIO_InitStruct);
}

/*
 *@brief External Interrupt setting
 *@param none
 *@retval none
 */
void rekrut_IRQ_RX_set(void)
{
	LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	INT_UART_RX_SYSCFG_CLK_ENABLE();

	/* Enable a rising trigger External line 0 Interrupt */
	EXTI_InitStruct.Line = LL_EXTI_LINE_PB0;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Type = LL_EXTI_TYPE_EDGE;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING_EDGE;
	LL_EXTI_Init(&EXTI_InitStruct);

	/* Configure NVIC for INT _IRQn */
	NVIC_SetPriority(INT_UART_RX_EXTI_IRQn, IRQ_LOW_PRIORITY );
	NVIC_EnableIRQ(INT_UART_RX_EXTI_IRQn);

	int_edge = IRQ_EDGE_IS_SET_FALLING;
}

/*
 *@brief Setting rising edge trigger for external interrupt
 *@param none
 *@retval none
 */
void rekrut_IRQ_RX_rising_edge(void)
{
	LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

	EXTI_InitStruct.Line = LL_EXTI_LINE_PB0;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Type = LL_EXTI_TYPE_EDGE;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_EDGE;
	LL_EXTI_Init(&EXTI_InitStruct);
	int_edge = IRQ_EDGE_IS_SET_RISING;
}

/*
 *@brief Setting falling edge trigger for external interrupt
 *@param none
 *@retval none
 */
void rekrut_IRQ_RX_falling_edge(void)
{
	LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

	EXTI_InitStruct.Line = LL_EXTI_LINE_PB0;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Type = LL_EXTI_TYPE_EDGE;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING_EDGE;
	LL_EXTI_Init(&EXTI_InitStruct);
	int_edge = IRQ_EDGE_IS_SET_FALLING;
}

/*
 *@brief Changing edge trigger for external interrupt depends on 0 or 1 from UART
 *@param none
 *@retval none
 */
void rekrut_toggle_IRQ_edge()
{
	if(int_edge == IRQ_EDGE_IS_SET_RISING)
	{
		rekrut_IRQ_RX_falling_edge();
	}
	else
	{
		rekrut_IRQ_RX_rising_edge();
	}
}

/*
 *@brief Test GPIO setting
 *@param none
 *@retval none
 */
void rekrut_out_pin_conf(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	LL_AHB_EnableClock(LL_AHB_PERIPH_GPIOB);

	/* Configure GPIO for INT */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief TIMx Initialization Function
  * @param none
  * @retval none
  */
void rekrut_MX_TIMx_Init(void)
{
  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_EnableClock_TIMx();

  /* Configure the TIMx time base unit */
  TIM_InitStruct.Prescaler = tim_swtrigger_prescaler;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = tim_swtrigger_period;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIMx, &TIM_InitStruct);
  LL_TIM_SetCounter(TIM1, 0);
}


/**
  * @brief UART speed counting
  * @param bit period
  * @retval UART speed
  */
static uint32_t rekrut_update_com_speed(uint32_t bit_period_val)
{
#define K			(1) // number of bits taken to measure speed (1 or 2)
#define S_115200 	(K*283) //K times TIM1 CNT value
#define S_57600  	(K*552)
#define S_38400     (K*830)
#define S_19200   	(K*1664)
#define S_9600 		(K*3330)

	uint32_t speed = 115200; // default speed
	uint32_t perc = 10; // percents of deviation

	if((bit_period_val < (S_115200 + (S_115200/perc))) && (bit_period_val > (S_115200 - (S_115200/perc))))
	{
		speed = 115200;
	}
	if((bit_period_val < (S_57600 + (S_57600/perc))) && (bit_period_val > (S_57600 - (S_57600/perc))))
	{
		speed = 57600;
	}
	if((bit_period_val < (S_38400 + (S_38400/perc))) && (bit_period_val > (S_38400 - (S_38400/perc))))
	{
		speed = 38400;
	}
	if((bit_period_val < (S_19200 + (S_19200/perc))) && (bit_period_val > (S_19200 - (S_19200/perc))))
	{
		speed = 19200;
	}
	if((bit_period_val < (S_9600 + (S_9600/perc))) && (bit_period_val > (S_9600 - (S_9600/perc))))
	{
		speed = 9600;
	}
	  COM_DeInit();
	  rekrutacja_COM_Init(NULL, speed);
	  return speed;
}

/**
  * @brief Settings for TIM1 to start UART transmission measurement
  * @param none
  * @retval none
  */
void rekrut_start_timer(void)
{
	measur_stat = 1;
	rekrut_IRQ_RX_rising_edge();
}


/**
  * @brief Storage of UART transmission measurement
  * @param none
  * @retval none
  */
void rekrut_measurement(void)
{
	uint32_t counter_val = LL_TIM_GetCounter(TIM1);
	rekrut_toggle_IRQ_edge();
	measurements[measur_number].period = counter_val;
	measurements[measur_number].bit_01 = int_edge;
	measur_number++;

	/* first measurement*/
	if(measur_number == 1)
	{
		prev_counter_value = measurements[0].period;
		min_period_value = measurements[0].period;
		measurements[measur_number-1].current_period = counter_val;
	}
	/*next measurements*/
	if(measur_number > 1)
	{
		uint32_t current_period_value = counter_val - prev_counter_value;
		measurements[measur_number - 1].current_period = current_period_value;
		prev_counter_value = counter_val;
		if(current_period_value < min_period_value)
		{
			min_period_value = current_period_value;
		}
	}
}

/**
  * @brief Counting rx byte
  * @param none
  * @retval none
  */
void rekrut_stop_measurement(void)
{
	NVIC_DisableIRQ(INT_UART_RX_EXTI_IRQn);
	LL_TIM_DisableCounter(TIM1);

	uint32_t current_speed = rekrut_update_com_speed(min_period_value);

#if DEBUG_ON == 1
	{
		printf("\ntim value: %ld (TIM1 CNT value per UART bit period)\n", min_period_value);
		printf("current UART speed: %ld \n", current_speed);

		for(uint8_t k = 0; k < 12; k++)
		{
			printf("Pomiar %d: %ld, current_period %ld, wartosc bitu: %d \n", k, measurements[k].period, measurements[k].current_period, measurements[k].bit_01);
		}
		printf("Pomiar min_period_value: %ld\n", min_period_value);
		printf("\n");
		printf("\n");
	}
#endif

	uint8_t rx_data;

	uint8_t bitsy[12];
	uint8_t bits_no = 0;

	for(uint8_t g = 0; g < 12; g++)
	{
		uint8_t p = 0;
		if(measurements[g].current_period > 0)
		{
		bitsy[g] = (measurements[g].current_period + (min_period_value/2)) / min_period_value;
#if DEBUG_ON == 1
		printf("Bitsy[%d] %d\n", g, bitsy[g]);
#endif

		bits_no = bits_no + bitsy[g];

		for(uint8_t j = 0; j < bitsy[g]; j++)
		{
			if(measurements[g].bit_01 == 0)
			{
				rx_data = (rx_data >> 1) & 0x7F;
			}
			else
			{
				rx_data = (rx_data >> 1) | 0x80;
			}
		}
	}
	}
	if(bits_no < 9)
	{
		for(uint8_t n = 0; n < (9 - bits_no); n++)
		{
			rx_data = (rx_data >> 1) | 0x80;
		}
	}
#if DEBUG_ON == 1
	printf("bits_no %d\n", bits_no);
	printf("Odczytano %c\n", rx_data);
	printf("Odczytano 0x%02x\n", rx_data);
#endif

}


/**
  * @brief UART transmission measurement init function
  * @param none
  * @retval none
  */
void rekrut_init(void)
{
	loop = 0;
	measur_stat = 0;
	measur_number = 0;
	min_period_value = 3700; // max period for 1 bit for UART speed 9600

	rekrut_RX_IRQ_pin_set();
	rekrut_IRQ_RX_set();
	rekrut_out_pin_conf();
	rekrut_MX_TIMx_Init();
	memset(&measurements[0], 0, 12*(sizeof(struct Bits_s)));
}


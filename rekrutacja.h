/*
 * rekrutacja.h
 *
 *  Created on: 29 cze 2024
 *      Author: gbednarski
 */

#ifndef INC_REKRUTACJA_H_
#define INC_REKRUTACJA_H_

#include "stdint.h"

/* definitions sets for EXTI IRQ UART RX*/
#define INT_UART_RX_GPIO_PIN               		LL_GPIO_PIN_0
#define INT_UART_RX_GPIO_PORT              		GPIOB
#define INT_UART_RX_GPIO_CLK_ENABLE()      		LL_AHB_EnableClock(LL_AHB_PERIPH_GPIOB)
#define INT_UART_RX_SYSCFG_CLK_ENABLE()    		LL_APB0_EnableClock(LL_APB0_PERIPH_SYSCFG)
#define INT_UART_RX_EXTI_LINE              		LL_EXTI_LINE_PB0
#define INT_UART_RX_EXTI_IRQn              		GPIOB_IRQn
#define INT_UART_RX_EXTI_LINE_ENABLE()     		LL_EXTI_EnableIT(INT_UART_RX_EXTI_LINE)
#define INT_UART_RX_EXTI_RISING_TRIG_ENABLE()  	      LL_EXTI_SetTrigger(LL_EXTI_TRIGGER_RISING_EDGE, INT_UART_RX_EXTI_LINE)

/* UART transmission measurement parameters structure*/
typedef struct Bits_s
{
  uint32_t period;
  uint32_t current_period;
  uint8_t  bit_01;
}Bits_TypeDef;


oid rekrut_start_timer(void);
void rekrut_init(void);
void rekrut_measurement(void);
void rekrut_stop_measurement(void);

#endif /* INC_REKRUTACJA_H_ */

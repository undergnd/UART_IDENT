

/**
* @brief  This function handles GPIO interrupt request.
* @param  None
* @retval None
*/

void GPIOB_IRQHandler(void)
{
	  if (LL_EXTI_IsInterruptPending(INT_UART_RX_EXTI_LINE) != RESET)
	  {
	    LL_EXTI_ClearInterrupt(INT_UART_RX_EXTI_LINE);
	    /* Handle interrupt from uart rx in dedicated function */
	    if(measur_stat == 0)
	    {
	    	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_8);
	    	LL_TIM_EnableCounter(TIM1);
	    	rekrut_start_timer();
	    }
	    else
	    {
	    	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);
	    	rekrut_measurement();
	    }
	  }
}


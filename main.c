
  COM_Init(NULL);

  uint32_t UID_0 = LL_GetUID_Word0();
  uint32_t UID_1 = LL_GetUID_Word1();


  printf("HELLO from Bluenrg-LP ");
  printf("UID_1: %x, UID_0: %x \r\n", UID_1, UID_0);
  LL_mDelay(10);

  rekrut_init();
  uint16_t g = 0;
  uint32_t counter_val_get = 0;

  while(1)
	{
	  printf("\niteration %d  \n", g++);

	  while((loop == 0) && (counter_val_get < (12 * min_period_value)))
	  {
			counter_val_get = LL_TIM_GetCounter(TIM1);
	  }
	  rekrut_stop_measurement();
	  rekrut_init();
	}
}


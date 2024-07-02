/*
 * main.h
 *
 *  Created on: 18 lut 2024
 *      Author: gbednarski
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stdint.h"
#include "stdio.h"
#include "timers.h"


//#define DEBUG
#define DEBUG_ON 1
#define POWER_SAVE 1
#define MAX_BITS_NO 	12

typedef enum
{
	BLE_RAW = 0,
	BLE_STACK  = 1
}ble_stack_raw;


typedef enum
{
	ADV_DEBUG = 0,
	ADV_FMB  = 1
}ble_adv_type_message;

/* Private macro -------------------------------------------------------------*/

#define PRINT_INT(x)    ((int)(x))
#define PRINT_FLOAT_1000(x)  (x>0)? ((int) (((x) - PRINT_INT(x)) * 1000)) : (-1*(((int) (((x) - PRINT_INT(x)) * 1000)))) // trzy miejsca po przecinku
#define PRINT_FLOAT(x)  (x>0)? ((int) (((x) - PRINT_INT(x)) * 100)) : (-1*(((int) (((x) - PRINT_INT(x)) * 100)))) // dwa miejsca po przecinku

#define POWER_HIGH_LOW   1 //1
#define POWER_LEVEL_ACI  31 //24


//0: 0x709896019fac1b34
//1: 0x709896019fb7efe4
//2: 0x709896019fffd9ca
//3: 0x709896019fe0082f
//4: 0x70989601905c0962
//5: 0x709896019fbc2456
//6: 0x70989601907bfca2
//7: 0x7098960190641171
//8: 0x709896019068018f
//9: 0x70989601909806be
//10:0x70989601909c1027
//11:0x7098960190000619
//12:0x7098960190080829
//13:0x709896019f94156b
//14:0x709896019f98222b
//15:0x70989601908bf8e2


#define S0_SUID_1  0x70989601
#define S0_SUID_0  0x9fac1b34
#define S1_SUID_1  0x70989601
#define S1_SUID_0  0x9fb7efe4
#define S2_SUID_1  0x70989601
#define S2_SUID_0  0x9fffd9ca
#define S3_SUID_1  0x70989601
#define S3_SUID_0  0x9fe0082f
#define S4_SUID_1  0x70989601
#define S4_SUID_0  0x905c0962
#define S5_SUID_1  0x70989601
#define S5_SUID_0  0x9fbc2456
#define S6_SUID_1  0x70989601
#define S6_SUID_0  0x907bfca2
#define S7_SUID_1  0x70989601
#define S7_SUID_0  0x90641171
#define S8_SUID_1  0x70989601
#define S8_SUID_0  0x9068018f
#define S9_SUID_1  0x70989601
#define S9_SUID_0  0x909806be
#define S10_SUID_1  0x70989601
#define S10_SUID_0  0x909c1027
#define S11_SUID_1  0x70989601
#define S11_SUID_0  0x90000619
#define S12_SUID_1  0x70989601
#define S12_SUID_0  0x90080829
#define S13_SUID_1  0x70989601
#define S13_SUID_0  0x9f94156b
#define S14_SUID_1  0x70989601
#define S14_SUID_0  0x9f98222b
//#define S15_SUID_1  0x70989601
//#define S15_SUID_0  0x908bf8e2
#define S15_SUID_1  0x70989601
#define S15_SUID_0  0x901c1b06 // DODATKOWY CZUJNIK Z POZA TABLICY


void ModulesTick(void);
void BlueNRG_reset(void );

#endif /* INC_MAIN_H_ */

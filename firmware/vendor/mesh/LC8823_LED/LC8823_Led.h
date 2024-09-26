/*
 * LC8823_Led.h
 *
 *  Created on: Jun 11, 2022
 *      Author: Hao Dv
 */

#ifndef LC8823_LED_H_
#define LC8823_LED_H_

#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

//#include "../RD_Switch/RD_Switch.h"
#include "../RD_Lib.h"
#define LC8823_FREQ         20000   // max ClK 27Khz
									// time send control 4 led = 32*4 + 32*2 = 192 bit  --> 10ms
//#define LED_DIM_ON 			20
//#define LED_DIM_OFF 		4

#define LC8823_CLK_H		gpio_write(LC8823_CLK_PIN, 1);
#define LC8823_CLK_L 		gpio_write(LC8823_CLK_PIN, 0);

#if(TYPE_LED != RGB_LED)
	#ifndef LC8823_CLK_PIN
	#define	LC8823_CLK_PIN		0
	#endif
	#ifndef LC8823_DATA_PIN
	#define	LC8823_DATA_PIN		0
	#endif
#endif
#define LC8823_DATA_H 		gpio_write(LC8823_DATA_PIN, 1);
#define LC8823_DATA_L 		gpio_write(LC8823_DATA_PIN, 0);

#define START_FRAME         0x00
#define STOP_FRAME          0xFF

#if( (TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH))
	#define LED1_DATA_ID        0x00
	#define LED2_DATA_ID        0x01
	#define LED3_DATA_ID 		0x02
	#define LED4_DATA_ID        0x03
	#define LED5_DATA_ID        0x04
#endif

#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
	#define LED1_DATA_ID        0x00
	#define LED2_DATA_ID        0x02
	#define LED3_DATA_ID 		0x03
	#define LED4_DATA_ID        0x01
#endif
#if(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH)
	#define LED1_DATA_ID        0x00
	#define LED2_DATA_ID        0x02
	#define LED3_DATA_ID 		0x01
#endif
#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH)
	#define LED1_DATA_ID        0x00
	#define LED2_DATA_ID        0x01
#endif
#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH)
	#define LED1_DATA_ID        0x00
#endif
#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
	#define LED1_DATA_ID        0x00
#endif
typedef struct
{
	uint8_t Lum;
	uint8_t Blue_Gray;
	uint8_t Green_Gray;
	uint8_t Red_Gray;
} LED_Data_Str;

typedef struct
{
	uint8_t  Control;
	LED_Data_Str LED[5];
} LED_Switch_Str;

struct RGB
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

struct HSL
{
	int H;
	float S;
	float L;
};

extern  LED_Switch_Str LED_Switch_Data;
void LC8823_Send_StartFrame(void);
void LC8823_Send_StopFrame(void);
void LC8823_Send_LED_Data(uint8_t Brightness, uint8_t Blue_Gray, uint8_t Green_Gray, uint8_t Red_Gray);
void LED_Set_Stt(uint8_t Led_Num, uint8_t Brightness, uint8_t Blue_Gray, uint8_t Green_Gray, uint8_t Red_Gray);
void LED_Set_OnOff_Stt(uint8_t Led_Num, uint8_t OnOff_Stt);
void LED_Update_Stt();
void LED_Init_Value(void);
/**
 * @brief convert HSL to RGB color
 * 
 * @param hsl: hsl val in
 * @return struct RGB: RGB out
 */
struct RGB HSLToRGB(struct HSL hsl);
#endif /* LC8823_LED_H_ */
 

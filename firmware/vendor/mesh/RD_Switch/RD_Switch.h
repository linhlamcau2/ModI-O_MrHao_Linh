/*
 * RD_Switch.h
 *
 *  Created on: Jan 20, 2022
 *      Author: Dang_Hao
 */

#ifndef RD_SWITCH_H_
#define RD_SWITCH_H_

#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "../../common/light.h"
#include "../RD_MessData/RD_MessData.h"
#include "../RD_Flash/RD_Flash.h"
#include "../RD_HARDWARE_TYPE/RD_Hardware_Type.h"
#include "../LC8823_LED/LC8823_Led.h"

#include "../../common/mi_api/libs/mesh_auth/mible_mesh_device.h"
//#if(TYPE_HARDWARE_SWITCH < X5_CEILING_SWITCH) // sw touch/machine
//#if(HARDWARE_VERSION == HARDWARE_V1)
//#	define  	VERSION_FIRM_H				0x01   // sw v1
//#	define  	VERSION_FIRM_L				0x0C
//#endif
//#if(HARDWARE_VERSION == HARDWARE_V2)
////#	define  	VERSION_FIRM_H				0x01   // sw new cover
////#	define  	VERSION_FIRM_L				0x00
//#endif
//#else
////#	define  	VERSION_FIRM_H				0x01  // celling sw
////#	define  	VERSION_FIRM_L				0x02 //  add K9B off all
//#endif
/*
 * + support K9B remote OnOff mode (pair, save, active, eraser)
 */



#define 	TIME_TRAIN_FACTORY			1200
#define  	TIME_RESET_ALL				60000//ms
#define 	TRAIN_MAX_TIME				1
#define     DELAY_TRAIN_TIME			90		//s
#define 	GW_ADD_DEFAULT				0x0001
#define     TIME_CYCLE_RST_TOUCH_S		3600   // 3600s 1hour

#if (TYPE_HARDWARE_SWITCH < X5_CEILING_SWITCH)
#	define 	LOGIC_ACTIVE_BT				1 // Touch-machine-socket is 1,
#else
#	define 	LOGIC_ACTIVE_BT				0 // Touch-machine-socket is 1,
#endif

/*--------------------------- Time wait detect relay----------------------------------*/
#if ( (TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH))
#	if(HARDWARE_VERSION == HARDWARE_V1)
#		define TIME_DETECT_ON		3000	//us
#		define TIME_DETECT_OFF		1700	//us
#	else
#		define TIME_DETECT_ON		3000	//us
#		define TIME_DETECT_OFF		1700	//us
#	endif
#endif

#if (  (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH) \
	|| (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) \
	|| (TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH))
#	if(HARDWARE_VERSION == HARDWARE_V1)
#		define TIME_DETECT_ON		4600	//us
#		define TIME_DETECT_OFF		3200	//us
#	else
#		define TIME_DETECT_ON		6000	//us
#		define TIME_DETECT_OFF		4800	//us
#	endif
#endif

#if (  (TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH) || (TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH))
#	if( (HARDWARE_VERSION == HARDWARE_V1) || (HARDWARE_VERSION == HARDWARE_V2))
#		define TIME_DETECT_ON		4600	//us
#		define TIME_DETECT_OFF		3200	//us
#	endif
#endif


#if (TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH)
#	define TIME_DETECT_ON		3000	//us
#	define TIME_DETECT_OFF		1700	//us
#endif

#if ( (TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH))
#	define TIME_DETECT_ON		4600	//us
#	define TIME_DETECT_OFF		3200	//us
#endif
/*-------------------------------------end detect time---------------------------------*/
#if(HARDWARE_VERSION == HARDWARE_V2)
#	define 	TOUCH_ACTIVE_POW			0 // 1 is origin ver
#	define 	TOUCH_NON_ACTIVE_POW		1
#else
#	define 	TOUCH_ACTIVE_POW			1 // 1 is origin ver
#	define 	TOUCH_NON_ACTIVE_POW		0
#endif


#if(TYPE_LED == RGB_LED)
	#if(TYPE_HARDWARE_SWITCH != X1_TOUCH_SWITCH)
		#define 	LED_DIM_OFF_DF 				2			//   2/31
		#define		LED_DIM_ON_DF				16
	#else
		#define 	LED_DIM_OFF_DF 				5			//  5/31
		#define		LED_DIM_ON_DF				30
	#endif
#else
	#if(TYPE_HARDWARE_SWITCH >= X5_CEILING_SWITCH)
	#	define 		LED_DIM_OFF_DF 				0
	#	define		LED_DIM_ON_DF				90
	#else
	#	define 		LED_DIM_OFF_DF 				20
	#	define		LED_DIM_ON_DF				90
	#endif
#endif

#if 1
#	define		BLUE_BT_DF					0xB2
#	define		GREEN_BT_DF					0xFF
#	define		RED_BT_DF					0x00
#else
#	define		BLUE_BT_DF					0xFF
#	define		GREEN_BT_DF					0x00
#	define		RED_BT_DF					0x00
#endif

//#define 	RD_PROVIDER					0x0001
//#define 	VHT_PROVIDER				0x0200
//#define 	HUE_PROVIDER				0x0201
//#define 	PROVIDER_ID					RD_PROVIDER

#if(TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH)
	#define 	NUM_OF_ELEMENT		 5

	#define 	DETECT_ZERO_PIN				GPIO_PA7

	#define		BT1_PIN						GPIO_PD3 //GPIO_PC2
	#define		BT2_PIN						GPIO_PD4 //GPIO_PA1
	#define		BT3_PIN						GPIO_PA0
	#define 	BT4_PIN						GPIO_PA1 //GPIO_PD4
	#define 	BT5_PIN						GPIO_PC2 //GPIO_PD3


	#define		RELAY1_PIN					GPIO_PB4 //GPIO_PC4
	#define		RELAY2_PIN					GPIO_PB6 //GPIO_PC0
	#define 	RELAY3_PIN					GPIO_PB7
	#define 	RELAY4_PIN					GPIO_PC0 //GPIO_PB6
	#define 	RELAY5_PIN					GPIO_PC4 //GPIO_PB4

	#define 	LED1_PIN					GPIO_PD2//GPIO_PC3
	#define 	LED1_PWM					PWM3_ID //PWM1_ID

	#define 	LED2_PIN					GPIO_PB1 //GPIO_PC1
	#define 	LED2_PWM					PWM4_ID //PWM0_ID

	#define 	LED3_PIN					GPIO_PB5
	#define 	LED3_PWM					PWM5_ID

	#define 	LED4_PIN					GPIO_PC1//GPIO_PB1
	#define 	LED4_PWM					PWM0_ID //PWM4_ID

	#define 	LED5_PIN					GPIO_PC3 //GPIO_PD2
	#define 	LED5_PWM					PWM1_ID //PWM3_ID

//	#define		x5_Celling_Sw_BROADCAR_0		0x02
//	#define		x5_Celling_Sw_BROADCAR_1		0x02
//	#define 	x5_Celling_Sw_BROADCAR_2		0x27
//	#define		x5_Celling_Sw_BROADCAR_3		0x00
//	#define		x5_Celling_Sw_BROADCAR_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x5_Celling_Sw_BROADCAR_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x5_Celling_Sw_BROADCAR_6		VERSION_FIRM_H
//	#define		x5_Celling_Sw_BROADCAR_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x5_Celling_Sw_BROADCAR_0
//	#define		Device_BROADCRARD_1		x5_Celling_Sw_BROADCAR_1
//	#define		Device_BROADCRARD_2		x5_Celling_Sw_BROADCAR_2
//	#define		Device_BROADCRARD_3		x5_Celling_Sw_BROADCAR_3
//	#define		Device_BROADCRARD_4		x5_Celling_Sw_BROADCAR_4
//	#define		Device_BROADCRARD_5		x5_Celling_Sw_BROADCAR_5
//	#define		Device_BROADCRARD_6		x5_Celling_Sw_BROADCAR_6
//	#define		Device_BROADCRARD_7		x5_Celling_Sw_BROADCAR_7
#endif

#if(TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH)
	#define 	NUM_OF_ELEMENT		 3

	#define 	DETECT_ZERO_PIN				GPIO_PC2

	#define		BT1_PIN						GPIO_PA0 //GPIO_PC3
	#define		BT2_PIN						GPIO_PA1
	#define		BT3_PIN						GPIO_PC3 //GPIO_PA0


	#define		RELAY1_PIN					GPIO_PB7 //GPIO_PC4
	#define		RELAY2_PIN					GPIO_PC0
	#define 	RELAY3_PIN					GPIO_PC4 //GPIO_PB7

	#define 	LED1_PIN					GPIO_PB1 //GPIO_PC1
	#define 	LED1_PWM					PWM4_ID //PWM0_ID

	#define 	LED2_PIN					GPIO_PB5
	#define 	LED2_PWM					PWM5_ID

	#define 	LED3_PIN					GPIO_PC1 //GPIO_PB1
	#define 	LED3_PWM					PWM0_ID //PWM4_ID


//	#define		x3_Celling_Sw_BROADCAR_0		0x02
//	#define		x3_Celling_Sw_BROADCAR_1		0x02
//	#define 	x3_Celling_Sw_BROADCAR_2		0x26
//	#define		x3_Celling_Sw_BROADCAR_3		0x00
//	#define		x3_Celling_Sw_BROADCAR_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x3_Celling_Sw_BROADCAR_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x3_Celling_Sw_BROADCAR_6		VERSION_FIRM_H
//	#define		x3_Celling_Sw_BROADCAR_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x3_Celling_Sw_BROADCAR_0
//	#define		Device_BROADCRARD_1		x3_Celling_Sw_BROADCAR_1
//	#define		Device_BROADCRARD_2		x3_Celling_Sw_BROADCAR_2
//	#define		Device_BROADCRARD_3		x3_Celling_Sw_BROADCAR_3
//	#define		Device_BROADCRARD_4		x3_Celling_Sw_BROADCAR_4
//	#define		Device_BROADCRARD_5		x3_Celling_Sw_BROADCAR_5
//	#define		Device_BROADCRARD_6		x3_Celling_Sw_BROADCAR_6
//	#define		Device_BROADCRARD_7		x3_Celling_Sw_BROADCAR_7
#endif

#if(TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH)
	#define 	NUM_OF_ELEMENT		 2

	#define 	DETECT_ZERO_PIN				GPIO_PC2

	#define		BT1_PIN						GPIO_PA1 //GPIO_PC3
	#define		BT2_PIN						GPIO_PC3 //GPIO_PA1


	#define		RELAY1_PIN					GPIO_PC0 //GPIO_PC4
	#define		RELAY2_PIN					GPIO_PC4 //GPIO_PC0
//	#define 	RELAY3_PIN					GPIO_PB7

	#define 	LED1_PIN					GPIO_PB1 //GPIO_PC1
	#define 	LED1_PWM					PWM4_ID //PWM0_ID

	#define 	LED2_PIN					GPIO_PC1 //GPIO_PB1
	#define 	LED2_PWM					PWM0_ID //PWM4_ID


	#define		x2_Celling_Sw_BROADCAR_0		0x02
	#define		x2_Celling_Sw_BROADCAR_1		0x02
	#define 	x2_Celling_Sw_BROADCAR_2		0x25
	#define		x2_Celling_Sw_BROADCAR_3		0x00
	#define		x2_Celling_Sw_BROADCAR_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
	#define		x2_Celling_Sw_BROADCAR_5		(PROVIDER_ID       & 0xFF) // khong dao
	#define		x2_Celling_Sw_BROADCAR_6		VERSION_FIRM_H
	#define		x2_Celling_Sw_BROADCAR_7		VERSION_FIRM_L

	#define		Device_BROADCRARD_0		x2_Celling_Sw_BROADCAR_0
	#define		Device_BROADCRARD_1		x2_Celling_Sw_BROADCAR_1
	#define		Device_BROADCRARD_2		x2_Celling_Sw_BROADCAR_2
	#define		Device_BROADCRARD_3		x2_Celling_Sw_BROADCAR_3
	#define		Device_BROADCRARD_4		x2_Celling_Sw_BROADCAR_4
	#define		Device_BROADCRARD_5		x2_Celling_Sw_BROADCAR_5
	#define		Device_BROADCRARD_6		x2_Celling_Sw_BROADCAR_6
	#define		Device_BROADCRARD_7		x2_Celling_Sw_BROADCAR_7
#endif

#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
	#define 				NUM_OF_ELEMENT		 4

	#define 	DETECT_ZERO_PIN				GPIO_PB4

	#define		BT1_PIN						GPIO_PC2
	#define		BT2_PIN						GPIO_PD3
	#define		BT3_PIN						GPIO_PC1
	#define 	BT4_PIN						GPIO_PD4

	#define 	LC8823_CLK_PIN 				GPIO_PC3
	#define 	LC8823_DATA_PIN 			GPIO_PC4

	#define 	RESET_TOUCH_PIN				GPIO_PA1

	#define		RELAY1_PIN					GPIO_PC0
	#define		RELAY2_PIN					GPIO_PB7
	#define 	RELAY3_PIN					GPIO_PB6
	#define 	RELAY4_PIN					GPIO_PB5

	
//	#define		x4_Touch_Sw_BROADCRARD_0		0x02
//	#define		x4_Touch_Sw_BROADCRARD_1		0x02
//
//	#if(HARDWARE_VERSION == HARDWARE_V2)
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x4_Touch_Sw_BROADCRARD_2		0x20
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#   	define		x4_Touch_Sw_BROADCRARD_2		0x21
//	#	endif
//	#else
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x4_Touch_Sw_BROADCRARD_2		0x0F
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#		define		x4_Touch_Sw_BROADCRARD_2		0x16
//	#	endif
//	#endif
//
//	#define		x4_Touch_Sw_BROADCRARD_3		0x00
//	#define		x4_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x4_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x4_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x4_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x4_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x4_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x4_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x4_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x4_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x4_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x4_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x4_Touch_Sw_BROADCRARD_7

#endif

#if(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)

	#define 	NUM_OF_ELEMENT		 		3
	#define 	DETECT_ZERO_PIN				GPIO_PB4

	#define		BT1_PIN						GPIO_PC2 
	#define		BT2_PIN						GPIO_PD3 
	#define		BT3_PIN						GPIO_PC1

	#define 	LC8823_CLK_PIN 				GPIO_PC3
	#define 	LC8823_DATA_PIN 			GPIO_PC4

	#define 	RESET_TOUCH_PIN				GPIO_PA1

	#define		RELAY1_PIN					GPIO_PC0			
	#define		RELAY2_PIN					GPIO_PB7
	#define 	RELAY3_PIN					GPIO_PB6

//	#define		x3_Touch_Sw_BROADCRARD_0		0x02
//	#define		x3_Touch_Sw_BROADCRARD_1		0x02
//
//	#if(HARDWARE_VERSION == HARDWARE_V2)
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x3_Touch_Sw_BROADCRARD_2		0x1E
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#   	define		x3_Touch_Sw_BROADCRARD_2		0x1F
//	#	endif
//	#else
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x3_Touch_Sw_BROADCRARD_2		0x0E
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#		define		x3_Touch_Sw_BROADCRARD_2		0x15
//	#	endif
//	#endif
//
//	#define		x3_Touch_Sw_BROADCRARD_3		0x00
//	#define		x3_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x3_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x3_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x3_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x3_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x3_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x3_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x3_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x3_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x3_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x3_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x3_Touch_Sw_BROADCRARD_7

#endif

#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)

	#define 	NUM_OF_ELEMENT		 		2
	#define 	DETECT_ZERO_PIN				GPIO_PB4

	#define		BT1_PIN						GPIO_PC2 
	#define		BT2_PIN						GPIO_PD3 

	#define 	LC8823_CLK_PIN 				GPIO_PC3
	#define 	LC8823_DATA_PIN 			GPIO_PC4

	#define 	RESET_TOUCH_PIN				GPIO_PA1

	#define		RELAY1_PIN					GPIO_PC0			
	#define		RELAY2_PIN					GPIO_PB7

//	#define		TYPE_DEVICE_CH_TOUCH		0x02
//
//	#define		x2_Touch_Sw_BROADCRARD_0		0x02
//	#define		x2_Touch_Sw_BROADCRARD_1		0x02
//
//
//	#if(HARDWARE_VERSION == HARDWARE_V2)
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x2_Touch_Sw_BROADCRARD_2		0x1C
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#   	define		x2_Touch_Sw_BROADCRARD_2		0x1D
//	#	endif
//	#else
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x2_Touch_Sw_BROADCRARD_2		0x0D
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#		define		x2_Touch_Sw_BROADCRARD_2		0x14
//	#	endif
//	#endif
//
//	#define		x2_Touch_Sw_BROADCRARD_3		0x00
//	#define		x2_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x2_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x2_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x2_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x2_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x2_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x2_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x2_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x2_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x2_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x2_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x2_Touch_Sw_BROADCRARD_7
#endif

#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
	#define 	NUM_OF_ELEMENT		 		1

	#define 	DETECT_ZERO_PIN				GPIO_PB4

	#define		BT1_PIN						GPIO_PC2

	#define 	LC8823_CLK_PIN 				GPIO_PC3
	#define 	LC8823_DATA_PIN 			GPIO_PC4

	#define 	RESET_TOUCH_PIN				GPIO_PA1

	#define		RELAY1_PIN					GPIO_PC0

//	#define		x1_Touch_Sw_BROADCRARD_0		0x02
//	#define		x1_Touch_Sw_BROADCRARD_1		0x02
//
//	#if(HARDWARE_VERSION == HARDWARE_V2)
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x1_Touch_Sw_BROADCRARD_2		0x1A
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#   	define		x1_Touch_Sw_BROADCRARD_2		0x1B
//	#	endif
//	#else
//	#	if(PCB_SHAPE == RECTANGLE_PCB)
//	#		define		x1_Touch_Sw_BROADCRARD_2		0x0C
//	#	endif
//	#	if(PCB_SHAPE == SQUARE_PCB)
//	#		define		x1_Touch_Sw_BROADCRARD_2		0x13
//	#	endif
//	#endif


//	#define		x1_Touch_Sw_BROADCRARD_3		0x00
//	#define		x1_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x1_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x1_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x1_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x1_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x1_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x1_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x1_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x1_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x1_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x1_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x1_Touch_Sw_BROADCRARD_7
#endif

#if(TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH)
	#define 	NUM_OF_ELEMENT		 		1


	#if(HARDWARE_VERSION == HARDWARE_V2)
		#define 	DETECT_ZERO_PIN				GPIO_PB6

		#define		BT1_PIN						GPIO_PC1

		#define 	LED1_PIN					GPIO_PC2 //GPIO_PC4
		#define 	LED1_PWM					PWM0_ID  //PWM2_ID

		#define		RELAY1_PIN					GPIO_PD2

		#define		x1_Touch_Sw_BROADCRARD_2		0x06
	#else
		#define 	DETECT_ZERO_PIN				GPIO_PB6

		#define		BT1_PIN						GPIO_PC1

		#define 	LED1_PIN					GPIO_PC2 //GPIO_PC4
		#define 	LED1_PWM					PWM0_ID  //PWM2_ID


		#define		RELAY1_PIN					GPIO_PD2
		#define		x1_Touch_Sw_BROADCRARD_2		0x01
	#endif


//	#define		x1_Touch_Sw_BROADCRARD_0		0x02
//	#define		x1_Touch_Sw_BROADCRARD_1		0x04
//	#define		x1_Touch_Sw_BROADCRARD_3		0x00
//	#define		x1_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x1_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x1_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x1_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x1_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x1_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x1_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x1_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x1_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x1_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x1_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x1_Touch_Sw_BROADCRARD_7
#endif

#if(TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH)
	#define 	NUM_OF_ELEMENT		 		2

	#if(HARDWARE_VERSION == HARDWARE_V2)
		#define 	DETECT_ZERO_PIN				GPIO_PB6

		#define		BT1_PIN						GPIO_PC4
		#define		BT2_PIN						GPIO_PC1

		#define 	LED1_PIN					GPIO_PC3
		#define 	LED2_PIN					GPIO_PC2

		#define 	LED1_PWM					PWM1_ID  //PWM2_ID
		#define 	LED2_PWM					PWM0_ID  //PWM2_ID


		#define		RELAY1_PIN					GPIO_PD2
		#define		RELAY2_PIN					GPIO_PD3
		#define		x2_Touch_Sw_BROADCRARD_2		0x07
	#else
		#define 	DETECT_ZERO_PIN				GPIO_PB6

		#define		BT1_PIN						GPIO_PC1
		#define		BT2_PIN						GPIO_PC4

		#define 	LED1_PIN					GPIO_PC2 //GPIO_PC4
		#define 	LED2_PIN					GPIO_PC3

		#define 	LED1_PWM					PWM0_ID  //PWM2_ID
		#define 	LED2_PWM					PWM1_ID  //PWM2_ID


		#define		RELAY1_PIN					GPIO_PD3
		#define		RELAY2_PIN					GPIO_PD2
		#define		x2_Touch_Sw_BROADCRARD_2		0x02
	#endif


//	#define		x2_Touch_Sw_BROADCRARD_0		0x02
//	#define		x2_Touch_Sw_BROADCRARD_1		0x04
//
//	#define		x2_Touch_Sw_BROADCRARD_3		0x00
//	#define		x2_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x2_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x2_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x2_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x2_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x2_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x2_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x2_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x2_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x2_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x2_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x2_Touch_Sw_BROADCRARD_7
#endif

#if(TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH)
	#define 	NUM_OF_ELEMENT		 		3

	#if(HARDWARE_VERSION == HARDWARE_V2)
		#define 	DETECT_ZERO_PIN				GPIO_PB6

		#define		BT1_PIN						GPIO_PC4
		#define		BT2_PIN						GPIO_PC1
		#define		BT3_PIN						GPIO_PB5

		#define 	LED1_PIN					GPIO_PC3 //GPIO_PC4
		#define 	LED2_PIN					GPIO_PB1
		#define 	LED3_PIN					GPIO_PC2

		#define 	LED1_PWM					PWM1_ID  //PWM2_ID
		#define 	LED2_PWM					PWM4_ID  //PWM2_ID
		#define 	LED3_PWM					PWM0_ID  //PWM2_ID

		#define		RELAY1_PIN					GPIO_PD2
		#define		RELAY2_PIN					GPIO_PD3
		#define		RELAY3_PIN					GPIO_PD4
		#define		x3_Touch_Sw_BROADCRARD_2		0x08
	#else
		#define 	DETECT_ZERO_PIN				GPIO_PB6
		#define		BT1_PIN						GPIO_PC4
		#define		BT2_PIN						GPIO_PC1
		#define		BT3_PIN						GPIO_PB5

		#define 	LED1_PIN					GPIO_PC3 //GPIO_PC4
		#define 	LED2_PIN					GPIO_PB1
		#define 	LED3_PIN					GPIO_PC2

		#define 	LED1_PWM					PWM1_ID  //PWM2_ID
		#define 	LED2_PWM					PWM4_ID  //PWM2_ID
		#define 	LED3_PWM					PWM0_ID  //PWM2_ID

		#define		RELAY1_PIN					GPIO_PD2
		#define		RELAY2_PIN					GPIO_PD3
		#define		RELAY3_PIN					GPIO_PD4

//		#define 	DETECT_ZERO_PIN				GPIO_PB6
//
//		#define		BT1_PIN						GPIO_PC4 //GPIO_PB5
//		#define		BT2_PIN						GPIO_PC1
//		#define		BT3_PIN						GPIO_PB5 //GPIO_PC4
//
//		#define 	LED1_PIN					GPIO_PC2 //GPIO_PB1
//		#define 	LED2_PIN					GPIO_PC3
//		#define 	LED3_PIN					GPIO_PB1 //GPIO_PC2
//
//		#define 	LED1_PWM					PWM0_ID //PWM4_ID  //PWM2_ID
//		#define 	LED2_PWM					PWM1_ID  //PWM2_ID
//		#define 	LED3_PWM					PWM4_ID //PWM0_ID  //PWM2_ID
//
//		#define		RELAY1_PIN					GPIO_PD2 //GPIO_PD4
//		#define		RELAY2_PIN					GPIO_PD3
//		#define		RELAY3_PIN					GPIO_PD4 //GPIO_PD2
		#define		x3_Touch_Sw_BROADCRARD_2		0x03
	#endif


//	#define		x3_Touch_Sw_BROADCRARD_0		0x02
//	#define		x3_Touch_Sw_BROADCRARD_1		0x04
//
//	#define		x3_Touch_Sw_BROADCRARD_3		0x00
//	#define		x3_Touch_Sw_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
//	#define		x3_Touch_Sw_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
//	#define		x3_Touch_Sw_BROADCRARD_6		VERSION_FIRM_H
//	#define		x3_Touch_Sw_BROADCRARD_7		VERSION_FIRM_L
//
//	#define		Device_BROADCRARD_0		x3_Touch_Sw_BROADCRARD_0
//	#define		Device_BROADCRARD_1		x3_Touch_Sw_BROADCRARD_1
//	#define		Device_BROADCRARD_2		x3_Touch_Sw_BROADCRARD_2
//	#define		Device_BROADCRARD_3		x3_Touch_Sw_BROADCRARD_3
//	#define		Device_BROADCRARD_4		x3_Touch_Sw_BROADCRARD_4
//	#define		Device_BROADCRARD_5		x3_Touch_Sw_BROADCRARD_5
//	#define		Device_BROADCRARD_6		x3_Touch_Sw_BROADCRARD_6
//	#define		Device_BROADCRARD_7		x3_Touch_Sw_BROADCRARD_7
#endif

#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
	#define 	NUM_OF_ELEMENT		 		1
	#define BT1_PIN			GPIO_PC3
	#define DETECT_ZERO_PIN	GPIO_PB4
	#define	LED1_PIN		GPIO_PC2
	#define LED2_PIN		GPIO_PC4


	#define	 RED_LED		LED2_PIN
	#define  GREEN_LED		LED1_PIN	// o cam keo dai k co

	#define  LED_SOCKET_ON		0
	#define  LED_SOCKET_OFF		1

	#define  RELAY1_PIN		GPIO_PB5

	#define		x1_Socket_BROADCRARD_0		0x02
	#define		x1_Socket_BROADCRARD_1		0x06
	#define		x1_Socket_BROADCRARD_2		0x01
	#define		x1_Socket_BROADCRARD_3		0x00
	#define		x1_Socket_BROADCRARD_4		0x00
	#define		x1_Socket_BROADCRARD_5		0x00
	#define		x1_Socket_BROADCRARD_6		VERSION_FIRM_H
	#define		x1_Socket_BROADCRARD_7		VERSION_FIRM_L

	#define		Device_BROADCRARD_0		x1_Socket_BROADCRARD_0
	#define		Device_BROADCRARD_1		x1_Socket_BROADCRARD_1
	#define		Device_BROADCRARD_2		x1_Socket_BROADCRARD_2
	#define		Device_BROADCRARD_3		x1_Socket_BROADCRARD_3
	#define		Device_BROADCRARD_4		x1_Socket_BROADCRARD_4
	#define		Device_BROADCRARD_5		x1_Socket_BROADCRARD_5
	#define		Device_BROADCRARD_6		x1_Socket_BROADCRARD_6
	#define		Device_BROADCRARD_7		x1_Socket_BROADCRARD_7
#endif

#define 				TYPE_MAINDEVICE 	 0x02
#define					TYPE_SWITCH			 0x02
#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
#define					TYPE_TOUCH_SWITCH	 0x04
#endif
#if(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)
#define					TYPE_TOUCH_SWITCH	 0x03
#endif
#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)
#define					TYPE_TOUCH_SWITCH	 0x02
#endif
#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
#define					TYPE_TOUCH_SWITCH	 0x01
#endif
#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
#define					TYPE_TOUCH_SWITCH	 0x00
#endif



#define		BT1_READ		gpio_read(BT1_PIN)
#define		BT2_READ		gpio_read(BT2_PIN)
#define		BT3_READ		gpio_read(BT3_PIN)
#define		BT4_READ		gpio_read(BT4_PIN)
#define		BT5_READ		gpio_read(BT5_PIN)

#if(NUM_OF_ELEMENT == 1)
	#define	ARR_BUTTON_PIN			{BT1_PIN}
	#define	ARR_RELAY_PIN 			{RELAY1_PIN}

	#if(TYPE_LED == PWM_LED)
		#define	ARR_LED_PIN 		{LED1_PIN}
		#define ARR_LED_PWM			{LED1_PWM}
	#endif
#endif
#if(NUM_OF_ELEMENT == 2)
	#define ARR_BUTTON_PIN			{BT1_PIN, BT2_PIN}
	#define ARR_RELAY_PIN 			{RELAY1_PIN, RELAY2_PIN}

	#if(TYPE_LED == PWM_LED)
		#define	ARR_LED_PIN 		{LED1_PIN, LED2_PIN}
		#define ARR_LED_PWM			{LED1_PWM, LED2_PWM}
	#endif
#endif
#if(NUM_OF_ELEMENT == 3)
	#define ARR_BUTTON_PIN			{BT1_PIN, BT2_PIN, BT3_PIN}
	#define ARR_RELAY_PIN 			{RELAY1_PIN, RELAY2_PIN, RELAY3_PIN}
	#if(TYPE_LED == PWM_LED)
		#define	ARR_LED_PIN 		{LED1_PIN, LED2_PIN, LED3_PIN}
		#define ARR_LED_PWM			{LED1_PWM, LED2_PWM, LED3_PWM}
	#endif
#endif
#if(NUM_OF_ELEMENT == 4)
	#define ARR_BUTTON_PIN			{BT1_PIN, BT2_PIN, BT3_PIN, BT4_PIN}
	#define	ARR_RELAY_PIN 			{RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN}
#endif

#if(NUM_OF_ELEMENT == 5)
	#define ARR_BUTTON_PIN			{BT1_PIN, BT2_PIN, BT3_PIN, BT4_PIN, BT5_PIN}
	#define	ARR_RELAY_PIN 			{RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN, RELAY5_PIN}
	#if(TYPE_LED == PWM_LED)
		#define	ARR_LED_PIN 		{LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, LED5_PIN}
		#define ARR_LED_PWM			{LED1_PWM, LED2_PWM, LED3_PWM, LED4_PWM, LED5_PWM}
	#endif
#endif

#define 	UART_ON 					1
#define 	CLOCK_TIME_OUT_PRESS_K9B_MS	500
#define		TIME_OUT_RESET				3000					// 4000 loop break pressResetCount
#define		COUNT_DETECT_POWER_OFF		7						// 2ms (timer1 interrupt) * 6 = 12ms zero
#define		NUM_CHECK_DETECH_MAX		150
#define 	DETECT_ZERO_OFF				0
#define		DETECT_ZERO_BY_ADC			1
#define 	DETECT_ZERO_BY_FAILING		2

#define 	METHOD_DETECT_ZERO			DETECT_ZERO_BY_FAILING //DETECT_ZERO_BY_FAILING

#define 	TIME_HOLD_BT				(500)

#define 	TSS_DEFAULT					0x0005

typedef enum
{
	Relay_On		= 0x01U,
	Relay_Off		= 0x00U
} Relay_Stt_Type;

typedef enum {
	RD_EN    = 0x01,
	RD_DIS   = 0x00
} RD_En_type;

typedef enum {
	RD_Success    = 0x01,
	RD_Failure    = 0x00
} RD_Result_Type;

typedef enum {
	RD_PowUpOff    = 0x00,
	RD_PowUpOn     = 0x01,
	RD_PowUpStore  = 0x02
} RD_PowUp_Type;

typedef enum
{
	Button_None		= 0x00U,
	Button_Press	= 0x01U,
	Button_Keeping	= 0x02U
} Button_Stt_Type;

typedef struct{
	uint32_t countLastOfMacID[MAX_NUM_K9ONOFF];
} K9bOnOff_countLast_st;

typedef struct{
	uint32_t countLastOfMacID[MAX_NUM_K9BHC];
} K9bHc_countLast_st;
typedef struct
{
	uint8_t OTA_Flag;   // check in OTA mode. Note

} Sw_Working_Stt_Str;


typedef enum{
    NONE_FUN = 0x00,
    TOGGLE_FUN  = 0x01,
    OFF_FUN     = 0x02,
    ON_FUN      = 0x03
} K9B_Function_Button_t;

typedef struct
{
	uint8_t Pair_K9BOnOff_Flag;
	uint8_t Button_K9BOnOff_Pair;
	uint8_t Pair_K9BHc_Flag;
	uint16_t Add_K9B_HCSet;			// HC set add for K9B remote
	uint32_t ClockTimeSetHc_ms;
	uint32_t Clock_BtK9BStartPress_ms[3];
	uint8_t  Bt_K9B_CountPress[3];
} Sw_Woring_K9B_Str;

typedef struct
{
	uint32_t MacOld;
	uint32_t MacNow;
	uint8_t  Button_ID;
	uint8_t  ButtonPressCounter;
	uint32_t  ClockTimePress_ms;
} Sw_press_K9BHC_Str;
typedef struct 
{
	uint32_t Time_Last_Update_ms;
	uint8_t Update_Stt_Flag[5];
} Sw_Update_Stt_Str;

typedef struct
{
	uint16_t CycleBlink;
	uint32_t Count_Toggle;
	uint32_t Last_ClockTime_Toggle;
}	Sw_SingleBlink_Str;

typedef struct
{
	uint32_t Time_Start_CountDown_s;
	uint32_t Time_CountDown_s;
	uint8_t  Taget;
} CountDown_Str;
extern Button_Stt_Type button1_Stt;
extern Button_Stt_Type button2_Stt;
extern Button_Stt_Type button3_Stt;
extern Button_Stt_Type button4_Stt;
extern Button_Stt_Type button5_Stt;

extern uint8_t		Button1_Hold_Flag;
extern uint8_t		Button2_Hold_Flag;
extern uint8_t		Button3_Hold_Flag;
extern uint8_t		Button4_Hold_Flag;
extern uint8_t		Button5_Hold_Flag;

extern 	uint8_t Kick_all_Flag;
extern Sw_Working_Stt_Str   Sw_Working_Stt_Val;
extern Relay_Stt_Type	relay_Stt[5];
extern CountDown_Str   CountDown_Val[5];

extern uint8_t 			countPower;
extern uint8_t			powerSaved;
extern uint8_t 			Traing_Loop;

#define SETMAX(Val_SET, Num_Max) 	Val_SET = (Val_SET > Num_Max) ? Num_Max:Val_SET   
#define SETMIN(Val_SET, Num_Min) 	Val_SET = (Val_SET < Num_Min) ? Num_Min : Val_SET

void RD_SwitchAC4Ch_Init(void);
void RD_SwitchAC4Ch_Touch(void);

/*
 * Loop for Swtich
 * +Scan touch Button
 * + update status of Button, led, relay,...
 */
void RD_SwitchAC4Ch_Loop(void);


// Scan button for IC SGL8022K
void RD_SwitchAC4Ch_ScanB_V2(void);
// Scan relay stt anh control led and relay Pin
void RD_SwitchAC4Ch_UpdateCtr(void);

void RD_SwitchAC4Ch_OnOffZero(GPIO_PinTypeDef RelayPin, Relay_Stt_Type OnOff_State);

void RD_SwitchAC4Ch_ScanReset(void);
/*
 *  call in loop
 *  Scan val blink led
 *  set val of blink led by RD_SwitchAc4Ch_BlinkScan
 */
void RD_SwitchAc4Ch_BlinkScan(void);
/*
 * set blink all 4 led
 * cycle: int count cycle blink
 * countToggle: time of 1 cycle, Recommend 100,
 */
void RD_SwitchAc4Ch_BlinkSet(uint8_t cycle, uint16_t countToggle);

/**
 * @brief Set blink Single led
 * 
 * @param ID_Led 1-2-3-4 led id set blink
 * @param cycle  : number of 1/2 cycle blink
 * @param countToggle : time tongle (recomem 200ms )
 */
void RD_Sw_SingleBlinkSet(uint8_t ID_Led, uint16_t cycle, uint16_t countToggle);
// Check Button Hold
void RD_SwitchAC4Ch_CheckHold(void);

// hardware Reset mesh- and delete Flash data
void RD_Switchx4_FactoryReset(void);

// GW Reset mesh- and delete Flash data
void RD_Switchx4_GW_FacReset(void);

void RD_Socket_WaitDetechZero(void);

// Auto train factory mode
//void RD_Train_Factory(void);

// Scan Kick_All flag. white 60s and reseet mesh
void RD_ScanKickAll(void);

void RD_Check_RST_Touch_IC(void);

void RD_Train_Factory(void);

void RD_SetAndRsp_Switch(int Light_index, u8 OnOff_Set, uint16_t GW_Add_Rsp_G_onoff);

void RD_Check_Reset_By_Secure(void);

void RD_CheckActiveCountDown(void);
/**
 * @brief Set countdown state and start count in time call function
 * 
 * @param Light_index : 0-4; light index of device
 * @param Time_Count_s : 0-65535  millsecond count 
 * @param Taget : 0-off,  1-on, 2-Toggle
 */
void RD_CountDown_Set(uint8_t Light_index, uint32_t Time_Count_s, uint8_t Taget);

/**
 * @brief set update relay stt. oxFF is update all
 * 
 * @param Relay_ID : 1->4 relay ID
 */
void RD_Set_UpdateStt(uint8_t Relay_ID);

void RD_SetSwitchToggle(uint8_t Light_index_set);

/*
 * OnOffSet: 0-1-2   on-off-toggle
 */
void RD_SetSwitch(uint8_t Light_index_set, uint8_t OnOffSet, uint16_t Gw_Add_Rsp);
/*
 * return Button_ID 1->4 if Pair_Flag on,
 */
uint8_t RD_K9B_Pair_OnOffGetFlag(void);
/*
 * uint8_t Button_ID 1-->4
 */
void RD_K9B_Pair_OnOffClearFlag(void);
void RD_K9B_ScanPairOnOff(void);
void RD_K9B_SaveOnOff(uint32_t macDevice, uint8_t key);
uint8_t RD_K9B_ScanOnOff(uint32_t macDevice, uint8_t key, uint32_t par_signature);
/*
 * Scan on-off new format for RD - use 32 bit counter to set Fun button
 */
uint8_t RD_K9B_ScanOnOffV2(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint32_t bt_function);

uint8_t RD_K9B_ScanLocal(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint32_t bt_function, uint8_t type_remote);
uint8_t RD_K9B_ScanLocalV1(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint32_t bt_function, uint8_t type_remote);
void RD_K9B_PairHCSet(uint8_t pairStt, uint16_t K9BAdd);
void RD_K9B_ScanPairHC(void);
uint8_t RD_K9B_ScanPress2HC(uint32_t macDevice, uint8_t key, uint32_t par_signature);
void RD_K9B_TimeOutScanK9BHC(void);
void RD_K9B_CheckSaveMacHc(uint32_t K9BMac_Buff, uint8_t Num_Of_Button);
void RD_K9B_CheckScanK9BHc(uint32_t K9BMac_Buff, uint8_t Num_Of_Button, s8 rssi);
#endif /* RD_SWITCH_H_ */

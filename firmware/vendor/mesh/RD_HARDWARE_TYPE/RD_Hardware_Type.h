/*
 * RD_Hardware_Type.h
 *
 *  Created on: 2 Mar 2022
 *      Author: HAO DANG
 */

#ifndef RD_HARDWARE_TYPE_H_
#define RD_HARDWARE_TYPE_H_
/*------------------------------Type hardware device --------------------------------*/
#define	X1_TOUCH_SWITCH				1
#define	X2_TOUCH_SWITCH				2
#define	X3_TOUCH_SWITCH				3
#define	X4_TOUCH_SWITCH				4
#define	HIPOW_TOUCH_SWITCH			5
#define	X1_SOCKET					6
#define X1_MACHINE_SWTCH			7
#define X2_MACHINE_SWTCH			8
#define X3_MACHINE_SWTCH			9

#define X5_CEILING_SWITCH			10
#define X3_CEILING_SWITCH			11
#define X2_CEILING_SWITCH			12


#define	TYPE_HARDWARE_SWITCH		X2_TOUCH_SWITCH//RD_EDIT A Set for TYPE hardware

/*--------------------------------- Hardware Version ---------------------------------------------------*/
#define HARDWARE_VERSION			HARDWARE_V2

#define HARDWARE_V1					1
#define HARDWARE_V2					2

/*--------------------------------- PCB shape ---------------------------------------------------*/
#define SQUARE_PCB	                1
#define RECTANGLE_PCB	            2
#define PCB_SHAPE                   RECTANGLE_PCB	// only touch switch



#if(TYPE_HARDWARE_SWITCH < X5_CEILING_SWITCH) // sw touch/machine
#if(HARDWARE_VERSION == HARDWARE_V1)
#	define  	VERSION_FIRM_H				0x01   // sw v1
#	define  	VERSION_FIRM_L				0x0C
#endif
#if(HARDWARE_VERSION == HARDWARE_V2)
#	define  	VERSION_FIRM_H				0x01   // sw new cover
#	define  	VERSION_FIRM_L				0x01
#endif
#else
#	define  	VERSION_FIRM_H				0x01  // celling sw
#	define  	VERSION_FIRM_L				0x02 //  add K9B off all
#endif


/*--------------------------------- Switch Line ---------------------------------------------------*/
#define TOUCH_SWITCH	            1
#define MACHINE_SWITCH	            2
#define CELLING_SWITCH				3

#if((TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH))
#	define SWITCH_LINE                   CELLING_SWITCH
#elif((TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH))
#	define SWITCH_LINE                   TOUCH_SWITCH
#elif((TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH) || (TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH) || (TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH) )
#	define SWITCH_LINE                   MACHINE_SWITCH
#endif




#define SECURE_ON					1
//#define MACHINE_HARDWARE_V2_ON		1				// PCB xoay huong

/*--------------- convert type device from 1 switch to Hi power switch-------------------------*/
#define	CONVERT_X1_TO_HIPOW_ON      0

#define CYCLE_ACTIVE_BUTTON			7 // num of read CYCLE_ACTIVE_BUTTON times active iss press
#define CYCLE_READ_BT_MS			5 // time of 1 cycle check read bt
// time scan = CYCLE_ACTIVE_BUTTON*CYCLE_READ_BT_MS

#if(HARDWARE_VERSION == HARDWARE_V2)
#	define FAST_PROVISION_EN_RD			1
#	define K9B_REMOTE_SUPPORT_ON		1
#	define CONFIG_POWUP_EN				1
#else
#	define FAST_PROVISION_EN_RD			0
#	define K9B_REMOTE_SUPPORT_ON		1
#	define CONFIG_POWUP_EN				1
#endif

#	define MAX_NUM_K9BHC				5
#	define MAX_NUM_K9ONOFF				5
#	define MAX_MUM_K9BPRESSTYPE			12 //Ver 2 7 // 3 button -> 7 type press 001-010-100-110-011-101-111

#if((TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH) || (TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH))
#	define K9B_RD_MESS_EN				1		// new format of RD
#	define K9B_RD_SCENE_LOCAL_EN		1
#	define K9B_TYPE_CTR_LOCAL_DF		K9B_CTR_LOCAL_ONOFF
#else
#	define K9B_RD_MESS_EN				0		// new format of RD
#	define K9B_TYPE_CTR_LOCAL_DF		K9B_CTR_LOCAL_ONOFF
#endif
#	define K9B_BLE_CID_ATM				0x09E2 // atomosic CID ble
#	define K9B_BLE_CID_RD				0x2804


#define K9B_CTR_LOCAL_ONOFF				1	// Mode V1 toggle anf offall button 6
#define K9B_CTR_LOCAL_SCENE				2	// Mode V2 control as Scene offall button 6



/* new hardware 10/2023  */
/*
#define	X1_TOUCH_SWITCH_V2			10
#define	X2_TOUCH_SWITCH_V2			11
#define	X3_TOUCH_SWITCH_V2			12
#define	X4_TOUCH_SWITCH_V2			13
#define X1_MACHINE_SWTCH_V2			14
#define X2_MACHINE_SWTCH_V2			15
#define X3_MACHINE_SWTCH_V2			16
*/
/*------------------------------ Type button in device --------------------------------*/
#define	TOUCH_BUTTON                1
#define MACHINE_BUTTON              2

#if(TYPE_HARDWARE_SWITCH <= HIPOW_TOUCH_SWITCH)   //  x1-x4 HiPow touch switch
    #define TYPE_BUTTON             TOUCH_BUTTON
#else
    #define TYPE_BUTTON             MACHINE_BUTTON
#endif

/*------------------------------Type LED in device ------------------------------------*/
#define RGB_LED                     1   // led rgb LC8823
#define PWM_LED                     2   // sigle color led and control by PWM signal
#define RED_GREEN_LED               3   // led red and green control by IO signal

#if(TYPE_HARDWARE_SWITCH <= HIPOW_TOUCH_SWITCH)   //  x1-x4 HiPow touch switch
    #define TYPE_LED				RGB_LED
#else
    #if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
        #define TYPE_LED             RED_GREEN_LED
    #else
        #define TYPE_LED             PWM_LED
    #endif
#endif

#if(TYPE_HARDWARE_SWITCH >= X5_CEILING_SWITCH)
#	define FACTORY_RESET_BY_POW_EN		1
#else
#	define FACTORY_RESET_BY_POW_EN		0
#endif
/*--------- type PCB version (in this firm, which has only 1 PCB version)----------------------*/
#define	PCB_V1						 1
#define	PCB_VERSION					 PCB_V1


/*------------------------------ Number of led in 1 button in RGB lec---------------------------*/
    #define	PCB_LED_1				1
    #define	PCB_LED_2				2

#if(TYPE_LED == RGB_LED)

    #define	PCB_LED_VER				PCB_LED_2
#else
	#define PCB_LED_NON				3
	#define	PCB_LED_VER				PCB_LED_NON
#endif


/*-------------------------------- count down -----------------------------*/
#if((X1_MACHINE_SWTCH == TYPE_HARDWARE_SWITCH) || (X1_TOUCH_SWITCH == TYPE_HARDWARE_SWITCH) || (X2_CEILING_SWITCH == TYPE_HARDWARE_SWITCH))
#	define COUNT_DOWN_EN 1
#else
#	define COUNT_DOWN_EN 0
#endif

#define 	RD_PROVIDER					0x0001
#define 	VHT_PROVIDER				0x0200
#define 	HUE_PROVIDER				0x0201
#define 	PROVIDER_ID					RD_PROVIDER
//
//#	define  	VERSION_FIRM_H				0x01  // celling sw
//#	define  	VERSION_FIRM_L				0x02 //  add K9B off all

#define		Device_BROADCRARD_0		0x02	//Switch
#define		Device_BROADCRARD_3		0x00
#define		Device_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
#define		Device_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
#define		Device_BROADCRARD_6		VERSION_FIRM_H
#define		Device_BROADCRARD_7		VERSION_FIRM_L



#if(SWITCH_LINE == MACHINE_SWITCH)
#	define		Device_BROADCRARD_1		0x04
#else
#	define		Device_BROADCRARD_1     0x02	// touch and celling
#endif

/*------------------------- code for device ---------------------------------*/

#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x1A
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#   	define		Device_BROADCRARD_2		0x1B
	#	endif
	#else
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x0C
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#		define		Device_BROADCRARD_2		0x13
	#	endif
	#endif
#elif(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x1C
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#   	define		Device_BROADCRARD_2		0x1D
	#	endif
	#else
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x0D
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#		define		Device_BROADCRARD_2		0x14
	#	endif
	#endif
#elif(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x1E
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#   	define		Device_BROADCRARD_2		0x1F
	#	endif
	#else
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x0E
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#		define		Device_BROADCRARD_2		0x15
	#	endif
	#endif
#elif(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x20
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#   	define		Device_BROADCRARD_2		0x21
	#	endif
	#else
	#	if(PCB_SHAPE == RECTANGLE_PCB)
	#		define		Device_BROADCRARD_2		0x0F
	#	endif
	#	if(PCB_SHAPE == SQUARE_PCB)
	#		define		Device_BROADCRARD_2		0x16
	#	endif
	#endif
#elif(TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	define			Device_BROADCRARD_2		0x06
	#else
	#	define			Device_BROADCRARD_2		0x01
	#endif
#elif(TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	define			Device_BROADCRARD_2		0x07
	#else
	#	define			Device_BROADCRARD_2		0x02
	#endif
#elif(TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH)
	#if(HARDWARE_VERSION == HARDWARE_V2)
	#	define			Device_BROADCRARD_2		0x08
	#else
	#	define			Device_BROADCRARD_2		0x03
	#endif
#elif(TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH)
	#	define			Device_BROADCRARD_2		0x25
#elif(TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH)
	#	define			Device_BROADCRARD_2		0x26
#elif(TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH)
	#	define			Device_BROADCRARD_2		0x27
#endif
#if(FAST_PROVISION_EN_RD)
#	define RD_VID_TYPE 	((VERSION_FIRM_H << 8) | VERSION_FIRM_L)
#	define RD_PID_TYPE 	(0xFFFF & ((Device_BROADCRARD_0 << 12) | (Device_BROADCRARD_1 << 8) | Device_BROADCRARD_2))

#else
//#	define RD_VID_TYPE 	(LIGHT_TYPE_SEL)
#	define RD_PID_TYPE 	(LIGHT_TYPE_SEL) //((Device_BROADCRARD_0 << 12) | (Device_BROADCRARD_1 << 8) | Device_BROADCRARD_2)
#endif

#endif /* RD_HARDWARE_TYPE_H_ */

/*
 * RD_Flash.h
 *
 *  Created on: Jan 20, 2022
 *      Author: Dang_Hao
 */

#ifndef RD_FLASH_H_
#define RD_FLASH_H_

#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#include "../RD_Switch/RD_Switch.h"

#define FLASH_ADDR               		0x78000//0x023000
#define DATA_POWER_OFF_FLASH_ADDR      	0x79000//0x023000
#define FLASH_BUFF_LEN           		91
#define FLASH_DATA_SIZE			 		91
#define FLASH_DATA_POWER_OFF_SIZE		9
#define SCENE_MAX 						5

#define NUM_
extern unsigned char  Buff_Flash_Read[FLASH_BUFF_LEN];
extern unsigned char  Buff_Flash_Write[FLASH_BUFF_LEN] ;

typedef struct
{
	uint8_t Lightness_Off;  // Dim when turn off button
	uint8_t Lightness_On;  // DIM when turn on button
	uint8_t Blue_Gray;
	uint8_t Green_Gray;
	uint8_t Red_Gray;
} Str_RGB_Data;

typedef struct
{
	uint16_t Scene_ID_On; 
	uint16_t Scene_ID_Off;
} Str_Scene_Stair;

typedef struct
{
	uint32_t Time_Start_CountDown_s;
	uint32_t Time_CountDown_s;
	uint8_t  Taget;
} CountDownForFlash_Str; // the same with CountDown_Str in RD_Switch.h

typedef struct
{
  uint32_t MacK9B[MAX_NUM_K9ONOFF];
  uint8_t K9B_BtKey[MAX_NUM_K9ONOFF];
  uint8_t NumSaveNext;  // num off array ID save to struct. NumSaveNext>=MAX_NUM_K9ONOFF NumSaveNext=0;
} RemoteK9BOnOff_Str;

typedef struct
{
  uint32_t MacK9B;
  uint16_t AddK9B;
  uint16_t Scene_ID_OnePress[MAX_MUM_K9BPRESSTYPE];
  uint8_t Button_ID[MAX_MUM_K9BPRESSTYPE];
  //uint16_t Scene_ID_DoublePress[7];
  uint8_t Num_Of_Button;
} RemoteK9BScene_Str;

typedef struct
{
	RemoteK9BOnOff_Str OnOff[5]; // 5 button on switch
	RemoteK9BScene_Str Scene[MAX_NUM_K9BHC]; // Max 3 remote
}RemoteK9B_Str;
typedef struct
{
	uint8_t Factory_Check;
	uint8_t Future[3];
	uint8_t Secure_RD;
	uint16_t Gw_Add;
	Str_RGB_Data	Data_Led[5];
	uint16_t Add_Link_Control[5];   // add  linking control onoff with button
	CountDownForFlash_Str CountDown[5];
	RemoteK9B_Str K9B_Data;
	uint8_t PowerUpStt;
	uint8_t Type_CtrLocal_K9B;
} Sw_Flash_Data;
extern Sw_Flash_Data Sw_Flash_Data_Val;


typedef struct
{
	uint8_t Stt_LastPower[5];
	uint8_t Factory_Check;
	uint8_t	Counter_Eraser[4];		// 32 bit ( max 100.000 )

} Sw_Flash_Data_Save_Power_Off;
Sw_Flash_Data_Save_Power_Off Sw_Flash_Data_Save_Power_Off_Val;
#define SIZE_SCENE_DATA 	6  // 6byte 2sceneID+4relay
// Check and Init Flash
//

/*
 * Eraser Data Flash (Gw_add, GRB config) anf save default value
 */
void RD_Flash_Save_DataDefault(void);
void RD_Flash_Init();
// Delete user Flash for POW-- don't delete when reset mesh
void RD_Flash_CleanSenceFlashPower(unsigned long AddFlashPower);



/*
 * Save data con fig RGB
 * LED_ID:  1-2-3-4  LED fit with button
 * Lightness: 15-->31:    Lightness when relay on
	Blue_Gray:   Gray level of Blue Value Range 0-->256;
	Green_Gray:  Gray level of Green Value Range 0-->256;
	Red_Gray:   Gray level of Red Value Range 0-->256;
 */
//void RD_Flash_SaveRGB_Config(uint8_t LED_ID, uint8_t Lightness, uint8_t Blue, uint8_t Green, uint8_t Red);
void RD_Flash_SaveRGB_Config(uint8_t LED_ID, uint8_t Lightness_On, uint8_t Lightness_Off, uint8_t Blue, uint8_t Green, uint8_t Red);



/*
 * Save new Gateway address
 */
void RD_Flash_SaveGwAdd(uint16_t Gw_Add);

/* save secure value
 * 1 pass value
 * else fail value //recommen fail 0
*/

void RD_Flash_SavePowerOff(void);

void RD_Flash_SaveElementAdd(uint16_t Element_Add, uint8_t Number_off_Element);

void RD_Flash_Save_Secure(uint8_t Secure_Stt);
/**
 * @brief Save add control on/off (stair control)
 * 
 * @param Button_Set : 1->4:  button ID set
 * @param Add_Control: Iadd link with button (element or group)
 */
void RD_Flash_Save_LinkControl(uint8_t Button_Set, uint16_t Add_Control);

/**
 * Light_Index: num element 0-1-2-3
 * Taget_Control:0- off
 * 					1-On
 * 					2_toggle
 * 	TimeCountDown_s : time active, s
 */
void RD_Flash_SetCountDown(uint8_t Light_Index, uint8_t Taget_Control, uint16_t TimeCountDown_s);

/**
 * macDevice: mac of remote K9B, get 4 byte 0->3
 * Key: mode+Button K9B press
 * SwButtonID: 1->4 ButtonID in RD switch
 */
void RD_Flash_SaveK9BOnOff(uint32_t macDevice, uint8_t key, uint8_t SwButtonID);
/**
 * delete data K9B onOff in this button
 * SwButtonID: 1->4 ButtonID in RD switch
 */
void RD_Flash_DeleteAllK9BOnOff(uint8_t SwButtonID);
/**
 * macDevice: mac of remote K9B, get 4 byte 0->3
 * Num_Of_Button: type remote, 1-2-3 button
 * K9BAdd: add Hc set for remote k9b
 *
 * return: 0: fail- full memory
 * 			1: success
 * 			2: fail. duplicate mac of remote K9b saved
 */
uint8_t RD_Flash_SaveK9BHc(uint32_t macDevice, uint8_t Num_Of_Button, uint16_t K9BAdd);

void RD_Flash_K9BSaveScene(uint8_t Button_ID, uint8_t Mode_ID, uint16_t Scene_ID);

/*
 * uint16_t K9BAdd add of K9B remote HC set, delete all = 0xffff
 * return: 1 success
 * 			0 fail
 */
uint8_t RD_Flash_DeleteK9BHC(uint16_t K9BAdd);
/*
 * save Scene for K9B remote
 * K9BAdd: add of k9b remote saved
 * ButtonID: button ID in remote. 1-2-3
 * ModeID: mode press: 1-2
 * SceneID: SceneID set
 *
 * return: 1  success
 * 			0 fail (don't find K9BAdd save in data)
 */
//uint8_t RD_Flash_SaveSceneK9BHC(uint16_t K9BAdd, uint8_t ButtonID, uint8_t ModeID, uint16_t SceneID);
uint8_t RD_Flash_SaveSceneK9BHC(uint16_t K9BAdd, uint8_t ButtonID, uint16_t SceneID);
uint8_t RD_Flash_DeleteSceneK9BHC(uint16_t K9BAdd);

int RD_CheckButtonPosK9BHC(uint8_t ButtonID_Aray[MAX_MUM_K9BPRESSTYPE], uint8_t ButtonID_Check);
void RD_Flash_PowUpConfig(uint8_t PowUpConfig);

void RD_Flash_K9BSaveLocalMode(uint8_t local_mode);
#endif /* RD_FLASH_H_ */

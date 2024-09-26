/*
 * RD_MessData.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Dang_Hao
 */



#include "RD_MessData.h"
//uint8_t Train_Factory =0;
//uint8_t reset_Mess_Flag =0;

uint8_t Train_Factory =0;
extern STT_Relay_Mess STT_Relay_Mess_Config;
extern void mesh_g_onoff_st_rsp_par_fill(mesh_cmd_g_onoff_st_t *rsp, u8 idx);
uint8_t reset_Mess_Flag =0;
unsigned char vr_RD_ProvDone;

uint16_t RD_GATEWAYADDRESS  = GW_ADD_DEFAULT;
uint16_t RD_ELEMENT_ADDR_1	= 0x0000;
uint16_t RD_ELEMENT_ADDR_2	= 0x0000;
uint16_t RD_ELEMENT_ADDR_3	= 0x0000;
uint16_t RD_ELEMENT_ADDR_4	= 0x0000;

/*----------------------------------- E2 mess handle ----------------------------------------*/
//
//#if(TYPE_LED == RGB_LED)
#if(1)
static void RD_Handle_RGB_CONFIG(uint8_t par[8], uint16_t Gw_Add_Buff, uint16_t Receive_add)
{
	//uint8_t LED_ID 			= par[2];				// in switch LED_ID will get from element add
	uint8_t LED_ID   		= Receive_add - ele_adr_primary +1;

	if(par[2] == 0xFF)		LED_ID =0xFF;

	uint8_t Blue_Gray		= par[3];
	uint8_t Green_Gray		= par[4];
	uint8_t Red_Gray		= par[5];
	uint8_t Dim_On			= par[6];		// 0-100%
	uint8_t Dim_Off			= par[7];		//0-100%

	SETMAX(Dim_On, 100);
	SETMIN(Dim_Off, 0);

	#if(TYPE_HARDWARE_SWITCH != X1_TOUCH_SWITCH)
		#if(TYPE_LED == RGB_LED)
			// scale down 40% power of led
			Dim_Off	= Dim_Off/6 +1;
			Dim_On 	= Dim_On/6 +1;   //dim1 is off
			SETMAX(Dim_On, 31);
			SETMIN(Dim_Off, 0);
		#else
			// PWM led don't need scale
			Dim_Off	= Dim_Off;
			Dim_On 	= Dim_On;   //dim1 is off
			SETMAX(Dim_On, 100);
			SETMIN(Dim_Off, 0);
		#endif
		SETMIN(Dim_On, Dim_Off+1);
	#else
		Dim_Off	= Dim_Off/3 +1;	
		Dim_On 	= Dim_On/3 +1;   //dim1 is off
		SETMAX(Dim_On, 31);
		SETMIN(Dim_Off, 0);
		SETMIN(Dim_On, Dim_Off+1);
	#endif




	if(get_provision_state() == STATE_DEV_UNPROV)		// factory test mode
	{
		if(LED_ID != 0xFF)								// change all
		{
			LED_Switch_Data.LED[LED_ID -1].Blue_Gray 	= Blue_Gray;
			LED_Switch_Data.LED[LED_ID -1].Green_Gray	= Green_Gray;
			LED_Switch_Data.LED[LED_ID -1].Red_Gray		= Red_Gray;

			// update but don't save to flash
			Sw_Flash_Data_Val.Data_Led[LED_ID -1].Lightness_On 	= Dim_On;
			Sw_Flash_Data_Val.Data_Led[LED_ID -1].Lightness_Off = Dim_Off;
			Sw_Flash_Data_Val.Data_Led[LED_ID -1].Blue_Gray		= Blue_Gray;
			Sw_Flash_Data_Val.Data_Led[LED_ID -1].Green_Gray	= Green_Gray;
			Sw_Flash_Data_Val.Data_Led[LED_ID -1].Red_Gray		= Red_Gray;

			LED_Set_OnOff_Stt(LED_ID, relay_Stt[LED_ID -1]);
		}
		else // all
		{
			for(int i = 0; i <5; i++)
			{
				// update but don't save to flash
				LED_Switch_Data.LED[i].Blue_Gray 	= Blue_Gray;
				LED_Switch_Data.LED[i].Green_Gray	= Green_Gray;
				LED_Switch_Data.LED[i].Red_Gray		= Red_Gray;

				Sw_Flash_Data_Val.Data_Led[i].Lightness_On 	= Dim_On;
				Sw_Flash_Data_Val.Data_Led[i].Lightness_Off = Dim_Off;
				Sw_Flash_Data_Val.Data_Led[i].Blue_Gray		= Blue_Gray;
				Sw_Flash_Data_Val.Data_Led[i].Green_Gray	= Green_Gray;
				Sw_Flash_Data_Val.Data_Led[i].Red_Gray		= Red_Gray;

				LED_Set_OnOff_Stt(i+1, relay_Stt[i]);
			}
		}
	}
	else
	{
		RD_Flash_SaveRGB_Config(LED_ID, Dim_On, Dim_Off, Blue_Gray, Green_Gray, Red_Gray);
	}

	LED_Update_Stt();
	mesh_tx_cmd2normal(RD_OPCODE_SCENE_RSP, par, 8, Receive_add, Gw_Add_Buff, RD_MAXRESPONESEND);
}

#endif
#if 0
// handle mess config HSL for button 
static void RD_Handle_HSL_CONFIG(uint8_t par[8], uint16_t Gw_Add_Buff, uint16_t Receive_add)
{
	// uint8_t LED_ID 		= par[2];
	// uint8_t Lightness	= par[3];
	// uint8_t Blue_Gray	= par[4];
	// uint8_t Green_Gray	= par[5];
	// uint8_t Red_Gray	= par[6];
	uint8_t LED_ID = 1;
	LED_ID   = Receive_add - ele_adr_primary +1;
	uint16_t  Hue;
	uint16_t  Saturation;
	uint16_t  Lightness;
	uint8_t   Dim = 31;
	Hue				= (par[3]<< 8 | par[2]);
	Hue				= (Hue*360)/65535;			// maping 360* --> 65535
	SETMAX(Hue,360);							// max limit í 360*

	Saturation		= (par[5]<< 8 | par[4]);
	Lightness		= (par[7]<< 8 | par[6]);

	struct RGB RGB_Set;
	struct HSL HSL_In;
	HSL_In.H	= Hue;
	HSL_In.S	= (float) Saturation/65535.0;	// maping 65535 -> 0.00-->1.00
	HSL_In.L	= (float) Lightness/65535.0;

	RGB_Set = HSLToRGB(HSL_In);

	RD_Flash_SaveRGB_Config(LED_ID, Dim, RGB_Set.B, RGB_Set.G, RGB_Set.R);
	LED_Update_Stt();
	mesh_tx_cmd2normal(RD_OPCODE_SCENE_RSP, par, 8,Receive_add , Gw_Add_Buff, RD_MAXRESPONESEND);
}
#endif
static void RD_Handle_CTR_SW(uint8_t par[8], uint16_t Gw_Add_Buff)
{

	uint8_t ButtonCtr_Buff 	= par[2];
	uint8_t Control_Stt		= par[3];

	//Control_Stt = 1 ? Control_Stt : (Control_Stt >1);
	Control_Stt   = (Control_Stt >1) ? 1:Control_Stt;
	if(ButtonCtr_Buff == 0xFF) // control All
	{
		for(int i=0; i<NUM_OF_ELEMENT; i++)
		{
			RD_SetAndRsp_Switch(i, Control_Stt, Gw_Add_Buff);
		}
	}
	else						// control single
	{
		ButtonCtr_Buff   = (ButtonCtr_Buff > 5) ? 5:ButtonCtr_Buff;

		RD_SetAndRsp_Switch(ButtonCtr_Buff -1, Control_Stt, Gw_Add_Buff);
	}

	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Control SW: %x-%x-%x-%x-%x \n",relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3], relay_Stt[4]);
		uart_CSend(UART_TempSend);
	#endif
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
}

static void RD_Handle_Train(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	if((clock_time_s() > DELAY_TRAIN_TIME) && (Traing_Loop == 0))
	{
		uart_CSend("start Train\n");
		Traing_Loop =1;
		#if(TYPE_LED == RGB_LED)									// set white LED when training
			if(get_provision_state() == STATE_DEV_UNPROV)
			{
				for(int i = 0; i <4; i++)
				{
					LED_Switch_Data.LED[i].Blue_Gray 	= 0xff;
					LED_Switch_Data.LED[i].Green_Gray	= 0xff;
					LED_Switch_Data.LED[i].Red_Gray		= 0xff;
					LED_Set_OnOff_Stt(i+1, relay_Stt[i]);
				}
			}
		#endif //
		Save_Scene_Mess_Config.Header[0]    = RD_HEADER_SW_TOUCH_TRAIN & 0xff;
		Save_Scene_Mess_Config.Header[1]	= RD_HEADER_SW_TOUCH_TRAIN >>8;
		Save_Scene_Mess_Config.Relay_Stt[0] = 0x00;
		Save_Scene_Mess_Config.Relay_Stt[1] = 0x00;
		Save_Scene_Mess_Config.Relay_Stt[2] = 0x00;
		Save_Scene_Mess_Config.Relay_Stt[3] = 0x00;

		uint8_t* Mess_Buff;
		Mess_Buff = (uint8_t *) (&Save_Scene_Mess_Config);
		mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 6, Gw_Add_Buff, RD_MAXRESPONESEND);
		Train_Factory =1;
		RD_SwitchAc4Ch_BlinkSet(5, 100);
	}
}

#if 0
/**
 * @brief Setup Stair Scene for button
 * 
 * @param par 
 * @param Gw_Add_Buff 
 */
static void RD_Handle_SetUp_LinkControl(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint8_t Button_Set;
	uint16_t Add_Link_Control;

	Button_Set 		= par[2];
	Add_Link_Control 	= par[4]<<8 | par[3];

	RD_Flash_Save_LinkControl(Button_Set, Add_Link_Control);

	RD_SwitchAc4Ch_BlinkSet(5, 100);
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 5, Gw_Add_Buff, RD_MAXRESPONESEND);
}
#endif
static void RD_Handle_SetUp_LinkControl2(uint8_t par[8], uint16_t Gw_Add_Buff, uint16_t Receive_add)
{

	uint8_t Button_Set = 1;
	Button_Set   = Receive_add - ele_adr_primary +1;	// Switch Add to button

//	Button_Set	 = SETMAX(Button_Set, 4);
//	Button_Set	 = SETMIN(Button_Set, 1);

	uint16_t Add_Link_Control;
	Add_Link_Control 	= par[3]<<8 | par[2];

	if(Add_Link_Control == 0xffff)	Add_Link_Control =0x0000;	
	RD_Flash_Save_LinkControl(Button_Set, Add_Link_Control);

	//RD_SwitchAc4Ch_BlinkSet(5, 100);
	RD_Sw_SingleBlinkSet(Button_Set, 5, 300);
	//mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 4, Gw_Add_Buff, RD_MAXRESPONESEND);
	mesh_tx_cmd2normal(RD_OPCODE_SCENE_RSP, par, 4,Receive_add , Gw_Add_Buff, RD_MAXRESPONESEND);
}

static void RD_Handle_SetUp_CountDown(uint8_t par[8], uint16_t Gw_Add_Buff, uint16_t Receive_add)
{
	uint8_t Relay_Index = Receive_add - ele_adr_primary;
	uint8_t Taget       = 0;
	uint16_t Time_Count_s =0;

	Taget = par[2];
	Time_Count_s = (uint16_t) ( par[4] <<8 | par[3]);

	//RD_CountDown_Set(Relay_Index, Time_Count_s, Taget); V1.09 only save for switch x1 and active when turn on
	RD_Flash_SetCountDown(Relay_Index, Taget, Time_Count_s);

	if(Relay_On == relay_Stt[Relay_Index]) RD_CountDown_Set(Relay_Index, Time_Count_s,  Taget);

	//RD_SwitchAc4Ch_BlinkSet(5, 100);
	RD_Sw_SingleBlinkSet(Relay_Index+1, 5, 100);
	mesh_tx_cmd2normal(RD_OPCODE_SCENE_RSP, par, 8,Receive_add , Gw_Add_Buff, RD_MAXRESPONESEND);
	
}

void RD_Mess_K9BPairHC_Rsp(uint16_t Gw_Add_Buff, uint8_t modeSet, uint16_t K9BAdd, uint8_t Num_Of_Button, uint8_t RspStt)
{
	uint8_t Mess_RspBuff[8] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_PAIR_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_PAIR_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2] =  modeSet;
	Mess_RspBuff[3] = (uint8_t) ((K9BAdd >> 8) & 0xFF);
	Mess_RspBuff[4] =  (uint8_t) (K9BAdd	 	  & 0xFF);
	Mess_RspBuff[5] = Num_Of_Button;
	Mess_RspBuff[6] = RspStt;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 7, Gw_Add_Buff, RD_MAXRESPONESEND);

}
void RD_Mess_ScanK9BHC_Rsp(uint16_t Gw_Add_Buff, uint32_t K9Bmac, uint8_t type, s8 TxPow)
{
	uint8_t Mess_RspBuff[8] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_PAIR_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_PAIR_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2] = (uint8_t) (K9Bmac      & 0xFF);
	Mess_RspBuff[3] = (uint8_t) ((K9Bmac >> 8) & 0xFF);
	Mess_RspBuff[4] = (uint8_t) ((K9Bmac >> 16) & 0xFF);
	Mess_RspBuff[5] = (uint8_t) ((K9Bmac >> 24) & 0xFF);
	Mess_RspBuff[6] = type;
	Mess_RspBuff[7] = TxPow;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 8, Gw_Add_Buff, RD_MAXRESPONESEND);

}
static void RD_Handle_PairK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint8_t  K9BPairHc_Stt = par[2];
	uint16_t K9BAdd_Buff =  (uint16_t)((par[4]<<8) | (par[3]));

	RD_SwitchAc4Ch_BlinkSet(5, 100);

	// save
	if(K9BPairHc_Stt == 0x01){
		RD_K9B_PairHCSet(K9BPairHc_Stt, K9BAdd_Buff); // turn on K9B pair Hc
		// Rsp when receiv K9B mac or over time
	}
	// delete
	else
	{
		uint8_t saveStt = RD_Flash_DeleteK9BHC(K9BAdd_Buff);
		RD_Mess_K9BPairHC_Rsp(Gw_Add_Buff, K9BPairHc_Stt, K9BAdd_Buff, 0x00, saveStt); //RD_Todo add stt rsp
		// RD_Todo: Flash
	}
}
/*
 * Start Scan K9B for HC
 */
static void RD_Handle_ScanK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uart_CSend("Start wating K9B HC \n");
	uint8_t  K9BPairHc_Stt = par[2];
	RD_SwitchAc4Ch_BlinkSet(5, 100);
	RD_K9B_PairHCSet(K9BPairHc_Stt, 0x0000); // turn on K9B pair Hc
}
/*
 * save Mac and add off K9B HC
 */
static void RD_Handle_SaveK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint32_t MacK9B_Buff = (uint32_t) ( (par[5]<<24) | (par[4]<<16) | (par[3]<<8) | (par[2]));
	uint16_t K9BAdd	 	 = (uint16_t) ((par[7] << 8) | par[6]  );
	uint8_t SaveK9B_Stt = RD_Flash_SaveK9BHc(MacK9B_Buff, 3, K9BAdd);// save max button

	uint8_t MessRsp[7];
	for(int i=0; i<6; i++) MessRsp[i] = par[i];

	MessRsp[6] = SaveK9B_Stt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &MessRsp[0], 7, Gw_Add_Buff, RD_MAXRESPONESEND);
	uart_CSend("Save Mac K9BHC \n");
	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_DeleteK9BHC(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t K9BAdd	 	 = (uint16_t) ((par[3] << 8) | par[2]  );
	uint8_t SaveK9B_Stt = RD_Flash_DeleteK9BHC(K9BAdd);

	uint8_t Mess_RspBuff[5] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_DELETEK9BHC 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_DELETEK9BHC >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= K9BAdd 				& 0xFF;
	Mess_RspBuff[3]	= (K9BAdd >> 8) 		& 0xFF;
	Mess_RspBuff[4] =  SaveK9B_Stt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 5, Gw_Add_Buff, RD_MAXRESPONESEND);
	uart_CSend("Delete K9BHC \n");
	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_K9BSaveScene(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t K9BAdd = (uint16_t) ((par[3] << 8) | par[2] );
	uint8_t ButtonID = par[4];
	uint8_t ModeID = par[5];
	uint16_t Scene_ID	= (uint16_t) ((par[7] << 8) | par[6] );
	//RD_Flash_K9BSaveScene(ButtonID, ModeID, Scene_ID);
	//uint8_t SaveStt = RD_Flash_SaveSceneK9BHC(K9BAdd, ButtonID, ModeID, Scene_ID);

	char UART_TempSend[128];
	sprintf(UART_TempSend,"save Scene add:%x  BID:%d Scene ID: 0x%x\n",K9BAdd,  ButtonID, Scene_ID);
	uart_CSend(UART_TempSend);

	uint8_t SaveStt = RD_Flash_SaveSceneK9BHC(K9BAdd, ButtonID, Scene_ID);

	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_SAVESCENE_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_SAVESCENE_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= SaveStt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_K9BDeleteScene(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t K9BAdd 	= (uint16_t) ((par[3] << 8) | par[2] );
	uint8_t ButtonID 	= par[4];
	uint8_t ModeID 		= par[5];
	uint8_t deleteStt=0;
	if( 0xFFFF == K9BAdd ) 		deleteStt = RD_Flash_DeleteSceneK9BHC(K9BAdd); 			// delete all Scene of all remote
	else
	{
		if( 0xff == ButtonID)   deleteStt = RD_Flash_DeleteSceneK9BHC(K9BAdd); 							// delete all scene of 1 remote K9BAdd
		else					deleteStt = RD_Flash_SaveSceneK9BHC(K9BAdd, ButtonID, 0x0000); // delete 1 scene
	}



	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_DELETESCENE_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_DELETESCENE_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= deleteStt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_PowUpConfig(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	RD_PowUp_Type PowUpStt_Config = par[2];
	RD_Flash_PowUpConfig(PowUpStt_Config);

	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_POWUP_CONFIG 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_POWUP_CONFIG >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= PowUpStt_Config;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_K9BLocalModeConfig(u8 *par, uint16_t Gw_Add_Buff)
{
	uint8_t  K9B_LocalMode_Config = par[2];
	RD_Flash_K9BSaveLocalMode(K9B_LocalMode_Config);

	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SET_K9B_LOCAL_MODE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SET_K9B_LOCAL_MODE >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= K9B_LocalMode_Config;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

	RD_SwitchAc4Ch_BlinkSet(10, 200);
}


static void RD_GroupAuto(uint16_t groupID, mesh_cb_fun_par_t *cb_par, uint16_t OpGroup){
	mesh_cb_fun_par_t *cb_par_g = cb_par;
	cb_par_g->op = OpGroup;
	cb_par_g->op_rsp = CFG_MODEL_SUB_STATUS;
	cb_par_g->model_idx = 0;
	uint8_t parGroup[8] = {0};
	if(cb_par_g->adr_dst == 0xFFFF) cb_par_g->adr_dst = ele_adr_primary;

	for(int i=0; i< ELE_CNT; i++)
	{
		if(OpGroup == CFG_MODEL_SUB_ADD){
			parGroup[0] = (cb_par_g->adr_dst+i) & 0xff ;
			parGroup[1] = (cb_par_g->adr_dst+i) >> 8 & 0xff;
			parGroup[2] = groupID & 0xff;
			parGroup[3] = groupID >>8 & 0xff;
			parGroup[4] = 0x00;
			parGroup[5] = 0x10;

			mesh_cmd_sig_cfg_model_sub_set(parGroup, 6, cb_par_g);
		}
		else if(OpGroup == CFG_MODEL_SUB_DEL){
			parGroup[0] = (cb_par_g->adr_dst+i) & 0xff ;
			parGroup[1] = (cb_par_g->adr_dst+i) >> 8 & 0xff;
			parGroup[2] = groupID & 0xff;
			parGroup[3] = groupID >>8 & 0xff;
			parGroup[4] = 0x00;
			parGroup[5] = 0x10;

			mesh_cmd_sig_cfg_model_sub_set(parGroup, 6, cb_par_g);
		}
		else if(OpGroup == CFG_MODEL_SUB_DEL_ALL){
			parGroup[0] = (cb_par_g->adr_dst+i) & 0xff ;
			parGroup[1] = (cb_par_g->adr_dst+i) >> 8 & 0xff;
			parGroup[2] = 0x00;
			parGroup[3] = 0x10;

			mesh_cmd_sig_cfg_model_sub_set(parGroup, 4, cb_par_g);
		}
	}
}
static void RD_Handle_AutoCreateGr(u8 *par, uint16_t Gw_Add_Buff, mesh_cb_fun_par_t *cb_par)
{
	const uint16_t groupType 		= 0x001f; // switch Group
	uint16_t id_group = 0x0000;
	uint16_t id_group_type = 0x0000;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, Gw_Add_Buff, 2);

	id_group = (par[2] | par[3]<<8);
	id_group_type = id_group + groupType;

	RD_GroupAuto(id_group, cb_par, CFG_MODEL_SUB_ADD);
	RD_GroupAuto(id_group_type, cb_par, CFG_MODEL_SUB_ADD);

	uart_CSend("Auto create Group default \n");
	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_AutoDeleteGr(u8 *par, uint16_t Gw_Add_Buff, mesh_cb_fun_par_t *cb_par)
{
	const uint16_t groupType 		= 0x001f; // switch Group
	uint16_t id_group = 0x0000;
	uint16_t id_group_type = 0x0000;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, Gw_Add_Buff, 2);

	id_group = (par[2] | par[3]<<8);
	id_group_type = id_group + groupType;

	RD_GroupAuto(id_group, cb_par, CFG_MODEL_SUB_DEL);
	RD_GroupAuto(id_group_type, cb_par, CFG_MODEL_SUB_DEL);

	uart_CSend("Auto delete Group default\n");
	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

int RD_Messenger_ProcessCommingProcess_SCENE(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	uart_CSend("co ban tin E2\n");

	uint16_t Header_buff =0x00;
	uint16_t Gw_Add_Buff =0x01;
	uint16_t Add_Receiver = ele_adr_primary;

	Header_buff = ((par[1]<<8) | par[0]);
	Gw_Add_Buff = cb_par->adr_src;
	Add_Receiver = cb_par->adr_dst;
	char UART_Buff[128];
	switch (Header_buff)
	{
		case RD_HEADER_CONTROL_SW_TOUCH:
			RD_Handle_CTR_SW(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SWITCH_TOUCH_STT:
			Send_Relay_Stt_Message_RALL(Gw_Add_Buff);
			break;
		#if 1
		case RD_HEADER_SW_RGB_CONFIG:
			// only RGB led have this function
			//RD_Handle_HSL_CONFIG(par, Gw_Add_Buff, Add_Receiver); // using if config HSL mode
			RD_Handle_RGB_CONFIG(par, Gw_Add_Buff, Add_Receiver);
			break;
		#endif

		case RD_HEADER_SW_SETUP_LINKCTR:
			RD_Handle_SetUp_LinkControl2(par, Gw_Add_Buff, Add_Receiver);
			break;
		case RD_HEADER_SW_SETUP_COUNT_DOWN:
			#if(COUNT_DOWN_EN == 1)
				RD_Handle_SetUp_CountDown(par, Gw_Add_Buff, Add_Receiver);
			#else
				uart_CSend("This function only on Switch x1\n");
			#endif
			break;
		case RD_HEADER_SW_PAIR_K9BREMOTE:
			//RD_Handle_PairK9BHc(par, Gw_Add_Buff);
			RD_Handle_ScanK9BHc(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_SAVESCENE_K9BREMOTE:
			RD_Handle_K9BSaveScene(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_DELETESCENE_K9BREMOTE:
			RD_Handle_K9BDeleteScene(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_SAVEK9BHC:
			RD_Handle_SaveK9BHc(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_DELETEK9BHC:
			RD_Handle_DeleteK9BHC(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_POWUP_CONFIG:
			RD_Handle_PowUpConfig(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SET_K9B_LOCAL_MODE:
			RD_Handle_K9BLocalModeConfig(par, Gw_Add_Buff);
			break;
		case RD_AUTO_CREATE_GS:
			RD_Handle_AutoCreateGr(par, Gw_Add_Buff, cb_par );
			break;
		case RD_AUTO_DELETE_GS:
			RD_Handle_AutoDeleteGr(par, Gw_Add_Buff, cb_par );
			break;
		default:
			sprintf(UART_Buff,"wrong header:  %x \n",Header_buff);
			uart_CSend(UART_Buff);
			break;
	}

	return 0;
}


/*----------------------------------- E0 mess handle----------------------------------------*/
/**
 * @brief Respone Secure mess. 
 * 
 * @param Secure_Rsp_Stt RD_Success: Respone secure Pass to gateway
 * 						 RD_Failure: Respone secure Fail to gateway
 * @param Gw_Add_Buff 		gateway address to send 
 */
static void RD_Mess_RspSecure(uint8_t Secure_Rsp_Stt, uint16_t Gw_Add_Buff)
{
	uint8_t Mess_buff[8] = {0};

	if(Secure_Rsp_Stt == RD_EN)
	{
		Mess_buff[0]	= RD_HEADER_RSP_TYPE_DEVICE & 0xff;
		Mess_buff[1]	= (RD_HEADER_RSP_TYPE_DEVICE >> 8) & 0xff;
		Mess_buff[2]	= Device_BROADCRARD_0;
		Mess_buff[3]	= Device_BROADCRARD_1;
		Mess_buff[4]	= Device_BROADCRARD_2;
		Mess_buff[5]	= 0x00;
		Mess_buff[6] 	= VERSION_FIRM_H;
		Mess_buff[7]	= VERSION_FIRM_L;
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		uart_CSend("Rsp Secure success \n");
	}
	else
	{
		Mess_buff[0]	= RD_HEADER_RSP_TYPE_DEVICE & 0xff;
		Mess_buff[1]	= (RD_HEADER_RSP_TYPE_DEVICE >> 8) & 0xff;
		Mess_buff[2]	= 0xFF;
		Mess_buff[3]	= 0xFE;
		Mess_buff[4]	= 0xFF;
		Mess_buff[5]	= 0xFE;
		Mess_buff[6] 	= 0xFF;
		Mess_buff[7]	= 0xFE;
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		uart_CSend("Rsp Secure failed \n");
	}
}

/**
 * @brief Handle mess type device.  check secure key and rsp
 * 
 * @param par 
 * @param Gw_Add_Buff 
 */
static void RD_Handle_MessType(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint8_t Secure_return =0;
	#if(SECURE_ON ==1)

		if(get_provision_state() == STATE_DEV_PROVED)
		{
			Secure_return = RD_Secure_AesreCheck(&RD_key[0], &tbl_mac[0], ele_adr_primary, &par[2]);

			if(Secure_return == 1)
			{
				uart_CSend("Secure key Pass \n");
				RD_Flash_Save_Secure(RD_EN);			// save secure pass to flash memory
				RD_Secure_Set(RD_EN);					// set secure stt pass
				RD_Mess_RspSecure(RD_EN, Gw_Add_Buff);
			}
			else
			{
				uart_CSend("Secure key fail \n");
				RD_Flash_Save_Secure(RD_DIS);			// save secure pass to flash memory
				RD_Secure_Set(RD_DIS);					// set secure stt pass
				RD_Mess_RspSecure(RD_DIS, Gw_Add_Buff);
				RD_Secure_Check_Key_Fail();

			}
		}
		else
		{
			RD_Mess_RspSecure(RD_EN, Gw_Add_Buff);
		}
	#else
	Secure_return =1;
	RD_Flash_Save_Secure(RD_EN);			// save secure pass to flash memory
	RD_Mess_RspSecure(RD_EN, Gw_Add_Buff);
	#endif

}

/**
 * @brief Rsp save Gw
 * 
 * @param Gw_Add_Buff :
 */
static void RD_Handle_SaveGw(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t GwAddToSave = par[3]<<8 | par[2];

	if(GwAddToSave == 0x0000) 	{RD_GATEWAYADDRESS = Gw_Add_Buff;} // save gw add send mess
	else						{RD_GATEWAYADDRESS = GwAddToSave;} // save other gw add

	char UART_TempSend[64];
	sprintf(UART_TempSend,"Save GWAdd:%x \n", RD_GATEWAYADDRESS);
	uart_CSend(UART_TempSend);
	if(get_provision_state() == STATE_DEV_PROVED)
	{
		RD_Flash_SaveGwAdd(RD_GATEWAYADDRESS);
		char UART_TempSend[64];
		sprintf(UART_TempSend,"Save GWAdd to Flash :%x \n", RD_GATEWAYADDRESS);
		uart_CSend(UART_TempSend);
	}
	else
	{
		char UART_TempSend[64];
		sprintf(UART_TempSend,"Save GWAdd to Ram for test :%x \n", RD_GATEWAYADDRESS);
		uart_CSend(UART_TempSend);
	}
	uint8_t Mess_buff[8] = {0};

	Mess_buff[0]	= RD_HEADER_SAVE_GW_ADD 				& 0xff;
	Mess_buff[1]	= (RD_HEADER_SAVE_GW_ADD >> 8) 			& 0xff;
	Mess_buff[2]	= Sw_Flash_Data_Val.Gw_Add		 		& 0xff;
	Mess_buff[3]	= (Sw_Flash_Data_Val.Gw_Add >> 8) 		& 0xff;
	Mess_buff[4]	= PROVIDER_ID  							& 0xff;
	Mess_buff[5]	= (PROVIDER_ID >> 8) 					& 0xff;
	Mess_buff[6] 	= 0x00;
	Mess_buff[7]	= 0x00;

	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND); // rsp to Gw send mess
	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Hanlde_FactoryTestEnd(uint16_t Gw_Add_Buff)
{
	uint8_t Mess_buff[8] = {0};

	Mess_buff[0]	= RD_HEADER_FACTORY_TEST_END & 0xff;
	Mess_buff[1]	= (RD_HEADER_FACTORY_TEST_END >> 8) & 0xff;
	Mess_buff[2]	= 0x00;
	Mess_buff[3]	= 0x00;
	Mess_buff[4]	= 0x00;
	Mess_buff[5]	= 0x00;
	Mess_buff[6] 	= 0x00;
	Mess_buff[7]	= 0x00;

	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);

	uint16_t GWAdresss = 0x0000;
	GWAdresss = 0x0001;				// rRess
	RD_Flash_SaveGwAdd(GWAdresss);
	reset_Mess_Flag = 1;
	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_KickAll(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	if((par[2]==0x01) && (par[3]==0x02) && (par[4]==0x03) && (par[5]==0x04) && (par[6]==0x05) && (par[7]==0x06))
	{
		uart_CSend("Reset All device. countdown 60s to reset \n");
		Kick_all_Flag=1;
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, par, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		RD_SwitchAc4Ch_BlinkSet(5, 100);
	}
}

int RD_Messenger_ProcessCommingProcess_Type(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	#if UART_ON
		uart_CSend("Co ban tin");
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Messenger type:%d-Content:%x-%x-%x-%x-%x-%x-%x-%x\n",par_len,par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7]);
		uart_CSend(UART_TempSend);
		uart_CSend("..\n");
	#endif

	uint16_t Gw_Add_Buff =0x00;
	Gw_Add_Buff = cb_par->adr_src;

	uint16_t header_Buff = 0x0000;
	header_Buff = (par[1] << 8 | par[0]);
	switch (header_Buff)
	{
		case(RD_HEADER_RSP_TYPE_DEVICE):
			RD_Handle_MessType(par, Gw_Add_Buff);
			break;
		case(RD_HEADER_SAVE_GW_ADD):
			RD_Handle_SaveGw(par, Gw_Add_Buff);
			break;
		case(RD_HEADER_FACTORY_TEST_END):
			RD_Hanlde_FactoryTestEnd( Gw_Add_Buff);
			break;
		case(RD_HEADER_KICK_ALL):
			RD_Handle_KickAll(par, Gw_Add_Buff);
			break;
		default:
			uart_CSend("Wrong header E0 \n");
			break;
	}
	return 0;
}


/******************************* Other mess *******************************/


void Send_Relay_Stt_Message_RALL(uint16_t GW_ADR)
{
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0]  = RD_HEADER_SWITCH_TOUCH_STT & 0xFF;
	Mess_Buff[1]  = RD_HEADER_SWITCH_TOUCH_STT >>8;
	Mess_Buff[2]  = NUM_OF_ELEMENT;
	Mess_Buff[3]  = relay_Stt[0];
	Mess_Buff[4]  = relay_Stt[1];
	Mess_Buff[5]  = relay_Stt[2];
	Mess_Buff[6]  = relay_Stt[3];
	Mess_Buff[7]  = relay_Stt[4];

	if(GW_ADR == 0x0000)
	{
		GW_ADR = 0x0001;
	}
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, GW_ADR, RD_MAXRESPONESEND); //SENSOR_STATUS

}

void Send_Relay_SttCtrAll_ReUp_Message(uint16_t GW_ADR, uint8_t relay_Stt[5])
{
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0]  = RD_HEADER_CTRALL_REUP_STT & 0xFF;
	Mess_Buff[1]  = RD_HEADER_CTRALL_REUP_STT >>8;
	Mess_Buff[2]  = NUM_OF_ELEMENT;
	for(int i=0; i< NUM_OF_ELEMENT; i++)
	{
		Mess_Buff[3+i] = relay_Stt[i];
	}


	if(GW_ADR == 0x0000)
	{
		GW_ADR = 0x0001;
	}
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, GW_ADR, RD_MAXRESPONESEND); //SENSOR_STATUS

}

void Send_Relay_Stt_Message_RALL_PowerUp(void)
{
	uint8_t Mess_Buff[8] = {0};

	u8 relay_stt_buff[5] ={0};
	for(int i=0; i<LIGHT_CNT; i++)
	{
		mesh_cmd_g_onoff_st_t onoff_stt = {0};

		mesh_g_onoff_st_rsp_par_fill(&onoff_stt, i);
		relay_stt_buff[i] = onoff_stt.present_onoff;
	}
	Mess_Buff[0]  = RD_HEADER_SWITCH_TOUCH_STT & 0xFF;
	Mess_Buff[1]  = RD_HEADER_SWITCH_TOUCH_STT >>8;
	Mess_Buff[2]  = NUM_OF_ELEMENT;
	Mess_Buff[3]  = relay_stt_buff[0];
	Mess_Buff[4]  = relay_stt_buff[1];
	Mess_Buff[5]  = relay_stt_buff[2];
	Mess_Buff[6]  = relay_stt_buff[3];
	Mess_Buff[7]  = relay_stt_buff[4];

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS

}

void Send_Relay_Stt_Message_RID(uint8_t Relay_ID)
{
	STT_Relay_Mess_Config.Header[0] 	= RD_HEADER_SENSOR_SWT_STT & 0xFF ;
	STT_Relay_Mess_Config.Header[1] 	= RD_HEADER_SENSOR_SWT_STT >>8;

	STT_Relay_Mess_Config.Relay_Stt[0] 	= Relay_ID;
	STT_Relay_Mess_Config.Relay_Stt[1] 	= relay_Stt[Relay_ID -1];
	STT_Relay_Mess_Config.Relay_Stt[2] 	= 0x00;
	STT_Relay_Mess_Config.Relay_Stt[3] 	= 0x00;
	STT_Relay_Mess_Config.Future[0]		= 0x00;
	STT_Relay_Mess_Config.Future[1]		= 0x00;
	uint8_t* Mess_Buff;
	Mess_Buff = (uint8_t *) (&STT_Relay_Mess_Config);
	mesh_tx_cmd2normal_primary(SENSOR_STATUS, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
}

int RD_Messenger_BindAll(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	extern int mesh_cmd_sig_cfg_bind(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

	extern unsigned char vr_RD_ProvDone;
	if((par[4] == 0x06) && (par[5] == 0x13)){
		vr_RD_ProvDone = 1;
	}
	return(mesh_cmd_sig_cfg_bind(par, par_len, cb_par));
}

int RD_mesh_cmd_sig_cfg_bind(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	#if UART_ON
	uart_CSend("Co ban tin");
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Messenger Box Bind :%d-Content:%x-%x-%x-%x-%x-%x-%x-%x\n",par_len,par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7]);
	uart_CSend(UART_TempSend);
	#endif

	static uint16_t add_element_bind_pre = 0x0000;
	static uint8_t  element_cnt =0;
	uint16_t 		add_element_bind_now = (uint16_t)  ( par[0] | (par[1] <<8));
	if(add_element_bind_now !=  add_element_bind_pre)
	{
		add_element_bind_pre = add_element_bind_now;
		element_cnt++;
		RD_Flash_SaveElementAdd(add_element_bind_now,  element_cnt);
		if(element_cnt == 1) {RD_ELEMENT_ADDR_1 = add_element_bind_now;}
		if(element_cnt == 2) {RD_ELEMENT_ADDR_2 = add_element_bind_now;}
		if(element_cnt == 3) {RD_ELEMENT_ADDR_3 = add_element_bind_now;}
		if(element_cnt == 4) {RD_ELEMENT_ADDR_4 = add_element_bind_now;}
	}

	return(mesh_cmd_sig_cfg_bind(par, par_len,  cb_par));
}

int RD_Messenger_ProcessCommingProcess_TRAIN(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	uint16_t Header_Buff = 0x0000;
	uint16_t Gw_Add_Buff = 0x0001;

	Gw_Add_Buff = cb_par->adr_src;
	Header_Buff = (par[1] << 8) | (par[0]);
	if((clock_time_s() > DELAY_TRAIN_TIME) && (Traing_Loop == 0) && (Header_Buff == 0x0101))
	{
		RD_Handle_Train(par, Gw_Add_Buff);
	}
	return 0;
}

void RD_Send_Relay_Stt(uint8_t Relay_ID, uint8_t Relay_Stt)
{
	uint8_t Mess_Buff[8] = {0};
	uint16_t Element_Add = 0x0000;

	Mess_Buff[0]		= Relay_Stt;
	Element_Add 		= ele_adr_primary + (Relay_ID -1);

	mesh_tx_cmd2normal(G_ONOFF_STATUS, Mess_Buff, 1, Element_Add, RD_GATEWAYADDRESS, 2);

	char UART_TempSend[128];
	sprintf(UART_TempSend,"Messenger On-Off Gw:0x%x- Relay: %d--%d--%d--%d  \n",RD_GATEWAYADDRESS, relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3]);
	uart_CSend(UART_TempSend);
}

void RD_Call_LinkControl(uint8_t Button_ID, uint8_t Type_Scene)
{
	uint16_t Add_Link_Control = 0x0000;
	if(Sw_Flash_Data_Val.Add_Link_Control[Button_ID -1] != 0x0000)
	{
		if(Type_Scene == 0x00) 	Add_Link_Control  	=	Sw_Flash_Data_Val.Add_Link_Control[Button_ID -1];
		else					Add_Link_Control	= 	Sw_Flash_Data_Val.Add_Link_Control[Button_ID -1];

		//RD_Call_Scene(Scene_ID_Call, Button_ID*4+Type_Scene);
		access_cmd_onoff(Add_Link_Control, 0, Type_Scene, CMD_NO_ACK, 0x0000);
		char UART_TempSend[64];
		sprintf(UART_TempSend,"call link control with add 0x%x -", Add_Link_Control);
		uart_CSend(UART_TempSend);
	}
}

void RD_MessK9BHc_Press(uint16_t K9B_Add, uint8_t ButtonID, uint8_t ModeID, uint16_t SenceID)
{
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0] 	= RD_HEADER_SW_K9B_PRESS & 0xFF ;
	Mess_Buff[1]	= RD_HEADER_SW_K9B_PRESS >>8;
	Mess_Buff[2]  	= K9B_Add & 0xFF;
	Mess_Buff[3]  	= K9B_Add >>8;
	Mess_Buff[4]  	= ButtonID;
	Mess_Buff[5]  	= ModeID;
	Mess_Buff[6]  	= SenceID & 0xFF;
	Mess_Buff[7]  	= SenceID >>8;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
	uart_CSend("send K9B Press HC \n");
}

void RD_Update_Relay_Stt(uint8_t Relay_ID)
{
//	SETMIN(Relay_ID, 1);
#if(0)
	uint8_t Mess_Buff[8] = {0};
	uint16_t Element_Add = 0x0000;

	Mess_Buff[0]		= relay_Stt[Relay_ID-1];
	Mess_Buff[1]		= 0x0A;
	Element_Add 		= ele_adr_primary + (Relay_ID -1);

	mesh_tx_cmd2normal(G_ONOFF_STATUS, Mess_Buff, 2, Element_Add, RD_GATEWAYADDRESS, 3);

	char UART_TempSend[128];
	sprintf(UART_TempSend,"Update On-Off Gw:0x%x- Relay: %d--%d--%d--%d element: %x \n",RD_GATEWAYADDRESS, relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3], Element_Add);
	uart_CSend(UART_TempSend);
#endif

	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0] 	= RD_HEADER_REUP_STT & 0xFF ;
	Mess_Buff[1]	= RD_HEADER_REUP_STT >>8;
	Mess_Buff[2]  	= NUM_OF_ELEMENT;
	Mess_Buff[3]  	= relay_Stt[0];
	Mess_Buff[4]  	= relay_Stt[1];
	Mess_Buff[5]  	= relay_Stt[2];
	Mess_Buff[6]  	= relay_Stt[3];
	Mess_Buff[7]  	= relay_Stt[4];

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
	uart_CSend("update stt \n");
}


void RD_Call_Scene(uint16_t Scene_ID, uint8_t Mess_ID)
{
	Call_Scene_Str Scene_Mess_Buff;
	Scene_Mess_Buff.Scene_ID[0] = (uint8_t) ((Scene_ID) & 0xFF );
	Scene_Mess_Buff.Scene_ID[1] = (uint8_t) ((Scene_ID >> 8) & 0xFF );
	Scene_Mess_Buff.Null_Byte   = Mess_ID;
	Scene_Mess_Buff.Tss[0]		= (uint8_t) ((TSS_DEFAULT) & 0xFF);
	Scene_Mess_Buff.Tss[1]		= (uint8_t) ((TSS_DEFAULT>>8) & 0xFF);
	Scene_Mess_Buff.Future[0]   = 0x00;
	Scene_Mess_Buff.Future[1]   = 0x00;
	Scene_Mess_Buff.Future[2]   = 0x00;

	uint8_t* Mess_Buff;
	Mess_Buff = (uint8_t *) (&Scene_Mess_Buff);
	mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, Mess_Buff, 8, 0xffff, 0); //SENSOR_STATUS

	//mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, (uint8_t *) (&Scene_Mess_Buff.Scene_ID[0]), 8, 0xffff, 0); //SENSOR_STATUS
}

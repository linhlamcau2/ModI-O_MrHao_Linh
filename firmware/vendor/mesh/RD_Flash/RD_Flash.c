/*
 * RD_Flash.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Dang_Hao
 */

#include "RD_Flash.h"
extern uint16_t RD_GATEWAYADDRESS;
uint8_t Jump_Flash=0x00;												// val to set add Flash for Power Off data
uint32_t Counter_Flash_Power_Eraser =0x00000000;						// counter Eraser Flash sector.  Higher 80000. move Add Flash Power Off ++
Sw_Flash_Data Sw_Flash_Data_Val ={0};

void RD_Flash_Save_DataDefault(void)
{
	Sw_Flash_Data Flash_Data_Buff ={0};
	Flash_Data_Buff.Factory_Check 	= 0x00;
	Flash_Data_Buff.Gw_Add 			= GW_ADD_DEFAULT;
	Flash_Data_Buff.Secure_RD 		= RD_Failure;
	Flash_Data_Buff.PowerUpStt      = RD_PowUpStore;
	for(int i=0; i< 5; i++)
	{
		Flash_Data_Buff.Data_Led[i].Lightness_Off	= LED_DIM_OFF_DF;
		Flash_Data_Buff.Data_Led[i].Lightness_On 	= LED_DIM_ON_DF;
		Flash_Data_Buff.Data_Led[i].Blue_Gray		= BLUE_BT_DF;
		Flash_Data_Buff.Data_Led[i].Green_Gray		= GREEN_BT_DF;
		Flash_Data_Buff.Data_Led[i].Red_Gray		= RED_BT_DF;
		Flash_Data_Buff.Add_Link_Control[i]			= 0x00;

		Flash_Data_Buff.CountDown[i].Taget 					= 0x00;
		Flash_Data_Buff.CountDown[i].Time_CountDown_s  		= 0x00;
		Flash_Data_Buff.CountDown[i].Time_Start_CountDown_s = 0x00;

	}
	Flash_Data_Buff.Type_CtrLocal_K9B = K9B_TYPE_CTR_LOCAL_DF;
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Flash_Data_Buff), (uint8_t *) &Flash_Data_Buff.Factory_Check );
	flash_read_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );

#if UART_ON

	uart_CSend("Save Data default");

#endif
}

static void RD_Flash_Data_Init(void)
{
	/*----------------------------------------Flash of Scene, Gw_Add, RGB------------------------------------------*/


	flash_read_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );

	if(Sw_Flash_Data_Val.Factory_Check == 0xFF)															// fist time to read reset memory
	{
		RD_Flash_Save_DataDefault();
	}

	RD_GATEWAYADDRESS = Sw_Flash_Data_Val.Gw_Add;
	if(RD_GATEWAYADDRESS == 0x0000)
	{
		RD_GATEWAYADDRESS = 0x0001;
	}

	char UART_TempSend[128];
	sprintf(UART_TempSend,"Flash data 0x780000: size: %d Secure:0x%x  \n GW add: 0x%x  \n  ",sizeof(Sw_Flash_Data_Val), Sw_Flash_Data_Val.Secure_RD, RD_GATEWAYADDRESS);
	uart_CSend(UART_TempSend);


}

static void RD_Flash_PowerUp_Init(void)
{
	uint8_t *Flash_Data_Power_Off_Buff;
	Flash_Data_Power_Off_Buff  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);

	flash_read_page(DATA_POWER_OFF_FLASH_ADDR, FLASH_DATA_POWER_OFF_SIZE, Flash_Data_Power_Off_Buff);
	if(Sw_Flash_Data_Save_Power_Off_Val.Factory_Check == 0xFF)															// fist time to read reset memory
	{
		RD_Flash_CleanSenceFlashPower(DATA_POWER_OFF_FLASH_ADDR);
	}
	else
	{
		Jump_Flash = Sw_Flash_Data_Save_Power_Off_Val.Factory_Check;
		if(Jump_Flash != 0x00)																							// Flash Add = DATA_POWER_OFF_FLASH_ADDR+ jump Flash // Flash Add = DATA_POWER_OFF_FLASH_ADDR
		{
			flash_read_page((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), FLASH_DATA_POWER_OFF_SIZE, Flash_Data_Power_Off_Buff);
			if(Sw_Flash_Data_Save_Power_Off_Val.Factory_Check == 0xFF)													// fist time to read reset memory
			{
				RD_Flash_CleanSenceFlashPower((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)));
			}
		}

		if(get_provision_state() == STATE_DEV_PROVED)
		{
			#if(CONFIG_POWUP_EN)
				uint8_t PowUpStt =0;
				if(RD_PowUpOff == Sw_Flash_Data_Val.PowerUpStt) PowUpStt =0;
				if(RD_PowUpOn == Sw_Flash_Data_Val.PowerUpStt) PowUpStt =1;

				if(RD_PowUpStore != Sw_Flash_Data_Val.PowerUpStt)
				{
					uart_CSend("  Set PowUp \n");
					for(int i=0; i< NUM_OF_ELEMENT; i++)
					{
						light_onoff_idx(i,PowUpStt, 0);
						set_on_power_up_onoff(i, 0, PowUpStt); // save for POWer_up
					}
				}
			#endif

			for(int i=0; i< 5; i++)
			{
				relay_Stt[i] = Relay_Off;				// if provisioned will get value from last power of element in SDK
			}
		}
		else
		{
			for(int i=0; i< 5; i++)
			{
				//relay_Stt[i] = Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[i];
				RD_SetAndRsp_Switch(i, Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[i], 0);
			}
			Counter_Flash_Power_Eraser = 	(Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[0] << 24) | (Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[1] << 16) | \
											(Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[2] << 8)  | (Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[3]);
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Flash POWER Off: Add: 0x%x---- Count Eraser:0x%x  \n Fac: 0x%x---Relay Stt %x-%x-%x-%x",(DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), Counter_Flash_Power_Eraser,  Sw_Flash_Data_Save_Power_Off_Val.Factory_Check/
					relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3]);
			uart_CSend(UART_TempSend);
		}



	}
}

void RD_Flash_Init()
{
	RD_Flash_Data_Init();
	/*-------------------------------------Data Power OFF------------------------------------------------------*/
	RD_Flash_PowerUp_Init();
	#if 0
	uint8_t *Flash_Data_Power_Off_Buff;
	Flash_Data_Power_Off_Buff  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);

	flash_read_page(DATA_POWER_OFF_FLASH_ADDR, FLASH_DATA_POWER_OFF_SIZE, Flash_Data_Power_Off_Buff);
	if(Sw_Flash_Data_Save_Power_Off_Val.Factory_Check == 0xFF)															// fist time to read reset memory
	{
		RD_Flash_CleanSenceFlashPower(DATA_POWER_OFF_FLASH_ADDR);
	}
	else
	{
		Jump_Flash = Sw_Flash_Data_Save_Power_Off_Val.Factory_Check;
		if(Jump_Flash != 0x00)																							// Flash Add = DATA_POWER_OFF_FLASH_ADDR+ jump Flash // Flash Add = DATA_POWER_OFF_FLASH_ADDR
		{
			flash_read_page((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), FLASH_DATA_POWER_OFF_SIZE, Flash_Data_Power_Off_Buff);
			if(Sw_Flash_Data_Save_Power_Off_Val.Factory_Check == 0xFF)													// fist time to read reset memory
			{
				RD_Flash_CleanSenceFlashPower((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)));
			}
		}


		for(int i=0; i<4; i++)
		{
			relay_Stt[i] = Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[i];
		}

		Counter_Flash_Power_Eraser = 	(Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[0] << 24) | (Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[1] << 16) | \
									 	(Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[2] << 8)  | (Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[3]);
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Flash POWER Off: Add: 0x%x---- Count Eraser:0x%x  \n Fac: 0x%x---Relay Stt %x-%x-%x-%x",(DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), Counter_Flash_Power_Eraser,  Sw_Flash_Data_Save_Power_Off_Val.Factory_Check/
				relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3]);
		uart_CSend(UART_TempSend);
	}
	#endif

}



void RD_Flash_CleanSenceFlashPower(unsigned long AddFlashPower)
{
	uint8_t Buff_Flash_Null[FLASH_DATA_POWER_OFF_SIZE]={0};
	uart_CSend("Clean Power Off Flash");
   	flash_erase_sector(AddFlashPower);
	flash_write_page(AddFlashPower, sizeof(Sw_Flash_Data_Save_Power_Off),Buff_Flash_Null );
	flash_read_page(AddFlashPower, sizeof(Sw_Flash_Data_Save_Power_Off), (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val));
}


void RD_Flash_SaveGwAdd(uint16_t Gw_Add)
{
	Sw_Flash_Data_Val.Gw_Add = Gw_Add;

	// Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);
   	// flash_erase_sector(FLASH_ADDR);
	// flash_write_page(FLASH_ADDR, FLASH_DATA_SIZE,Flash_Data_Buff );
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save GW Address: 0x%x\n", Sw_Flash_Data_Val.Gw_Add);
	uart_CSend(UART_TempSend);

#endif


}

void RD_Flash_SetCountDown(uint8_t Light_Index, uint8_t Taget_Control, uint16_t TimeCountDown_s)
{
	Sw_Flash_Data_Val.CountDown[Light_Index].Taget = Taget_Control;
	Sw_Flash_Data_Val.CountDown[Light_Index].Time_CountDown_s = TimeCountDown_s;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"time countdown : 0x%x\n", Sw_Flash_Data_Val.CountDown[Light_Index].Time_CountDown_s);
	uart_CSend(UART_TempSend);

#endif
}

void RD_Flash_PowUpConfig(uint8_t PowUpConfig)
{
	Sw_Flash_Data_Val.PowerUpStt = PowUpConfig;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"PowUp Stt : %d\n", Sw_Flash_Data_Val.PowerUpStt);
	uart_CSend(UART_TempSend);

#endif
}
/*
 * Save data con fig RGB
 * LED_ID:  1-2-3-4  LED fit with button
 * Lightness: 15-->31:    Lightness when relay on
	Blue_Gray:   Gray level of Blue Value Range 0-->256;
	Green_Gray:  Gray level of Green Value Range 0-->256;
	Red_Gray:   Gray level of Red Value Range 0-->256;
 */

void RD_Flash_SaveRGB_Config(uint8_t LED_ID, uint8_t Lightness_On, uint8_t Lightness_Off, uint8_t Blue, uint8_t Green, uint8_t Red)
{
	//LED_ID = LED_ID:3 ?  LED_ID > 3;
//	if(Lightness_On >31)		Lightness = 31;
//	SETMAX(Lightness_On, 31);
//	SETMIN(Lightness_Off, 0);
	if(LED_ID != 0xFF)
	{
		LED_Switch_Data.LED[LED_ID -1].Blue_Gray 	= Blue;
		LED_Switch_Data.LED[LED_ID -1].Green_Gray	= Green;
		LED_Switch_Data.LED[LED_ID -1].Red_Gray		= Red;

		Sw_Flash_Data_Val.Data_Led[LED_ID -1].Lightness_On 	= Lightness_On;
		Sw_Flash_Data_Val.Data_Led[LED_ID -1].Lightness_Off = Lightness_Off;
		Sw_Flash_Data_Val.Data_Led[LED_ID -1].Blue_Gray		= Blue;
		Sw_Flash_Data_Val.Data_Led[LED_ID -1].Green_Gray	= Green;
		Sw_Flash_Data_Val.Data_Led[LED_ID -1].Red_Gray		= Red;

		LED_Set_OnOff_Stt(LED_ID, relay_Stt[LED_ID -1]);
	}
	else
	{
		for(int i = 0; i <4; i++)
		{
			LED_Switch_Data.LED[i].Blue_Gray 	= Blue;
			LED_Switch_Data.LED[i].Green_Gray	= Green;
			LED_Switch_Data.LED[i].Red_Gray		= Red;

			Sw_Flash_Data_Val.Data_Led[i].Lightness_On 	= Lightness_On;
			Sw_Flash_Data_Val.Data_Led[i].Lightness_Off = Lightness_Off;
			Sw_Flash_Data_Val.Data_Led[i].Blue_Gray		= Blue;
			Sw_Flash_Data_Val.Data_Led[i].Green_Gray	= Green;
			Sw_Flash_Data_Val.Data_Led[i].Red_Gray		= Red;

			LED_Set_OnOff_Stt(i+1, relay_Stt[i]);
		}
	}
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );

#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save RGB ID:%d \t Dim_On: %x \t Dim_Off: %x\t B:%d, G:%d, R:%d \n", LED_ID, Lightness_On, Lightness_Off, Blue, Green, Red);
	uart_CSend(UART_TempSend);
#endif


}

void RD_Flash_SaveElementAdd(uint16_t Element_Add, uint8_t Number_off_Element)
{


}

void RD_Flash_SavePowerOff(void)
{
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[0]	= relay1_Stt;
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[1]	= relay2_Stt;
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[2]	= relay3_Stt;
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[3]	= relay4_Stt;
	
	for(int i=0; i< 5; i++)
	{
		Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[i] = relay_Stt[i];
	}
	Counter_Flash_Power_Eraser++;
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[0] = (uint8_t) ((Counter_Flash_Power_Eraser>>24) & 0xFF);
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[1] = (uint8_t) ((Counter_Flash_Power_Eraser>>16) & 0xFF);
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[2] = (uint8_t) ((Counter_Flash_Power_Eraser>>8) & 0xFF);
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[3] = (uint8_t) ((Counter_Flash_Power_Eraser) & 0xFF);

	if(Counter_Flash_Power_Eraser >=80000)																		// over eraser Flash this Sector
	{
		Jump_Flash++;																							// move to next Flash sector
		Counter_Flash_Power_Eraser=0;
		Sw_Flash_Data_Save_Power_Off_Val.Factory_Check = Jump_Flash;
		uint8_t *Flash_Data_Buff2;
		Flash_Data_Buff2  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);
	   	flash_erase_sector(DATA_POWER_OFF_FLASH_ADDR);															// update to 0x79000
		flash_write_page(DATA_POWER_OFF_FLASH_ADDR, FLASH_DATA_POWER_OFF_SIZE,Flash_Data_Buff2 );
	}
	uint8_t *Flash_Data_Buff;
	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);
   	flash_erase_sector((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)));
	flash_write_page((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), FLASH_DATA_POWER_OFF_SIZE,Flash_Data_Buff );

	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Saves data power Off: %x-%x-%x-%x \n Eraser: %d Add: 0x%x \n",	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[0], Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[1], Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[2], Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[3], Counter_Flash_Power_Eraser, (DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash))  );
		uart_CSend(UART_TempSend);
	#endif
}

void RD_Flash_Save_Secure(uint8_t Secure_Stt)
{
	Sw_Flash_Data_Val.Secure_RD = RD_Success;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
}

void RD_Flash_Save_LinkControl(uint8_t Button_Set, uint16_t Add_Control)
{
	//Button_Set = (Button_Set < 4)	?  Button_Set : 4;
	SETMAX(Button_Set, 5);
	SETMIN(Button_Set, 1);

	Sw_Flash_Data_Val.Add_Link_Control[Button_Set -1] 	= Add_Control;
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );

	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Save Scene Stair for button: %d\n",	Button_Set-1);
		uart_CSend(UART_TempSend);
	#endif

}

/**
 * macDevice: mac of remote K9B, get 4 byte 0->3
 * Key: mode+Button K9B press
 * SwButtonID: 1->4 ButtonID in RD switch
 */
void RD_Flash_SaveK9BOnOff(uint32_t macDevice, uint8_t key, uint8_t SwButtonID)
{
	uint8_t ID_ArraySave = Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext;
	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].MacK9B[ID_ArraySave] = macDevice;
	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].K9B_BtKey[ID_ArraySave] = key;

	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext++;  // next ID array to save
	if(Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext >= MAX_NUM_K9ONOFF) // resave over arrayy 0ad
	{
		uart_CSend("save max button K9B\n");
		Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext = 0;
	}

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save K9B OnOff: Button:%d Mac:%x  Key:%x \n",SwButtonID,  macDevice, key);
	uart_CSend(UART_TempSend);
#endif
}

/**
 * delete data K9B onOff in this button
 * SwButtonID: 1->4 ButtonID in RD switch
 */
void RD_Flash_DeleteAllK9BOnOff(uint8_t SwButtonID)
{
	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext =0;  // next ID array to save

	for(int i=0; i < MAX_NUM_K9ONOFF; i++)
	{
		Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].MacK9B[i] = 0x00;
		Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].K9B_BtKey[i] = 0x00;
	}
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[64];
	sprintf(UART_TempSend,"delete All K9B on button:%d \n",SwButtonID);
	uart_CSend(UART_TempSend);
#endif
}

uint8_t RD_Flash_SaveK9BHc(uint32_t macDevice, uint8_t Num_Of_Button, uint16_t K9BAdd)
{
	uint8_t overSave_Flag =0; // resave when duplicate mac
	uint8_t  overSave_ID =0;
	// check duplicate remote saved
	for(int i=0; i<MAX_NUM_K9BHC; i++)
	{
		if(Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B == macDevice)
		{
			overSave_Flag =1;
			overSave_ID = i;
			break;
		}
	}

	if(1 == overSave_Flag)
	{
		Sw_Flash_Data_Val.K9B_Data.Scene[overSave_ID].MacK9B = macDevice;
		Sw_Flash_Data_Val.K9B_Data.Scene[overSave_ID].Num_Of_Button = Num_Of_Button;
		Sw_Flash_Data_Val.K9B_Data.Scene[overSave_ID].AddK9B = K9BAdd;
		flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
		char UART_TempSend[128];
		sprintf(UART_TempSend,"over Save K9B HC Mac:%x  NumOfButton:%d add: 0x%x\n",macDevice,  Num_Of_Button, K9BAdd);
		uart_CSend(UART_TempSend);
		return 1;
	}
	// check null task memory
	for(int i=0; i<MAX_NUM_K9BHC; i++)
	{
		if(Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B == 0x00)		// task memory is ready for new
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B = macDevice;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].Num_Of_Button = Num_Of_Button;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B = K9BAdd;
			flash_erase_sector(FLASH_ADDR);
			flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Save K9B HC Mac:%x  NumOfButton:%d add: 0x%x\n",macDevice,  Num_Of_Button, K9BAdd);
			uart_CSend(UART_TempSend);
			return 1;
		}
		if(i == (MAX_NUM_K9BHC-1) ) return 0; // fail full mem
	}
	return 0;
}

// check pos button ID in button aray of remote K9B HC
int RD_CheckButtonPosK9BHC(uint8_t ButtonID_Aray[MAX_MUM_K9BPRESSTYPE], uint8_t ButtonID_Check){
	for(int i=0; i<MAX_MUM_K9BPRESSTYPE; i++){
		if(ButtonID_Aray[i] == ButtonID_Check) {
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Button Pos %d\n",i);
			uart_CSend(UART_TempSend);
			return i;
		}
	}
	uart_CSend("don't find BID \n");
	return -1;
}
uint8_t RD_Flash_SaveSceneK9BHC(uint16_t K9BAdd, uint8_t ButtonID, uint16_t SceneID)
{
	for(int i=0; i<MAX_NUM_K9BHC; i++)
	{
		if(Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B == K9BAdd)		// task memory is ready for new
		{
			int ButtonPos_Buff = RD_CheckButtonPosK9BHC(&Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[0], ButtonID);

			if(-1 == ButtonPos_Buff){ // new ButtonID //RD_Note
				int ButtonPos_New = RD_CheckButtonPosK9BHC(&Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[0], 0x00);
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[ButtonPos_New] = SceneID;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[ButtonPos_New] = ButtonID;
			}
			else{ // duplicate Button ID, over save
				if(0x0000 == SceneID) ButtonID =0; // delete scene
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[ButtonPos_Buff] = SceneID;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[ButtonPos_Buff] = ButtonID;
			}

			flash_erase_sector(FLASH_ADDR);
			flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Save K9B HC scene:%x  BID:%d, ButtonPos_Buff: %d\n",SceneID,  ButtonID, ButtonPos_Buff);
			uart_CSend(UART_TempSend);
			return 1;
		}
	}
	return 0; // don;t find k9b remote
}

void RD_Flash_SetNullK9B(uint8_t numOfArray)
{
	if( 0xFF == numOfArray)
	{
		for(int i=0; i< MAX_NUM_K9BHC; i++)
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B = 0x00;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].Num_Of_Button = 0x00;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B = 0x00;
			for(int j=0; j< MAX_MUM_K9BPRESSTYPE; j++)
			{
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[j] = 0;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[j] = 0;
			}
		}
	}
	else
	{
		Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].MacK9B = 0x00;
		Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Num_Of_Button = 0x00;
		Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].AddK9B = 0x00;
		for(int j=0; j< MAX_MUM_K9BPRESSTYPE; j++)
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Scene_ID_OnePress[j] = 0;
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Button_ID[j] = 0;
		}
	}


}
uint8_t RD_Flash_DeleteK9BHC(uint16_t K9BAdd)
{
	if(0xFFFF != K9BAdd)
	{
		for(int i=0; i<MAX_NUM_K9BHC; i++)
		{
			if(Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B == K9BAdd)
			{
				RD_Flash_SetNullK9B(i);
				flash_erase_sector(FLASH_ADDR);
				flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
				char UART_TempSend[128];
				sprintf(UART_TempSend,"delete K9B HC add: 0x%x\n",K9BAdd);
				uart_CSend(UART_TempSend);
				return 1;
			}
		}
		uart_CSend("don't have  K9B HC to delete");
		return 0; // no data to delete
	}
	else
	{
		RD_Flash_SetNullK9B(0xFF);
		flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
		uart_CSend("delete all remote K9B HC");
		return 1;
	}
}

static void RD_Flash_SetNullSceneK9B(uint8_t numOfArray)
{
	if( 0xFF == numOfArray)
	{
		for(int i=0; i< MAX_NUM_K9BHC; i++)
		{
			for(int j=0; j< MAX_MUM_K9BPRESSTYPE; j++)
			{
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[j] = 0;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[j] = 0;
			}
		}
	}
	else
	{
		for(int j=0; j < MAX_MUM_K9BPRESSTYPE; j++)
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Scene_ID_OnePress[j] = 0;
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Button_ID[j] = 0;
		}
	}


}
uint8_t RD_Flash_DeleteSceneK9BHC(uint16_t K9BAdd)
{
	if(0xFFFF != K9BAdd)
	{
		for(int i=0; i<MAX_NUM_K9BHC; i++)
		{
			if(Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B == K9BAdd)
			{
				RD_Flash_SetNullSceneK9B(i);
				flash_erase_sector(FLASH_ADDR);
				flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
				char UART_TempSend[128];
				sprintf(UART_TempSend,"delete all scene K9B add: 0x%x\n",K9BAdd);
				uart_CSend(UART_TempSend);
				return 1;
			}
		}
		uart_CSend("don't have  K9B HC to delete Scene");
		return 0; // no data to delete
	}
	else
	{
		RD_Flash_SetNullSceneK9B(0xFF);
		flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
		uart_CSend("delete all scene of all K9B \n");
		return 1;
	}
}
void RD_Flash_K9BSaveScene(uint8_t Button_ID, uint8_t Mode_ID, uint16_t Scene_ID)
{
//	if(Button_ID >3) Button_ID = 3;
//	if(0x01 == Mode_ID ) 	Sw_Flash_Data_Val.K9B_Data.Scene.Scene_ID_OnePress[Button_ID-1] = Scene_ID;
//	else  					Sw_Flash_Data_Val.K9B_Data.Scene.Scene_ID_DoublePress[Button_ID-1] = Scene_ID;
//
//	flash_erase_sector(FLASH_ADDR);
//	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
//#if UART_ON
//	char UART_TempSend[128];
//	sprintf(UART_TempSend,"Save K9B Scene Button:%d, Mode: %d, SceneID: %04x \n",Button_ID,  Mode_ID, Scene_ID);
//	uart_CSend(UART_TempSend);
//#endif
}

void RD_Flash_K9BSaveLocalMode(uint8_t local_mode)
{
	if(K9B_CTR_LOCAL_SCENE != local_mode){
		local_mode = K9B_TYPE_CTR_LOCAL_DF;
		uart_CSend("save K9B_TYPE_CTR_LOCAL_DF \n");
	}
	else{
		local_mode = K9B_CTR_LOCAL_SCENE;
		uart_CSend("save K9B_CTR_LOCAL_SCENE \n");
	}

	Sw_Flash_Data_Val.Type_CtrLocal_K9B = local_mode;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
}

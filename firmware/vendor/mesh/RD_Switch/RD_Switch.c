/*
 * RD_FPT_Switch.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Dang_Hao1
 */
#include "RD_Switch.h"
extern void mesh_g_onoff_st_rsp_par_fill(mesh_cmd_g_onoff_st_t *rsp, u8 idx);
extern void set_on_power_up_onoff(int idx, int st_trans_type, u8 onoff);
extern int mesh_tx_cmd_g_onoff_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md, u16 op_rsp);
uint8_t 	CycleBlink = 0;
uint8_t     check = 0;

uint16_t 	CountToggleBlink=0;
uint8_t		Button1_Keep_Flag=0;
uint8_t		Button2_Keep_Flag=0;
uint8_t		Button3_Keep_Flag=0;
uint8_t		Button4_Keep_Flag=0;
uint8_t		Button5_Keep_Flag=0;

uint8_t		Button1_Hold_Flag=0;
uint8_t		Button2_Hold_Flag=0;
uint8_t		Button3_Hold_Flag=0;
uint8_t		Button4_Hold_Flag=0;
uint8_t		Button5_Hold_Flag=0;

uint8_t     Sw1_PairK9B_OnOff_Flag =0;
uint8_t	Register_Config[128] = {0};

Button_Stt_Type button1_Stt = Button_None;
Button_Stt_Type button2_Stt = Button_None;
Button_Stt_Type	button3_Stt	= Button_None;
Button_Stt_Type button4_Stt	= Button_None;
Button_Stt_Type button5_Stt	= Button_None;
static uint8_t  Reset_Count_Buff=0;

Relay_Stt_Type	relay_Stt[5] = {0};

CountDown_Str   CountDown_Val[5] = {
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0}
};

Sw_SingleBlink_Str Sw_SingleBlink_Val[NUM_OF_ELEMENT];

uint8_t 	pressResetCount	=0;
uint8_t 	countPower		=0;
uint8_t		powerSaved		=0;
uint8_t 	Traing_Loop 	=0;

uint8_t     Fist_Socket_Relay =0;
uint8_t 	Kick_all_Flag;
Sw_Working_Stt_Str   Sw_Working_Stt_Val;
Sw_press_K9BHC_Str   Sw_Press_K9BHC_Val;
Sw_Update_Stt_Str    Sw_Update_Stt_Val;
u8 element		= 2;

uint32_t Button_Pin_Array[NUM_OF_ELEMENT] 	= ARR_BUTTON_PIN;
uint32_t Reay_Pin_Array[NUM_OF_ELEMENT] 	= ARR_RELAY_PIN;

Sw_Woring_K9B_Str Sw_Woring_K9B_Val ={0};
#if(TYPE_LED == PWM_LED)
	uint32_t 	Led_Pin_Array[NUM_OF_ELEMENT]	= ARR_LED_PIN;
	pwm_id 		Led_PWM_Array[NUM_OF_ELEMENT]   = ARR_LED_PWM;
#endif

static uint16_t count_k9B_press[NUM_OF_ELEMENT] = {0};
static void RD_Switch_GPIO_Init(void)
{

#if 0
#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)

	gpio_setup_up_down_resistor(BT1_PIN, PM_PIN_PULLDOWN_100K);		gpio_set_func(BT1_PIN, AS_GPIO);
	gpio_set_input_en(BT1_PIN, 1);									gpio_set_output_en(BT1_PIN, 0);

	gpio_setup_up_down_resistor(LED1_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(LED1_PIN, AS_GPIO);
	gpio_set_input_en(LED1_PIN, 0);									gpio_set_output_en(LED1_PIN, 1);
	gpio_write(LED1_PIN, 1); // off
	gpio_setup_up_down_resistor(LED2_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(LED2_PIN, AS_GPIO);
	gpio_set_input_en(LED2_PIN, 0);	 								gpio_set_output_en(LED2_PIN, 1);
	gpio_write(LED2_PIN, 1);  // off

	gpio_setup_up_down_resistor(RELAY1_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(RELAY1_PIN, AS_GPIO);
	gpio_set_input_en(RELAY1_PIN, 0);								gpio_set_output_en(RELAY1_PIN, 1);
	gpio_write(RELAY1_PIN, Relay_Off);
#endif

#else  // new init
	/*-------------------------------------Detect-----------------------------------------------*/
	gpio_setup_up_down_resistor(DETECT_ZERO_PIN, PM_PIN_UP_DOWN_FLOAT); 		gpio_set_func(DETECT_ZERO_PIN, AS_GPIO);
	gpio_set_output_en(DETECT_ZERO_PIN, 0); 									gpio_set_input_en(DETECT_ZERO_PIN, 1);

	#if(TYPE_BUTTON == TOUCH_BUTTON)
		/*----------------------------------Reset Pin-------------------------------------------*/
		gpio_set_input_en(RESET_TOUCH_PIN, 0);							gpio_set_output_en(RESET_TOUCH_PIN, 1);
		gpio_write(RESET_TOUCH_PIN, TOUCH_ACTIVE_POW);
	#endif

	/*------------------------------------- LED RGB-----------------------------------------------*/
	#if(TYPE_LED == RGB_LED)
		gpio_setup_up_down_resistor(LC8823_CLK_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(LC8823_CLK_PIN, AS_GPIO);
		gpio_set_input_en(LC8823_CLK_PIN, 0);								gpio_set_output_en(LC8823_CLK_PIN, 1);
		gpio_write(LC8823_DATA_PIN, 1);

		gpio_setup_up_down_resistor(LC8823_DATA_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(LC8823_DATA_PIN, AS_GPIO);
		gpio_set_input_en(LC8823_DATA_PIN, 0);								gpio_set_output_en(LC8823_DATA_PIN, 1);
		gpio_write(LC8823_DATA_PIN, 1);
	#endif

	/*-------------------------------------- LED GREEN-RED---------------------------------------*/
	#if(TYPE_LED == RED_GREEN_LED)
			gpio_setup_up_down_resistor(LED1_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(LED1_PIN, AS_GPIO);
			gpio_set_input_en(LED1_PIN, 0);									gpio_set_output_en(LED1_PIN, 1);
			gpio_write(LED1_PIN, 1); // off
			gpio_setup_up_down_resistor(LED2_PIN, PM_PIN_PULLUP_10K);	gpio_set_func(LED2_PIN, AS_GPIO);
			gpio_set_input_en(LED2_PIN, 0);	 								gpio_set_output_en(LED2_PIN, 1);
			gpio_write(LED2_PIN, 1);  // off
	#endif

	for(int i=0; i< NUM_OF_ELEMENT; i++)
	{
		/*-------------------------------------Button-----------------------------------------------*/
		#if(LOGIC_ACTIVE_BT == 1)
			gpio_setup_up_down_resistor(Button_Pin_Array[i], PM_PIN_PULLDOWN_100K); 		gpio_set_func(Button_Pin_Array[i], AS_GPIO);
		#else
			gpio_setup_up_down_resistor(Button_Pin_Array[i], PM_PIN_PULLUP_10K); 		gpio_set_func(Button_Pin_Array[i], AS_GPIO);
		#endif

		gpio_set_output_en(Button_Pin_Array[i], 0); 								gpio_set_input_en(Button_Pin_Array[i], 1);

		/*----------------------------------------Relay-------------------------------------------------*/
		gpio_setup_up_down_resistor(Reay_Pin_Array[i], PM_PIN_UP_DOWN_FLOAT);	gpio_set_func(Reay_Pin_Array[i], AS_GPIO);
		gpio_set_input_en(Reay_Pin_Array[i], 0);								gpio_set_output_en(Reay_Pin_Array[i], 1);
		gpio_write(Reay_Pin_Array[i], Relay_Off);
		/*----------------------------------------Led PWM-------------------------------------------------*/
		#if(TYPE_LED == PWM_LED)
			pwm_set_clk(CLOCK_SYS_CLOCK_HZ, 3);
			gpio_set_func(Led_Pin_Array[i], AS_PWM);
			pwm_set_mode(Led_PWM_Array[i], PWM_NORMAL_MODE);
			//pwm_set_cycle_and_duty(Led_PWM_Array[i], 100, 90);
			pwm_set_cycle_and_duty(Led_PWM_Array[i], 100, 100 - LED_DIM_OFF_DF);
			pwm_start(Led_PWM_Array[i]);
		#endif
	}

#endif


}
static void RD_Switch_Check_Provision(void)
{
#if(TYPE_HARDWARE_SWITCH != X1_SOCKET)
if (get_provision_state() == STATE_DEV_UNPROV )
{
	#if(CONVERT_X1_TO_HIPOW_ON == 0)
	RD_SwitchAc4Ch_BlinkSet(7, 150);
	#else
	RD_SwitchAc4Ch_BlinkSet(7, 150);
	#endif

}

#endif
#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
if (get_provision_state() == STATE_DEV_UNPROV )
{
	for(int i=0; i<3; i ++)
	{
		gpio_write(RED_LED, 0);
		gpio_write(GREEN_LED, 0);
		wd_clear(); //clear watch dog
		sleep_ms(300);
		gpio_write(RED_LED, 1);
		gpio_write(GREEN_LED, 1);
		wd_clear(); //clear watch dog
		sleep_ms(300);
	}
}

	Fist_Socket_Relay = 1;

#endif
}
static void RD_Switch_Intro(void)
{
#if 1  // Intro
	char UART_TempSend[64];
	uint8_t Firm_Ver_H = VERSION_FIRM_H;
	uint8_t Firm_Ver_L = VERSION_FIRM_L;
	#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
		sprintf(UART_TempSend,"RD Socket x1 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
		sprintf(UART_TempSend,"RD Switch V3 x4 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	#if(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)
		sprintf(UART_TempSend,"RD Switch V3 x3 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)
		sprintf(UART_TempSend,"RD Switch V3 x2 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
		#if(CONVERT_X1_TO_HIPOW_ON == 0)
				sprintf(UART_TempSend,"RD Switch V3 x1 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
		#else
				sprintf(UART_TempSend,"RD Switch V3 Hi Pow x1 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
		#endif
	#endif
	#if(TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH)
		sprintf(UART_TempSend,"RD machine switch x1 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	#if(TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH)
		sprintf(UART_TempSend,"RD machine switch x2 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	#if(TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH)
		sprintf(UART_TempSend,"RD machine switch x Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	#endif
	uart_CSend(UART_TempSend);

	//mesh_key log
		#if 0
			char UART_TempSend2[128] = {0};
			uart_CSend("APPkey:");
			for(int i=0; i<16; i++)
			{

				sprintf(&UART_TempSend2[0],"0x%x-",mesh_key.net_key[0][0].app_key[0].key[i]);
				uart_CSend(UART_TempSend2);
			}

			uart_CSend("\n Netkey 1:");
			for(int i=0; i<16; i++)
			{
				sprintf(&UART_TempSend2[0],"0x%x-",mesh_key.net_key[0][0].key[i]);
				uart_CSend(UART_TempSend2);
			}
			uart_CSend("\n Netkey 2:");
			for(int i=0; i<16; i++)
			{
				sprintf(&UART_TempSend2[0],"0x%x-",mesh_key.net_key[0][1].key[i]);
				uart_CSend(UART_TempSend2);
			}
			uart_CSend("\n Netkey 3:");
			for(int i=0; i<16; i++)
			{
				sprintf(&UART_TempSend2[0],"0x%x-",mesh_key.net_key[1][0].key[i]);
				uart_CSend(UART_TempSend2);
			}
			uart_CSend("\n Netkey 4:");
			for(int i=0; i<16; i++)
			{
				sprintf(&UART_TempSend2[0],"0x%x-",mesh_key.net_key[1][1].key[i]);
				uart_CSend(UART_TempSend2);
			}
		#endif
#endif
}
static void RD_LoopK9BHC_Press(void);
void RD_Check_UpdateStt(void);
/*---------------------------------------- Init --------------------------------------------------*/
void RD_SwitchAC4Ch_Init(void)
{
	/*--------------------------------GPIO-------------------------------------*/
	RD_Switch_GPIO_Init();
	/*--------------------------------Flash------------------------------------*/
	RD_Flash_Init();
	/*--------------------------check Provision state--------------------------*/
	RD_Switch_Check_Provision();
	//RD_TODO: kick all
	/*------------------------Intro type switch and version--------------------*/
	RD_Switch_Intro();
	/*-------------------------Set data default for led------------------------*/
	LED_Init_Value();
	/*------------------------ Send Gw  device Stt -----------------------------*/
	Send_Relay_Stt_Message_RALL_PowerUp();
	
	#if(SECURE_ON == 1)
		RD_Secure_CheckInit();
	#endif


}


void RD_CheckActiveCountDown(void)
{
	static Relay_Stt_Type RelaySttLast[5] = {Relay_Off, Relay_Off, Relay_Off, Relay_Off, Relay_Off};

	// only high pow switch and mode off->on;
	for(int i=0; i < NUM_OF_ELEMENT; i++)
	{
		if((RelaySttLast[i] != relay_Stt[i]) && (Relay_On == relay_Stt[i]) && (0 != Sw_Flash_Data_Val.CountDown[i].Time_CountDown_s) )
		{
			RD_CountDown_Set(i, Sw_Flash_Data_Val.CountDown[i].Time_CountDown_s,  Sw_Flash_Data_Val.CountDown[i].Taget);
			uart_CSend("Set countDont\n");
		}

		// reset when  mode on->off;
		if((RelaySttLast[i] != relay_Stt[i]) && (Relay_Off == relay_Stt[i]) && (0 != Sw_Flash_Data_Val.CountDown[i].Time_CountDown_s) )
		{
			RD_CountDown_Set(i, 0x0000,  Sw_Flash_Data_Val.CountDown[i].Taget);
			uart_CSend("ReSet countDont\n");
		}
		RelaySttLast[i] = relay_Stt[i];
	}
}
/**
 * @brief Check count down stt and update on-off status
 * 
 */
static void RD_CountDown_Check(void)
{
	for(int i=0; i < ELE_CNT; i++)
	{
		if(CountDown_Val[i].Time_CountDown_s != 0)
		{
			if(CountDown_Val[i].Time_Start_CountDown_s > clock_time_s()) CountDown_Val[i].Time_Start_CountDown_s = clock_time_s();

			if(clock_time_s() - CountDown_Val[i].Time_Start_CountDown_s >= CountDown_Val[i].Time_CountDown_s )
			{
				switch(CountDown_Val[i].Taget)
				{
					case 0: // off
						relay_Stt[i] = 0;
						break;
					case 1: // on
						relay_Stt[i] = 1;
						break;
					case 2: // toggle
						relay_Stt[i] = !relay_Stt[i];
						break;
					default:
						uart_CSend("count dount wrong handle \n");
						break;
				}

				// send uart 
				char UART_TempSend[64];
				sprintf(UART_TempSend,"dount down relay: %d taget: %D \n",i, relay_Stt[i]);
				uart_CSend(UART_TempSend);
				
				// update control and stt
				RD_Call_LinkControl(i+1, relay_Stt[i]);
				sleep_ms(TIME_CYCLE_MESS_MS);
				RD_SetAndRsp_Switch(i, relay_Stt[i], RD_GATEWAYADDRESS);

				// reset val of count down
				CountDown_Val[i].Time_Start_CountDown_s =0;
				CountDown_Val[i].Time_CountDown_s =0;
				CountDown_Val[i].Taget =0;
			}
		}
	}
}

void RD_Socket_WaitDetechZero(void)
{
uint16_t overTimeBuff =0;
uint8_t detectRezoState_Buff, detectRezoState_Past_Buff, detectRezoState_Unnoise_Buff,  detectRezoState_Unnoise_Buff2;
detectRezoState_Unnoise_Buff = detectRezoState_Past_Buff = detectRezoState_Buff = gpio_read(DETECT_ZERO_PIN);
do
{
	overTimeBuff ++;
	detectRezoState_Past_Buff = gpio_read(DETECT_ZERO_PIN);
	sleep_us(50);
	detectRezoState_Buff = gpio_read(DETECT_ZERO_PIN);

	sleep_us(500);

	detectRezoState_Unnoise_Buff = gpio_read(DETECT_ZERO_PIN);
	sleep_us(50);
	detectRezoState_Unnoise_Buff2 = gpio_read(DETECT_ZERO_PIN);
	if(overTimeBuff>=NUM_CHECK_DETECH_MAX)
	{
		uart_CSend("break \n");
		break;
	}
}
while( !( (detectRezoState_Past_Buff != 0) && (detectRezoState_Buff != 0) && (detectRezoState_Unnoise_Buff == 0) && (detectRezoState_Unnoise_Buff2 ==0)) );

}

void RD_SwitchAC4Ch_ScanReset(void)
{
	/*--------------------------Factory Reset------------------------*/

	static uint16_t checkTimeBuff2=0;

//	#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH) )
	#if( NUM_OF_ELEMENT >= 2  )
		if((Button1_Hold_Flag + Button2_Hold_Flag + Button3_Hold_Flag +Button4_Hold_Flag + Button5_Hold_Flag) == 2)
			{
				checkTimeBuff2=0;
				Reset_Count_Buff++;
				Button1_Hold_Flag=0;
				Button2_Hold_Flag=0;
				Button3_Hold_Flag=0;
				Button4_Hold_Flag=0;
				Button5_Hold_Flag=0;
				RD_SwitchAc4Ch_BlinkSet(3, 150);
			}
	#endif

	#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH )
		if( Button1_Hold_Flag == 1)
		{
			checkTimeBuff2=0;
			Reset_Count_Buff++;
			Button1_Hold_Flag=0;
			Button2_Hold_Flag=0;
			Button3_Hold_Flag=0;
			Button4_Hold_Flag=0;

			#if(K9B_REMOTE_SUPPORT_ON)
				Sw1_PairK9B_OnOff_Flag =1; // set for pair K9B onoff. only for sw 1 element
				//RD_SwitchAc4Ch_BlinkSet(3, 150); // blink in pair k9b Set
			#else
						RD_SwitchAc4Ch_BlinkSet(3, 150); // blink in pair k9b Set
			#endif
		}
	#endif

	#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
		if( Button1_Hold_Flag == 1)
		{
			checkTimeBuff2=0;
			Reset_Count_Buff++;
			Button1_Hold_Flag=0;
			Button2_Hold_Flag=0;
			Button3_Hold_Flag=0;
			Button4_Hold_Flag=0;
			RD_SwitchAc4Ch_BlinkSet(3, 150);
		}
	#endif
		else
		{
			checkTimeBuff2++;
			if(checkTimeBuff2 >= TIME_OUT_RESET)
			{
				checkTimeBuff2=TIME_OUT_RESET;
				Reset_Count_Buff=0;
			}
		}

	if(Reset_Count_Buff == 3 ||   ( (reset_Mess_Flag== 1) && (get_provision_state() == STATE_DEV_UNPROV)) )
	{

		RD_Switchx4_FactoryReset();
	}

	RD_ScanKickAll();


}



static void RD_SW_SingleBlinkcan(void)
{
	uint8_t Control_Led_Flag =0;
	for(int i=0; i< NUM_OF_ELEMENT; i ++)
	{
		if(Sw_SingleBlink_Val[i].CycleBlink > 0) // has blink signal
		{
			if(clock_time_ms() - Sw_SingleBlink_Val[i].Last_ClockTime_Toggle >= Sw_SingleBlink_Val[i].Count_Toggle) // tongle time
			{
				
				if(Sw_SingleBlink_Val[i].CycleBlink%2 ==0 )
				{
					#if(TYPE_LED != RED_GREEN_LED)
						LED_Set_OnOff_Stt(i+1, 1);
					#else // Socket link 2 led when notication										
						gpio_write(RED_LED, LED_SOCKET_ON);
						gpio_write(GREEN_LED, LED_SOCKET_ON);
					#endif
				}
				else
				{
					#if(TYPE_LED != RED_GREEN_LED)
						LED_Set_OnOff_Stt(i+1, 0);
					#else // Socket link 2 led when notication										
						gpio_write(RED_LED, LED_SOCKET_OFF);
						gpio_write(GREEN_LED, LED_SOCKET_OFF);
					#endif
				}

				if(Sw_SingleBlink_Val[i].CycleBlink == 1)
				{
					#if(TYPE_LED != RED_GREEN_LED)
						LED_Set_OnOff_Stt(i+1, relay_Stt[i]);
					#else // Socket link 2 led when notication										
						gpio_write(RED_LED, relay_Stt[i]);
						gpio_write(GREEN_LED, relay_Stt[i]);
					#endif
				}

				Control_Led_Flag =1;
				Sw_SingleBlink_Val[i].CycleBlink--;
				Sw_SingleBlink_Val[i].Last_ClockTime_Toggle = clock_time_ms();
			}
		}
	}

	#if(TYPE_LED != RED_GREEN_LED)
	if(Control_Led_Flag ==1)
	{
		LED_Update_Stt();
	}

	#endif
}

void RD_SwitchAc4Ch_BlinkScan(void)
{

	static uint16_t TimeBlink_Buff=0;
	static uint8_t  provision_blink=0;
	static uint8_t send_onoff_provision_flag =0;
	static uint32_t clock_time_provision=0;
//	if((get_provision_state() == STATE_DEV_PROVING) && (provision_blink == 0))
//	{
//		provision_blink=1;
//		RD_SwitchAc4Ch_BlinkSet(50, 100);
//		send_onoff_provision_flag = 1;
//		clock_time_provision = clock_time_ms();
//	}
//	#if(FAST_PROVISION_EN_RD)
		if((get_provision_state() == STATE_DEV_UNPROV) && (provision_blink == 0))
		{
			provision_blink=1;

		}
//	#endif
	if((get_provision_state() == STATE_DEV_PROVED )&& (provision_blink == 1) )
	{
		send_onoff_provision_flag = 1;
		clock_time_provision = clock_time_ms();
		provision_blink=0;
		RD_SwitchAc4Ch_BlinkSet(10, 100);

	}

	if( (1 == send_onoff_provision_flag) && (clock_time_ms() - clock_time_provision >= 30000)) //5s
	{
		send_onoff_provision_flag = 0;
		Send_Relay_Stt_Message_RALL(RD_GATEWAYADDRESS);
	}
	if(CycleBlink >0)
	{
		TimeBlink_Buff++;

		if(TimeBlink_Buff>= CountToggleBlink)
		{
			TimeBlink_Buff=0;
			if(CycleBlink >1)
			{
				if(CycleBlink%2 == 0)
				{
					#if(TYPE_LED != RED_GREEN_LED)
						for(int i=0; i< NUM_OF_ELEMENT; i++)
						{
							LED_Set_OnOff_Stt(i+1, 1);
						}
						LED_Update_Stt();
					#else // Socket link 2 led when notication										
						gpio_write(RED_LED, LED_SOCKET_ON);
						gpio_write(GREEN_LED, LED_SOCKET_ON);
					#endif

				}
				else
				{
					#if(TYPE_LED != RED_GREEN_LED)
						for(int i=0; i< NUM_OF_ELEMENT; i++)
						{
							LED_Set_OnOff_Stt(i+1, 0);
						}
						LED_Update_Stt();
					#else // Socket link 2 led when notication										
						gpio_write(RED_LED, LED_SOCKET_OFF);
						gpio_write(GREEN_LED, LED_SOCKET_OFF);
					#endif

				}
			}

			if(CycleBlink == 1)								// reset led. set as relay stt
			{
				for(int i=0; i< 5; i++)
				{
					LED_Set_OnOff_Stt(i+1, relay_Stt[i]);
				}
				LED_Update_Stt();

			}

			CycleBlink--;
		}
	}

	// blink single led check
	RD_SW_SingleBlinkcan();
}

void RD_SwitchAc4Ch_BlinkSet(uint8_t cycle, uint16_t countToggle)
{
	CycleBlink		 = cycle;
	CountToggleBlink = countToggle;
}

void RD_Sw_SingleBlinkSet(uint8_t ID_Led, uint16_t cycle, uint16_t countToggle)
{
	SETMAX(ID_Led, NUM_OF_ELEMENT);
	Sw_SingleBlink_Val[ID_Led -1].CycleBlink = cycle;
	Sw_SingleBlink_Val[ID_Led -1].Count_Toggle =countToggle;
}
/*----------------------------------- loop process---------------------------------------------*/
void RD_SwitchAC4Ch_Loop(void)
{
	static uint16_t timeLoop=0;
	static uint64_t clockTick_ReadBt_ms=0;
	timeLoop++;
	if(timeLoop >= 200)
	{

		timeLoop=0;

		#if 0
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Toggle%x \n",relay_stt);
			uart_CSend(UART_TempSend);
		#endif

	}
	/*-------------------------- Scan button-------------------------*/
	if(clock_time_ms() < clockTick_ReadBt_ms) clockTick_ReadBt_ms = clock_time_ms();
	if(clock_time_ms() - clockTick_ReadBt_ms >= CYCLE_READ_BT_MS)
	{
		clockTick_ReadBt_ms = clock_time_ms();
		RD_SwitchAC4Ch_ScanB_V2();
	}

	/*------------------ Check end control factory mode----------------*/
	RD_Train_Factory();
	/*---------------------------Count down check-------------------------*/
	#if(COUNT_DOWN_EN)// only for high power switch
		RD_CheckActiveCountDown();
		RD_CountDown_Check();
	#endif
	/*------------------ Check Status anh update control -------------*/
	RD_SwitchAC4Ch_UpdateCtr();
	/*------------------Check Reset factory by user-------------------*/
	RD_SwitchAC4Ch_ScanReset();

	#if(K9B_REMOTE_SUPPORT_ON)
		/*------------------Check Scan pair on/off K9B and PairHc-------------------*/
		RD_K9B_ScanPairOnOff();
		//RD_K9B_ScanPairHC();
		RD_K9B_TimeOutScanK9BHC();

	//	RD_LoopK9BHC_Press();
	#endif
	/*----------------- Scan blink status and control led ------------*/
	RD_SwitchAc4Ch_BlinkScan();

	/* check control by Scene or on-off by orther gw */
	RD_Check_UpdateStt();
	/*------------------ Check keep button long and reset touch IC------*/
	#if(TYPE_BUTTON == TOUCH_BUTTON)
		RD_Check_RST_Touch_IC();
	#endif

	#if(SECURE_ON == 1)
		RD_Secure_CheckLoop();
	#endif
}

//#warning "TEST count K9B"
//extern uint32_t count_k9B_mess;
void RD_SwitchAC4Ch_ScanB_V2(void)
{
	static uint8_t Bt1_Count_Buff =0;
	static uint8_t Bt2_Count_Buff =0;
	static uint8_t Bt3_Count_Buff =0;
	static uint8_t Bt4_Count_Buff =0;
	static uint8_t Bt5_Count_Buff =0;

	static uint8_t buttonSttBuff[5] = {0};

#if(NUM_OF_ELEMENT >= 1 )
/*------------------------------------------BT1----------------------------------------------------*/
	#if(TYPE_HARDWARE_SWITCH != X1_SOCKET)
			#if(LOGIC_ACTIVE_BT == 1)
				if(BT1_READ != 0)
			#else
				if(BT1_READ == 0)
			#endif
	#endif
	#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
			if(BT1_READ == 0)
	#endif

			{
				Bt1_Count_Buff++;
				if(Bt1_Count_Buff == CYCLE_ACTIVE_BUTTON)
				{
					buttonSttBuff[0] =1;
				}
				if(Bt1_Count_Buff>= (CYCLE_ACTIVE_BUTTON+1))
				{
					Bt1_Count_Buff = CYCLE_ACTIVE_BUTTON+1;
				}
			}
			else
			{
					Bt1_Count_Buff=0;
					buttonSttBuff[0] =0;
			}

			//------------------update BT1 status------------------------//
			if( (buttonSttBuff[0]==1) && (button1_Stt == Button_None) )
			{
				uart_CSend("Bt 1 Press \n");
				button1_Stt = Button_Press;

//				#warning "TEST count K9B"
//				RD_MessK9BHc_Press(count_k9B_press[0], 0, 0,count_k9B_mess/10);

				Button1_Keep_Flag =1;

				if( (RD_K9B_Pair_OnOffGetFlag() == 1) )
				{
					RD_Flash_DeleteAllK9BOnOff(1);
					RD_K9B_Pair_OnOffClearFlag(); //clear phase working
					RD_Sw_SingleBlinkSet(1, 5, 200);
				}
			}
			else if(buttonSttBuff[0] == 0)
			{
				if(button1_Stt == Button_Keeping  )
				{
					uart_CSend("Reset bt Stt \n");
				}
				button1_Stt = Button_None;
				Button1_Keep_Flag =0;
			}
#endif

//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH) )
#if(NUM_OF_ELEMENT >= 2 )
/*------------------------------------------BT2----------------------------------------------------*/
			//if(BT2_READ == LOGIC_ACTIVE_BT)
			#if(LOGIC_ACTIVE_BT == 1)
				if(BT2_READ != 0)
			#else
				if(BT2_READ == 0)
			#endif
			{
				Bt2_Count_Buff++;
				if(Bt2_Count_Buff == CYCLE_ACTIVE_BUTTON)
				{
					//uart_CSend("BT2 On \n");
					buttonSttBuff[1] = 1;
				}
				if(Bt2_Count_Buff >= (CYCLE_ACTIVE_BUTTON+1))
				{
					Bt2_Count_Buff= CYCLE_ACTIVE_BUTTON+1;
				}
			}
			else
			{
					Bt2_Count_Buff=0;
					buttonSttBuff[1] =0;
			}

			//------------------update BT2 status------------------------//
			if( (buttonSttBuff[1]==1) && (button2_Stt == Button_None) )
			{
				uart_CSend("Bt 2 Press \n");
				button2_Stt = Button_Press;
				Button2_Keep_Flag =1;

				if( (RD_K9B_Pair_OnOffGetFlag() == 2) )
				{
					RD_Flash_DeleteAllK9BOnOff(2);
					RD_K9B_Pair_OnOffClearFlag(); //clear phase working
					RD_Sw_SingleBlinkSet(2, 5, 200);
				}
			}
			else if(buttonSttBuff[1] == 0)
			{
				if(button2_Stt == Button_Keeping  )
				{
					uart_CSend("Reset bt Stt \n");
				}
				button2_Stt = Button_None;
				Button2_Keep_Flag =0;
			}
#endif

//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) )
#if(NUM_OF_ELEMENT >= 3)
/*------------------------------------------BT3----------------------------------------------------*/
			//if(BT3_READ == LOGIC_ACTIVE_BT)
			#if(LOGIC_ACTIVE_BT == 1)
				if(BT3_READ != 0)
			#else
				if(BT3_READ == 0)
			#endif
			{
				Bt3_Count_Buff++;
				if(Bt3_Count_Buff == CYCLE_ACTIVE_BUTTON)
				{
					//uart_CSend("BT3 On \n");
					buttonSttBuff[2] =1;
				}
				if(Bt3_Count_Buff>=(CYCLE_ACTIVE_BUTTON+1))
				{
					Bt3_Count_Buff= (CYCLE_ACTIVE_BUTTON+1);
				}
			}
			else
			{
					Bt3_Count_Buff=0;
					buttonSttBuff[2] =0;
			}

			//------------------update BT3 status------------------------//
			if( (buttonSttBuff[2]==1) && (button3_Stt == Button_None) )
			{
				uart_CSend("Bt 3 Press \n");
				button3_Stt = Button_Press;
				Button3_Keep_Flag =1;

				if((RD_K9B_Pair_OnOffGetFlag() == 3) )
				{
					RD_Flash_DeleteAllK9BOnOff(3);
					RD_K9B_Pair_OnOffClearFlag(); //clear phase working
					RD_Sw_SingleBlinkSet(3, 5, 200);
				}
			}
			else if(buttonSttBuff[2] == 0)
			{
				if(button3_Stt == Button_Keeping  )
				{
					uart_CSend("Reset bt Stt \n");
				}
				button3_Stt = Button_None;
				Button3_Keep_Flag =0;
			}
#endif

//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
#if(NUM_OF_ELEMENT >= 4 )
/*------------------------------------------BT4----------------------------------------------------*/
			//if(BT4_READ  == LOGIC_ACTIVE_BT)
			#if(LOGIC_ACTIVE_BT == 1)
				if(BT4_READ != 0)
			#else
				if(BT4_READ == 0)
			#endif
			{
				Bt4_Count_Buff++;
				if(Bt4_Count_Buff == CYCLE_ACTIVE_BUTTON)
				{
					//uart_CSend("BT4 On \n");
					buttonSttBuff[3] =1;
				}
				if(Bt4_Count_Buff >= (CYCLE_ACTIVE_BUTTON+1))
				{
					Bt4_Count_Buff = CYCLE_ACTIVE_BUTTON;
				}
			}
			else
			{
					Bt4_Count_Buff=0;
					buttonSttBuff[3] =0;
			}

			//------------------update BT4 status------------------------//
			if( (buttonSttBuff[3]==1) && (button4_Stt == Button_None) )
			{
				uart_CSend("Bt 4 Press \n");
				button4_Stt = Button_Press;
				Button4_Keep_Flag =1;

				if( (RD_K9B_Pair_OnOffGetFlag() == 4) )
				{
					RD_Flash_DeleteAllK9BOnOff(4);
					RD_K9B_Pair_OnOffClearFlag(); //clear phase working
					RD_Sw_SingleBlinkSet(4, 5, 200);
				}
			}
			else if(buttonSttBuff[3] == 0)
			{
				if(button4_Stt == Button_Keeping  )
				{
					uart_CSend("Reset bt Stt \n");
				}
				button4_Stt = Button_None;
				Button4_Keep_Flag =0;
			}
#endif

#if(NUM_OF_ELEMENT >= 5 )
/*------------------------------------------BT5----------------------------------------------------*/
			//if(BT5_READ  == LOGIC_ACTIVE_BT)
			#if(LOGIC_ACTIVE_BT == 1)
				if(BT5_READ != 0)
			#else
				if(BT5_READ == 0)
			#endif
			{
				Bt5_Count_Buff++;
				if(Bt5_Count_Buff == CYCLE_ACTIVE_BUTTON)
				{
					buttonSttBuff[4] =1;
				}
				if(Bt5_Count_Buff >= (CYCLE_ACTIVE_BUTTON+1))
				{
					Bt5_Count_Buff = CYCLE_ACTIVE_BUTTON;
				}
			}
			else
			{
					Bt5_Count_Buff=0;
					buttonSttBuff[4] =0;
			}

			//------------------update BT4 status------------------------//
			if( (buttonSttBuff[4]==1) && (button5_Stt == Button_None) )
			{
				uart_CSend("Bt 5 Press \n");
				button5_Stt = Button_Press;
				Button5_Keep_Flag =1;

				if( (RD_K9B_Pair_OnOffGetFlag() == 5) )
				{
					RD_Flash_DeleteAllK9BOnOff(5);
					RD_K9B_Pair_OnOffClearFlag(); //clear phase working
					RD_Sw_SingleBlinkSet(5, 5, 200);
				}
			}
			else if(buttonSttBuff[4] == 0)
			{
				if(button5_Stt == Button_Keeping  )
				{
					uart_CSend("Reset bt Stt \n");
				}
				button5_Stt = Button_None;
				Button5_Keep_Flag =0;
			}
#endif
 RD_SwitchAC4Ch_CheckHold( );
	/*------------------------------- Check Hold Button-------------------------------*/
}

void RD_SwitchAC4Ch_CheckHold(void)
{
	static uint16_t Count_BuffBT1, Count_BuffBT2, Count_BuffBT3, Count_BuffBT4, Count_BuffBT5;
	static bool4 Flag_Print1, Flag_Print2, Flag_Print3 =0, Flag_Print4=0, Flag_Print5=0;
//--------------------BUTTON 1 --------------------//
	if(button1_Stt == Button_Keeping)
	{
//		uart_CSend("BT1 Keep\n");
		Count_BuffBT1++;
		if(Count_BuffBT1>= 50000) Count_BuffBT1=50000;
		if(Count_BuffBT1 >= TIME_HOLD_BT)
		{

			if(Flag_Print1 == 0)
			{
				Button1_Hold_Flag =1;
				Flag_Print1=1;
			    uart_CSend("BT1 Hold\n");
			}
		}

	}
	else
	{
		Count_BuffBT1=0;
		Flag_Print1=0;
	}


	//--------------------BUTTON 2 --------------------//
//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)  )
#if( NUM_OF_ELEMENT >= 2  )

	if(button2_Stt == Button_Keeping)
	{
		Count_BuffBT2++;
		if(Count_BuffBT2>= 50000) Count_BuffBT2=50000;
		if(Count_BuffBT2 >= TIME_HOLD_BT)
		{

			if(Flag_Print2 == 0)
			{
				Button2_Hold_Flag =1;
				Flag_Print2=1;
			    uart_CSend("BT2 Hold\n");
			}
		}
	}
	else
	{
		Count_BuffBT2=0;
		Flag_Print2=0;
	}
#endif

//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) ) ///BUTTON3
#if( NUM_OF_ELEMENT >= 3  )
	if(button3_Stt == Button_Keeping)
	{
		Count_BuffBT3++;
		if(Count_BuffBT3>= 50000) Count_BuffBT3=50000;
		if(Count_BuffBT3 >= TIME_HOLD_BT)
		{

			if(Flag_Print3 == 0)
			{
				Button3_Hold_Flag =1;
				Flag_Print3=1;
			    uart_CSend("BT3 Hold\n");
			}
		}
	}
	else
	{

		Count_BuffBT3=0;
		Flag_Print3=0;
	}
#endif

//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)  	//BUTTON 4
#if( NUM_OF_ELEMENT >= 4)
	if(button4_Stt == Button_Keeping)
	{
		Count_BuffBT4++;
		if(Count_BuffBT4>= 50000) Count_BuffBT4=50000;
		if(Count_BuffBT4 >= TIME_HOLD_BT)
		{

			if(Flag_Print4 == 0)
			{
				Button4_Hold_Flag =1;
				Flag_Print4=1;
			    uart_CSend("BT4 Hold\n");
			}
		}
	}
	else
	{

		Count_BuffBT4=0;
		Flag_Print4=0;
	}
#endif

#if( NUM_OF_ELEMENT >= 5)
	if(button5_Stt == Button_Keeping)
	{
		Count_BuffBT5++;
		if(Count_BuffBT5>= 50000) Count_BuffBT5=50000;
		if(Count_BuffBT5 >= TIME_HOLD_BT)
		{

			if(Flag_Print5 == 0)
			{
				Button5_Hold_Flag =1;
				Flag_Print5=1;
			    uart_CSend("BT5 Hold\n");
			}
		}
	}
	else
	{

		Count_BuffBT5=0;
		Flag_Print5=0;
	}
#endif
}

void RD_SwitchAC4Ch_UpdateCtr(void)
{
	/*--------------------update Relay Stt------------------	 */
	if(button1_Stt == Button_Press)
	{
		uart_CSend("BT1 To Keep\n");
		button1_Stt = Button_Keeping;
		relay_Stt[0] = !relay_Stt[0];
		RD_Call_LinkControl(1, relay_Stt[0]);
		RD_SetAndRsp_Switch(0, relay_Stt[0], RD_GATEWAYADDRESS);
	}

//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)  )
#if(NUM_OF_ELEMENT >=2)
	if(button2_Stt == Button_Press)
	{
		button2_Stt = Button_Keeping;
		relay_Stt[1] = !relay_Stt[1];
		RD_Call_LinkControl(2, relay_Stt[1]);
		RD_SetAndRsp_Switch(1, relay_Stt[1], RD_GATEWAYADDRESS);
	}
#endif

//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)  )
#if(NUM_OF_ELEMENT >=3)
	if(button3_Stt == Button_Press)
	{
		button3_Stt = Button_Keeping;
		relay_Stt[2] = !relay_Stt[2];
		RD_Call_LinkControl(3, relay_Stt[2]);
		#if 0 //RD_Edit: control room test
				access_cmd_onoff(0xC000, 0, relay_Stt[2], CMD_NO_ACK, 0x0000);
		#endif
		RD_SetAndRsp_Switch(2, relay_Stt[2], RD_GATEWAYADDRESS);
	}
#endif

//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
#if(NUM_OF_ELEMENT >=4)
	if(button4_Stt == Button_Press)
	{
		button4_Stt = Button_Keeping;
		relay_Stt[3] = !relay_Stt[3];
		RD_Call_LinkControl(4, relay_Stt[3]);
		RD_SetAndRsp_Switch(3, relay_Stt[3], RD_GATEWAYADDRESS);
	}
#endif

#if(NUM_OF_ELEMENT >= 5)
	if(button5_Stt == Button_Press)
	{
		button5_Stt = Button_Keeping;
		relay_Stt[4] = !relay_Stt[4];
		RD_Call_LinkControl(5, relay_Stt[4]);
		RD_SetAndRsp_Switch(4, relay_Stt[4], RD_GATEWAYADDRESS);
	}
#endif
#if 1
	/* Update Relay Stt From Light_Index */
	u8 relay_stt_buff[5] ={0};
	for(int i=0; i<LIGHT_CNT; i++)
	{
		mesh_cmd_g_onoff_st_t onoff_stt = {0};

		mesh_g_onoff_st_rsp_par_fill(&onoff_stt, i);
		relay_stt_buff[i] = onoff_stt.present_onoff;
	}
	relay_Stt[0] = relay_stt_buff[0];
	relay_Stt[1] = relay_stt_buff[1];
	relay_Stt[2] = relay_stt_buff[2];
	relay_Stt[3] = relay_stt_buff[3];
	relay_Stt[4] = relay_stt_buff[4];
#endif
	/*-------------------------Recontrol relay and led----------------*/
	static uint8_t time_CountBuff=0;
	static  Relay_Stt_Type Relay1LastStt_Buff = Relay_Off;
	static  Relay_Stt_Type Relay2LastStt_Buff = Relay_Off;
	static  Relay_Stt_Type Relay3LastStt_Buff = Relay_Off;
	static  Relay_Stt_Type Relay4LastStt_Buff = Relay_Off;
	static  Relay_Stt_Type Relay5LastStt_Buff = Relay_Off;

	time_CountBuff++;
	if(time_CountBuff>=2)
	{
		time_CountBuff=0;

#if 0 // old check. 23 12 2023
	//#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH) ||  (TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH))
	#if(NUM_OF_ELEMENT >=2)
				if(Relay1LastStt_Buff != relay_Stt[0])								// relay 1
				{
					LED_Set_OnOff_Stt(1, relay_Stt[0]);
					LED_Update_Stt();

					if(relay_Stt[0]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_Off);
					else							RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_On);

					Relay1LastStt_Buff = relay_Stt[0];

					if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
				}

				else if(Relay2LastStt_Buff != relay_Stt[1])								// relay 2
				{
					LED_Set_OnOff_Stt(2, relay_Stt[1]);
					LED_Update_Stt();

					if(relay_Stt[1]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY2_PIN, Relay_Off);
					else							RD_SwitchAC4Ch_OnOffZero(RELAY2_PIN, Relay_On);

					Relay2LastStt_Buff = relay_Stt[1];
					if(get_provision_state() != STATE_DEV_PROVED)  RD_Flash_SavePowerOff();
				}

				#if((TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH))
				else if(Relay3LastStt_Buff != relay_Stt[2])								// relay 3
				{
					LED_Set_OnOff_Stt(3, relay_Stt[2]);
					LED_Update_Stt();

					if(relay_Stt[2]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY3_PIN, Relay_Off);
					else							RD_SwitchAC4Ch_OnOffZero(RELAY3_PIN, Relay_On);

					Relay3LastStt_Buff = relay_Stt[2];
					if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
				}
				#endif

				#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
				else if(Relay4LastStt_Buff != relay_Stt[3])								// relay 4
				{
					LED_Set_OnOff_Stt(4, relay_Stt[3]);
					LED_Update_Stt();

					if(relay_Stt[3]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY4_PIN, Relay_Off);
					else							RD_SwitchAC4Ch_OnOffZero(RELAY4_PIN, Relay_On);

					Relay4LastStt_Buff = relay_Stt[3];
					if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
				}
				#endif
	#endif

	#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH)
				if(Relay1LastStt_Buff != relay_Stt[0])								// relay 1
				{
					LED_Set_OnOff_Stt(1, relay_Stt[0]);
					LED_Update_Stt();

					if(relay_Stt[0]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_Off);
					else							RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_On);

					Relay1LastStt_Buff = relay_Stt[0];
					if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
				}
	#endif
#else // after 23 12 2023
		if(Relay1LastStt_Buff != relay_Stt[0])								// relay 1
		{
			LED_Set_OnOff_Stt(1, relay_Stt[0]);
			LED_Update_Stt();

			if(relay_Stt[0]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_Off);
			else							RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_On);

			Relay1LastStt_Buff = relay_Stt[0];
			if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
		}
	#if(NUM_OF_ELEMENT >=2)
		else if(Relay2LastStt_Buff != relay_Stt[1])								// relay 2
		{
			LED_Set_OnOff_Stt(2, relay_Stt[1]);
			LED_Update_Stt();

			if(relay_Stt[1]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY2_PIN, Relay_Off);
			else							RD_SwitchAC4Ch_OnOffZero(RELAY2_PIN, Relay_On);

			Relay2LastStt_Buff = relay_Stt[1];
			if(get_provision_state() != STATE_DEV_PROVED)  RD_Flash_SavePowerOff();
		}
	#endif
	#if(NUM_OF_ELEMENT >= 3)
		else if(Relay3LastStt_Buff != relay_Stt[2])								// relay 3
		{
			LED_Set_OnOff_Stt(3, relay_Stt[2]);
			LED_Update_Stt();

			if(relay_Stt[2]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY3_PIN, Relay_Off);
			else							RD_SwitchAC4Ch_OnOffZero(RELAY3_PIN, Relay_On);

			Relay3LastStt_Buff = relay_Stt[2];
			if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
		}
	#endif
	#if(NUM_OF_ELEMENT >= 4)
		else if(Relay4LastStt_Buff != relay_Stt[3])								// relay 4
		{
			LED_Set_OnOff_Stt(4, relay_Stt[3]);
			LED_Update_Stt();

			if(relay_Stt[3]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY4_PIN, Relay_Off);
			else							RD_SwitchAC4Ch_OnOffZero(RELAY4_PIN, Relay_On);

			Relay4LastStt_Buff = relay_Stt[3];
			if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
		}
	#endif
	#if(NUM_OF_ELEMENT >= 5)
		else if(Relay5LastStt_Buff != relay_Stt[4])								// relay 5
		{
			LED_Set_OnOff_Stt(5, relay_Stt[4]);
			LED_Update_Stt();

			if(relay_Stt[4]  ==  Relay_Off)	RD_SwitchAC4Ch_OnOffZero(RELAY5_PIN, Relay_Off);
			else							RD_SwitchAC4Ch_OnOffZero(RELAY5_PIN, Relay_On);

			Relay5LastStt_Buff = relay_Stt[4];
			if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
		}
	#endif
#endif
#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
			if((Relay1LastStt_Buff != relay_Stt[0]) || (Fist_Socket_Relay ==1))								// relay 1
			{
				LED_Set_OnOff_Stt(1, relay_Stt[0]);
				LED_Update_Stt();

				Fist_Socket_Relay =0;
				if(relay_Stt[0]  ==  Relay_Off)
				{
					RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_Off);
				}
				else
				{
					RD_SwitchAC4Ch_OnOffZero(RELAY1_PIN, Relay_On);
				}

				Relay1LastStt_Buff = relay_Stt[0];
				if(get_provision_state() != STATE_DEV_PROVED) RD_Flash_SavePowerOff();
			}
#endif
//		Relay1LastStt_Buff = relay_Stt[0];
//		Relay2LastStt_Buff = relay2_Stt;
//		Relay3LastStt_Buff = relay3_Stt;
//		Relay4LastStt_Buff = relay4_Stt;

	}

}

void RD_SwitchAC4Ch_OnOffZero(GPIO_PinTypeDef RelayPin, Relay_Stt_Type OnOff_State)
{

	#if (METHOD_DETECT_ZERO == DETECT_ZERO_BY_FAILING)
		RD_Socket_WaitDetechZero();
		if(OnOff_State == Relay_On)
		{
			sleep_us(TIME_DETECT_ON);  // for switch 1-2-3-4 4700
		}
		else
			sleep_us(TIME_DETECT_OFF);    // for switch 1-2-3-4 3200
	#endif
	gpio_write(RelayPin, OnOff_State);



}

#if 0

#endif

void RD_ScanKickAll(void)
{
	if(Kick_all_Flag==1)
	{
		static uint32_t countDownt =0;
		static uint8_t check_reset_Flag=0;
		if(check_reset_Flag == 0)
		{
			check_reset_Flag =1;
			countDownt = clock_time_ms();
		}
		if( (check_reset_Flag == 1) && ((clock_time_ms() - countDownt) >= TIME_RESET_ALL) )
		{
			uart_CSend("\nFactory Reset\n");
			RD_Switchx4_FactoryReset();
		}
	}
}

void RD_Switchx4_FactoryReset(void)
{
	for(int i=0; i<5; i ++)
	{
		//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH )
		for(int i=1; i<= NUM_OF_ELEMENT; i++)
		{
			LED_Set_OnOff_Stt(i, 1);
		}
		LED_Update_Stt();
		sleep_ms(200);
		for(int i=1; i<= NUM_OF_ELEMENT; i++)
		{
			LED_Set_OnOff_Stt(i, 0);
		}
		LED_Update_Stt();
		sleep_ms(200);
		wd_clear(); //clear watch dog
#if 0 // old
		#if(NUM_OF_ELEMENT == 4 )
			LED_Set_OnOff_Stt(1, 1);
			LED_Set_OnOff_Stt(2, 1);
			LED_Set_OnOff_Stt(3, 1);
			LED_Set_OnOff_Stt(4, 1);
			LED_Update_Stt();
			sleep_ms(200);
			LED_Set_OnOff_Stt(1, 0);
			LED_Set_OnOff_Stt(2, 0);
			LED_Set_OnOff_Stt(3, 0);
			LED_Set_OnOff_Stt(4, 0);
			LED_Update_Stt();
			sleep_ms(200);

			wd_clear(); //clear watch dog
		#endif

		//#if(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)
		#if(NUM_OF_ELEMENT == 3 )
			LED_Set_OnOff_Stt(1, 1);
			LED_Set_OnOff_Stt(2, 1);
			LED_Set_OnOff_Stt(3, 1);
			LED_Update_Stt();
			sleep_ms(200);
			LED_Set_OnOff_Stt(1, 0);
			LED_Set_OnOff_Stt(2, 0);
			LED_Set_OnOff_Stt(3, 0);
			LED_Update_Stt();
			sleep_ms(200);

			wd_clear(); //clear watch dog
		#endif

		//#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)
		#if(NUM_OF_ELEMENT == 2 )
			LED_Set_OnOff_Stt(1, 1);
			LED_Set_OnOff_Stt(2, 1);
			LED_Update_Stt();

			sleep_ms(200);
			LED_Set_OnOff_Stt(1, 0);
			LED_Set_OnOff_Stt(2, 0);
			LED_Update_Stt();
			sleep_ms(200);

			wd_clear(); //clear watch dog
		#endif

		#if(NUM_OF_ELEMENT ==1  )
			#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH)

				LED_Set_OnOff_Stt(1, 1);
				LED_Update_Stt();
				sleep_ms(200);
				LED_Set_OnOff_Stt(1, 0);
				LED_Update_Stt();
				sleep_ms(200);

				wd_clear(); //clear watch dog
			#endif
			#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
				gpio_write(RED_LED, 1);
				gpio_write(GREEN_LED, 1);
				sleep_ms(200);
				gpio_write(RED_LED, 0);
				gpio_write(GREEN_LED, 0);
				sleep_ms(200);
				wd_clear(); //clear watch dog
			#endif
		#endif
#endif
	}
	if((get_provision_state() == STATE_DEV_UNPROV))
	{
		reset_Mess_Flag = 0;
	}
	RD_Flash_Save_DataDefault();
	sleep_ms(200);
	factory_reset();
	start_reboot();
}

void RD_Switchx4_GW_FacReset(void) //RD_Todo:
{
	for(int i=0; i<5; i ++)
	{
		//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH )
		for(int i=1; i<= NUM_OF_ELEMENT; i++)
		{
			LED_Set_OnOff_Stt(i, 1);
		}
		LED_Update_Stt();
		sleep_ms(200);
		for(int i=1; i<= NUM_OF_ELEMENT; i++)
		{
			LED_Set_OnOff_Stt(i, 0);
		}
		LED_Update_Stt();
		sleep_ms(200);
		wd_clear(); //clear watch dog
	}
	if((get_provision_state() == STATE_DEV_UNPROV))
	{
		reset_Mess_Flag = 0;
	}
	RD_Flash_Save_DataDefault();
	sleep_ms(200);

#if 0 // old
	for(int i=0; i<5; i ++)
	{
		//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
		#if(NUM_OF_ELEMENT == 4 )
		LED_Set_OnOff_Stt(1, 1);
		LED_Set_OnOff_Stt(2, 1);
		LED_Set_OnOff_Stt(3, 1);
		LED_Set_OnOff_Stt(4, 1);
		LED_Update_Stt();
		sleep_ms(200);
		LED_Set_OnOff_Stt(1, 0);
		LED_Set_OnOff_Stt(2, 0);
		LED_Set_OnOff_Stt(3, 0);
		LED_Set_OnOff_Stt(4, 0);
		LED_Update_Stt();
		sleep_ms(200);

			wd_clear(); //clear watch dog
		#endif

		//#if(TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH)
		#if(NUM_OF_ELEMENT == 3 )
			#if (PCB_VERSION == PCB_V1)
				LED_Set_OnOff_Stt(1, 1);
				LED_Set_OnOff_Stt(2, 1);
				LED_Set_OnOff_Stt(3, 1);
				LED_Update_Stt();
				sleep_ms(200);
				LED_Set_OnOff_Stt(1, 0);
				LED_Set_OnOff_Stt(2, 0);
				LED_Set_OnOff_Stt(3, 0);
				LED_Update_Stt();
				sleep_ms(200);
			#endif
			#if (PCB_VERSION == PCB_V2)
				LED1_PWM_H;
				LED2_PWM_H;
				LED3_PWM_H;
				sleep_ms(200);
				LED1_PWM_L;
				LED2_PWM_L;
				LED3_PWM_L;
				sleep_ms(200);
			#endif
			wd_clear(); //clear watch dog
		#endif

		//#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH)
		#if(NUM_OF_ELEMENT == 2 )
			#if (PCB_VERSION == PCB_V1)
				LED_Set_OnOff_Stt(1, 1);
				LED_Set_OnOff_Stt(2, 1);
				LED_Update_Stt();
				sleep_ms(200);
				LED_Set_OnOff_Stt(1, 0);
				LED_Set_OnOff_Stt(2, 0);
				LED_Update_Stt();
				sleep_ms(200);
			#endif

			wd_clear(); //clear watch dog
		#endif

		#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
			#if (PCB_VERSION == PCB_V1)
				LED_Set_OnOff_Stt(1, 1);
				LED_Set_OnOff_Stt(2, 1);
				LED_Update_Stt();
				sleep_ms(200);
				LED_Set_OnOff_Stt(1, 0);
				LED_Set_OnOff_Stt(2, 0);
				LED_Update_Stt();
				sleep_ms(200);
			#endif
			#if (PCB_VERSION == PCB_V2)
				LED1_PWM_H;
				LED2_PWM_H;
				sleep_ms(200);
				LED1_PWM_L;
				LED2_PWM_L;
				sleep_ms(200);
			#endif
			wd_clear(); //clear watch dog
		#endif
		#if(TYPE_HARDWARE_SWITCH == X1_SOCKET)
			gpio_write(RED_LED, 1);
			gpio_write(GREEN_LED, 1);
			sleep_ms(200);
			gpio_write(GREEN_LED, 0);
			gpio_write(RED_LED, 0);
			sleep_ms(200);
			wd_clear(); //clear watch dog
		#endif
	}
#endif

}
#if(TYPE_BUTTON == TOUCH_BUTTON)

static void RD_Check_Cycle_RST_Touch_IC(void)
{
	static uint32_t Time_Last_Rst=0;
	if(clock_time_s() - Time_Last_Rst > TIME_CYCLE_RST_TOUCH_S)
	{
		Time_Last_Rst = clock_time_s();

		uart_CSend("Reset Touch IC cycle \n");
		gpio_write(RESET_TOUCH_PIN, TOUCH_NON_ACTIVE_POW);
		sleep_ms(200);
		gpio_write(RESET_TOUCH_PIN, TOUCH_ACTIVE_POW);

	}
}
void RD_Check_RST_Touch_IC(void)
{
	static uint16_t Count_BuffBT1 = 0;
	static uint16_t Count_BuffBT2 = 0;
	static uint16_t Count_BuffBT3 = 0;
	static uint16_t Count_BuffBT4 = 0;

	uint8_t RST_IC_Flag = 0;
	//--------------------BUTTON 1 --------------------//
		if(button1_Stt == Button_Keeping)
		{
			Count_BuffBT1++;

			if(Count_BuffBT1 >= 50000) Count_BuffBT1 = 50000;

			if(Count_BuffBT1 >= (TIME_HOLD_BT * 3))
			{

				RST_IC_Flag = 1;
			}
		}
		else
		{
			Count_BuffBT1=0;
		}

	//--------------------BUTTON 2 --------------------//
		if(button2_Stt == Button_Keeping)
		{
			Count_BuffBT2++;

			if(Count_BuffBT2 >= 50000) Count_BuffBT2 = 50000;

			if(Count_BuffBT2 >= (TIME_HOLD_BT * 3))
			{

				RST_IC_Flag = 1;
			}
		}
		else
		{
			Count_BuffBT2 = 0;
		}
		//--------------------BUTTON 3 --------------------//
			if(button3_Stt == Button_Keeping)
			{
				Count_BuffBT3++;

				if(Count_BuffBT3 >= 50000) Count_BuffBT3 = 50000;

				if(Count_BuffBT3 >= (TIME_HOLD_BT * 3))
				{

					RST_IC_Flag = 1;
				}
			}
			else
			{
				Count_BuffBT3=0;
			}
		//--------------------BUTTON 4 --------------------//
			if(button4_Stt == Button_Keeping)
			{
				Count_BuffBT4++;

				if(Count_BuffBT4 >= 50000) Count_BuffBT4 = 50000;

				if(Count_BuffBT4 >= (TIME_HOLD_BT * 3))
				{

					RST_IC_Flag = 1;
				}
			}
			else
			{
				Count_BuffBT4=0;
			}
/************************ check rst touch cycle **********************************/
			RD_Check_Cycle_RST_Touch_IC();


#if(TYPE_HARDWARE_SWITCH != X1_SOCKET)
		if(RST_IC_Flag == 1)
		{
			Count_BuffBT1 =0;
			Count_BuffBT2 =0;
			Count_BuffBT3 =0;
			Count_BuffBT4 =0;
			uart_CSend("Reset Touch IC \n");
			gpio_write(RESET_TOUCH_PIN, TOUCH_NON_ACTIVE_POW);
			sleep_ms(100);
			gpio_write(RESET_TOUCH_PIN, TOUCH_ACTIVE_POW);
			RD_SwitchAc4Ch_BlinkSet(3, 150);

		}
#endif
}
#endif

static void RD_Check_TouchError(void){
	static uint8_t TouchErorr_Flag[4] = {0, 0, 0, 0};
	Button_Stt_Type buttonStt_buf[4];
	buttonStt_buf[0] = button1_Stt;
	buttonStt_buf[1] = button2_Stt;
	buttonStt_buf[2] = button3_Stt;
	buttonStt_buf[3] = button4_Stt;

	for(int i=0; i<4; i++){
		if((0 == TouchErorr_Flag[i] ) & (Button_Press ==  buttonStt_buf[i])){
			TouchErorr_Flag[i] = 1;
			RD_Flash_SaveRGB_Config(i+1, LED_DIM_ON_DF, LED_DIM_OFF_DF, 0x00, 0x00, 0xFF);
		}
	}
}
void RD_Train_Factory(void)
{
	static uint32_t Time_Train_Detect=0;
	static uint32_t 	Time_Pre=0;

	if((Train_Factory == 1) && (get_provision_state() == STATE_DEV_UNPROV))
	{

		//disable_mesh_adv_filter();
		mesh_adv_prov_link_close();
		#if(SWITCH_LINE == TOUCH_SWITCH)
			RD_Check_TouchError();
		#endif
		if((clock_time_s() - Time_Pre) >= 1)
		{
			Time_Pre = clock_time_s();
			#if(TYPE_HARDWARE_SWITCH == X5_CEILING_SWITCH)
				uint8_t On_Loop = Time_Train_Detect%5;
			#endif
			#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
				uint8_t On_Loop = Time_Train_Detect%4;
			#endif

			#if((TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X3_MACHINE_SWTCH) || (TYPE_HARDWARE_SWITCH == X3_CEILING_SWITCH))
				uint8_t On_Loop = Time_Train_Detect%3;
			#endif

			#if(TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH || (TYPE_HARDWARE_SWITCH == X2_MACHINE_SWTCH) || (TYPE_HARDWARE_SWITCH == X2_CEILING_SWITCH))
				uint8_t On_Loop = Time_Train_Detect%2;
			#endif

			#if((TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH) || (TYPE_HARDWARE_SWITCH == X1_SOCKET) || (TYPE_HARDWARE_SWITCH == X1_MACHINE_SWTCH) )
				uint8_t On_Loop = Time_Train_Detect%1;
			#endif

			Time_Train_Detect++;
			switch (On_Loop)
			{
				case 0:
					RD_SetAndRsp_Switch(0, !relay_Stt[0], 0);
				break;
				case 1:
					RD_SetAndRsp_Switch(1, !relay_Stt[1], 0);
				break;
				case 2:
					RD_SetAndRsp_Switch(2, !relay_Stt[2], 0);
				break;
				case 3:
					RD_SetAndRsp_Switch(3, !relay_Stt[3], 0);
				break;
				case 4:
					RD_SetAndRsp_Switch(4, !relay_Stt[4], 0);
				break;
			}
		}

	}
	if(Train_Factory == 1)
	{
	mesh_adv_prov_link_close();
	}
}

unsigned char modulo_calculate(unsigned short par1,unsigned short par2,unsigned short *out)
{
	if(par1>par2){
		*out = par1 % par2;
		return 1;
	}
	else if(par1<par2){
		*out = par2 % par1;
	return 1;
	}
	else return 0;
}

void RD_Check_Reset_By_Secure(void)
{
	static u8 Provision_state_old = STATE_DEV_PROVED;



	static uint32_t Timing_Reset_Start =0;
	static uint8_t  Check_Flag_Reset =0;
	if(Provision_state_old == STATE_DEV_PROVING)
	{
		uart_CSend("Waiting FPT key Start \n ");
		Check_Flag_Reset=1;
		Timing_Reset_Start = clock_time_s();
	}

	if(Check_Flag_Reset == 1 && Sw_Working_Stt_Val.OTA_Flag == 0x00)
	{
		if( ((clock_time_s() -  Timing_Reset_Start) >= 120)  && (Sw_Flash_Data_Val.Secure_RD != 0x01))
		{
			uart_CSend("Waiting FPT key over time. Reset \n ");
			RD_Switchx4_FactoryReset();
		}

		if( Sw_Flash_Data_Val.Secure_RD == 0x01)
		{
			Check_Flag_Reset =0;
		}
	}

	Provision_state_old = get_provision_state();
}


void RD_SetAndRsp_Switch(int Light_index, u8 OnOff_Set, uint16_t GW_Add_Rsp_G_onoff)
{
	//uint16_t ele_add_buff = 0x0000;
	uart_CSend("  SetAndRsp_Switch\n");
	light_onoff_idx(Light_index,OnOff_Set, 0);
	set_on_power_up_onoff(Light_index, 0, OnOff_Set); // save for POWer_up

	if(GW_Add_Rsp_G_onoff != 0x0000)			// Rsp to Gw if GW_Add_Rsp_G_onoff != 0x0000;
	{
		uart_CSend("send on_off\n");
		//mesh_tx_cmd_g_onoff_st(Light_index, ele_add_buff, GW_Add_Rsp_G_onoff, 0, 0, G_ONOFF_STATUS);
		RD_Send_Relay_Stt(Light_index + 1, OnOff_Set);
	}
#if 0 // RD_Edit cusstom call scene
	if(Light_index == 2) // button 3
	{
		if(OnOff_Set == 0)  RD_Call_Scene(0x0002, 1);
		else 				  RD_Call_Scene(0x0005, 2);
	}
	if(Light_index == 3) // button 4
	{
		if(OnOff_Set == 0)  RD_Call_Scene(0x0004, 3);
		else 				  RD_Call_Scene(0x0003, 4);
	}
#endif
	//FPT_Send_Relay_Stt(Light_index);
}

void RD_SetSwitchToggle(uint8_t Light_index_set)
{
	RD_Call_LinkControl(Light_index_set+1, !relay_Stt[Light_index_set]); //RD_Todo: check
	RD_SetAndRsp_Switch(Light_index_set, !relay_Stt[Light_index_set], 0x0001);
}
/*
 * OnOffSet: 0-1-2   on-off-toggle
 */
void RD_SetSwitch(uint8_t Light_index_set, uint8_t OnOffSet, uint16_t Gw_Add_Rsp)
{
	if(2 == OnOffSet)		OnOffSet = !relay_Stt[Light_index_set]; // toggle

	if(OnOffSet < 2){
		RD_Call_LinkControl(Light_index_set+1, OnOffSet); //RD_Todo: check
		RD_SetAndRsp_Switch(Light_index_set, OnOffSet, Gw_Add_Rsp);
	}
}

void RD_CountDown_Set(uint8_t Light_index, uint32_t Time_Count_s, uint8_t Taget)
{
//	SETMAX(Taget, 2);
//	SETMIN(Taget, 0);

	CountDown_Val[Light_index].Taget = Taget;
	CountDown_Val[Light_index].Time_CountDown_s = Time_Count_s;
	CountDown_Val[Light_index].Time_Start_CountDown_s = clock_time_s();

	char UART_Buff[64];
	sprintf(UART_Buff, " Set count down relay: %d, time: %d s taget: %d\n", Light_index+1, Time_Count_s, Taget);
	uart_CSend(UART_Buff);
}

/**
 * @brief check flag end send relay stt to gateway through G_On_off stt.
 * 
 */
void RD_Check_UpdateStt(void)
{
//	static uint32_t Time_Last_Check=0;
	if(Sw_Update_Stt_Val.Time_Last_Update_ms < clock_time_ms()) Sw_Update_Stt_Val.Time_Last_Update_ms =0;
	if(clock_time_ms() - Sw_Update_Stt_Val.Time_Last_Update_ms >= 500)
	{
		Sw_Update_Stt_Val.Time_Last_Update_ms = clock_time_ms();

		for(int i=0; i<4; i++)
		{
			if(Sw_Update_Stt_Val.Update_Stt_Flag[0] == RD_EN)
			{
				Sw_Update_Stt_Val.Update_Stt_Flag[0] = RD_DIS;
				RD_Update_Relay_Stt(1);
				break;
			}

		}
	}
}

//extern uint32_t rand_in(uint16_t min, uint16_t max);
void RD_Set_UpdateStt(uint8_t Relay_ID)
{
//	SETMIN(Relay_ID, 1);

	Sw_Update_Stt_Val.Time_Last_Update_ms = clock_time_ms() + (ele_adr_primary%50)*100; // set time rsp in many time task
	if(Relay_ID <= 5 )
	{
		Sw_Update_Stt_Val.Update_Stt_Flag[Relay_ID-1] = RD_EN;
	}
	if(Relay_ID == 0xff)
	{
		Sw_Update_Stt_Val.Update_Stt_Flag[0] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[1] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[2] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[3] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[4] = RD_EN;
	}

	char UART_Buff[64];
	sprintf(UART_Buff,"set update stt Relay ID: %d", Relay_ID);
	uart_CSend(UART_Buff);

}


/*--------------------------Remote K9B ---------------------------------*/
/*
 * uint8_t Button_ID 1-->5
 */
void RD_K9B_Pair_OnOffSetFlag(uint8_t Button_ID){
	Sw_Woring_K9B_Val.Button_K9BOnOff_Pair = Button_ID;
	Sw_Woring_K9B_Val.Pair_K9BOnOff_Flag   = 1;
}

/*
 * uint8_t Button_ID 1-->5
 */
void RD_K9B_Pair_OnOffClearFlag(void){
	Sw_Woring_K9B_Val.Button_K9BOnOff_Pair = 0x00;
	Sw_Woring_K9B_Val.Pair_K9BOnOff_Flag   = 0x00;
}

/*
 * return Button_ID 1->5 if Pair_Flag on,
 */
uint8_t RD_K9B_Pair_OnOffGetFlag(void)
{
	return Sw_Woring_K9B_Val.Button_K9BOnOff_Pair;
}

void RD_K9B_ScanPairOnOff(void)
{
	const uint8_t timeOut_K9BPairOnOff_s =10;
	static uint32_t clockTime_K9BPairOnOff_s =0;
/*----------------------------Set------------------------------------------*/
	if(( (Button1_Hold_Flag + Button2_Hold_Flag + Button3_Hold_Flag + Button4_Hold_Flag + Button5_Hold_Flag) == 1) \
			|| (1 == Sw1_PairK9B_OnOff_Flag)) // only 1 buttuon hold
	{
		uint8_t NumOfButton_Pressing =0;
		uint8_t ButtuonId_Hold =0;
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
		//	if(gpio_read(Button_Pin_Array[i]) == LOGIC_ACTIVE_BT)
			#if(LOGIC_ACTIVE_BT == 1)
				if(gpio_read(Button_Pin_Array[i]) != 0)
			#else
				if(gpio_read(Button_Pin_Array[i]) == 0)
			#endif
			{
				NumOfButton_Pressing++; // press
				ButtuonId_Hold = i+1;  // istart 0
			}
		}

		if(NumOfButton_Pressing ==1)
		{
			Sw1_PairK9B_OnOff_Flag =0;
			clockTime_K9BPairOnOff_s = clock_time_s();
			RD_K9B_Pair_OnOffSetFlag(ButtuonId_Hold);

			#if(K9B_RD_SCENE_LOCAL_EN)
				if(Sw_Flash_Data_Val.Type_CtrLocal_K9B == K9B_CTR_LOCAL_SCENE)	RD_Sw_SingleBlinkSet(ButtuonId_Hold, 7, 300);
				else															RD_Sw_SingleBlinkSet(ButtuonId_Hold, 5, 300);
			#else
				RD_Sw_SingleBlinkSet(ButtuonId_Hold, 5, 300);
			#endif

			Button1_Hold_Flag=0;
			Button2_Hold_Flag=0;
			Button3_Hold_Flag=0;
			Button4_Hold_Flag=0;
			Button5_Hold_Flag=0;
		}
	}

/*-------------------------------- clear-----------------------------------*/
	if(clock_time_s() < clockTime_K9BPairOnOff_s) clockTime_K9BPairOnOff_s =0;
	if((0x00 != RD_K9B_Pair_OnOffGetFlag()) &&  (clock_time_s() - clockTime_K9BPairOnOff_s > timeOut_K9BPairOnOff_s))
	{
		uart_CSend("Time out Pair K9B onOff \n");
		RD_Sw_SingleBlinkSet(RD_K9B_Pair_OnOffGetFlag(), 3, 300);
		//RD_Flash_SaveK9BOnOff(0x00000000, 0x00, RD_K9B_Pair_OnOffGetFlag());
		RD_K9B_Pair_OnOffClearFlag();
	}


/*----------------------------------Set Mode for Control local---------------------------------------*/
#if(K9B_RD_SCENE_LOCAL_EN)
	static uint32_t hold_button_clock_ms =0;
	static uint8_t keep_flag =0;
	if(clock_time_ms() < hold_button_clock_ms) hold_button_clock_ms = clock_time_ms();
	if(button1_Stt == Button_Keeping)
	{
		if(keep_flag == 0){
			keep_flag =1;
			hold_button_clock_ms = clock_time_ms();
		}
		if((clock_time_ms()- hold_button_clock_ms >= 10000) && (1 == keep_flag) )
		{
			keep_flag = 0;
			uart_CSend("change K9B local Mode \n");
			if(K9B_CTR_LOCAL_SCENE == Sw_Flash_Data_Val.Type_CtrLocal_K9B) 	RD_Flash_K9BSaveLocalMode(K9B_CTR_LOCAL_ONOFF);
			else															RD_Flash_K9BSaveLocalMode(K9B_CTR_LOCAL_SCENE);
			RD_SwitchAc4Ch_BlinkSet(10, 200);
		}
	}
	else{
		keep_flag =0;
	}
#endif
}

static int RD_K9B_SaveOnOff_yet(uint32_t macDevice, uint8_t key, uint8_t switch_button)
{

	for(int i=0; i< MAX_NUM_K9ONOFF; i++)
	{
		uint32_t mac_buf = Sw_Flash_Data_Val.K9B_Data.OnOff[switch_button - 1].MacK9B[i];
		uint8_t  key_buf = Sw_Flash_Data_Val.K9B_Data.OnOff[switch_button - 1].K9B_BtKey[i];
		if( (mac_buf == macDevice) && (key_buf == key) )
		{
			uart_CSend("This key K9B saved \n");
			return 1;
		}
	}
	return 0; // this key don't save yet
}
void RD_K9B_SaveOnOff(uint32_t macDevice, uint8_t key)
{
	uint8_t keyFillterStt =0;

	if( ((1 == key) || (2 == key) || (4 == key) || (8 == key)|| (16 == key) || (32 == key)|| (40 == key) ) )  // only single button
	{
		keyFillterStt = 1;
	//	key = key + 0x80; // save for action when press button rising
	}
	if(0x00 != RD_K9B_Pair_OnOffGetFlag() && (1 == keyFillterStt))
	{
		if(RD_K9B_SaveOnOff_yet(macDevice, key, RD_K9B_Pair_OnOffGetFlag()) == 0) // this key don't save yet
		{
			RD_Flash_SaveK9BOnOff(macDevice, key, RD_K9B_Pair_OnOffGetFlag());
			RD_K9B_Pair_OnOffClearFlag();
			#if(NUM_OF_ELEMENT == 1)
				Reset_Count_Buff = 0; // clear reset process
			#endif
		}
		else
		{
			RD_K9B_Pair_OnOffClearFlag();
		}
	}
	//Sw_Flash_Data_Val
}

uint8_t RD_K9B_ScanOnOff(uint32_t macDevice, uint8_t key, uint32_t par_signature)
{
	uint8_t scanSttReturn = 0;
	static uint32_t clockTime_toggleButton_ms[MAX_NUM_K9ONOFF] ={0};
	static uint32_t signatureLast = 0;
	static K9bOnOff_countLast_st OnOff_CountLast[NUM_OF_ELEMENT] = {{0}};
#if 0 // mode save 1 button k9b on the 1 element
	for(int i=0; i< NUM_OF_ELEMENT; i++)
	{
		if( (Sw_Flash_Data_Val.K9B_Data.OnOff[i].K9B_BtKey - 0x80 == key) && (Sw_Flash_Data_Val.K9B_Data.OnOff[i].MacK9B == macDevice) )
		{
			if(clock_time_ms() < clockTime_toggleButton_ms[i]) clockTime_toggleButton_ms[i] = clock_time_ms();

			if(i != 2)// test press double. button 3l
			{
				if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS)
				{
					clockTime_toggleButton_ms[i] = clock_time_ms();
					RD_SetSwitchToggle(i);
				}
			}
			else
			{
				clockTime_toggleButton_ms[i] = clock_time_ms();
				RD_SetSwitchToggle(i);
			}
		}
	}
#else
	if((0x00 == par_signature) || (par_signature != signatureLast))
	{
		uart_CSend("Scan K9b onOff  V1:\n");
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			//uint8_t numBtSaved = Sw_Flash_Data_Val.K9B_Data.OnOff[i].NumSaveNext;
			uint8_t numBtSaved = MAX_NUM_K9ONOFF;
			if(clock_time_ms() < clockTime_toggleButton_ms[i]) clockTime_toggleButton_ms[i] = clock_time_ms();
			for(int j=0; j < numBtSaved; j++)
			{
				if( (Sw_Flash_Data_Val.K9B_Data.OnOff[i].K9B_BtKey[j] /*- 0x80*/ & key) && (Sw_Flash_Data_Val.K9B_Data.OnOff[i].MacK9B[j] == macDevice) )
				{
					uart_CSend("key pass \n");
					if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS)
					{
						if(OnOff_CountLast[i].countLastOfMacID[j] != par_signature){
							OnOff_CountLast[i].countLastOfMacID[j] = par_signature;
							clockTime_toggleButton_ms[i] = clock_time_ms();
							RD_SetSwitchToggle(i);
//							#warning "TEST count K9B"
//							count_k9B_press[i]++;
							//RD_MessK9BHc_Press(count_k9B_press[i], i, 0,0);



							//scanSttReturn =1;
							signatureLast = par_signature;
						}
					}
					break; // out off this element of switch
				}
			}
		}
	}

	return scanSttReturn;
#endif
}


static int find_key_position(uint8_t byte) {
    int position = 0;
    while (byte) {
        if (byte & 1) {
            return position;
        }
        byte >>= 1;
        position++;
    }
    return -1; //
}

static uint8_t RD_K9B_Convert_btFun2key(uint8_t key_origin, uint32_t bt_function){
	uint8_t pressUp = 0b10000000 & key_origin; // bit press up down
	uint8_t key_new =0;
	for(int i=0; i<7; i++){
		uint32_t fun_buf = 0b1111 & (bt_function >> (i*4)); // 4 bit / 1 button function
		if(fun_buf != 0x00){
			key_new = key_new | (0x01 << i);
		}
	}
	key_new = key_new | pressUp;
	return key_new;
}

static uint8_t RD_K9B_SetSwitch(uint8_t Light_index_set, uint8_t K9B_Bt_IDSet, uint32_t bt_function){

	uint8_t targetCtl = 0;
	uint32_t fun_buf = 0b1111 & (bt_function >> (K9B_Bt_IDSet*4)); // 4 bit / 1 button function
	switch(fun_buf){
		case NONE_FUN:
			targetCtl = 3; // nothing
			break;
		case ON_FUN:
			targetCtl = 1; // on
			break;
		case OFF_FUN:
			targetCtl = 0; // off
			break;
		case TOGGLE_FUN:
			targetCtl = 2; // toggle (default)
			break;
		default:
			targetCtl = 2; //
	}
	RD_SetSwitch(Light_index_set, targetCtl, GW_ADDRESS);

	return 1;
}
/*
 * Scan on-off new format for RD - use 32 bit counter to set Fun button
 */
uint8_t RD_K9B_ScanOnOffV2(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint32_t bt_function)
{

	uint8_t scanSttReturn = 0;
	static uint32_t clockTime_toggleButton_ms[MAX_NUM_K9ONOFF] ={0};
	static uint32_t signatureLast = 0;
	static K9bOnOff_countLast_st OnOff_CountLast[NUM_OF_ELEMENT] = {{0}};

	char UART_Buff[64];
	if((0x00 == par_signature) || (par_signature != signatureLast))
	{
		uint8_t keyV2 = RD_K9B_Convert_btFun2key(key, bt_function);
		sprintf(UART_Buff,"Scan K9b onOff  V2 fun but: %x--key new 0x%x\n",bt_function, keyV2);
		uart_CSend(UART_Buff);
		//uart_CSend("Scan K9b onOff  V2:\n");
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			uint8_t numBtSaved = MAX_NUM_K9ONOFF;
			if(clock_time_ms() < clockTime_toggleButton_ms[i]) clockTime_toggleButton_ms[i] = clock_time_ms();
			for(int j=0; j < numBtSaved; j++)
			{
				if( (Sw_Flash_Data_Val.K9B_Data.OnOff[i].K9B_BtKey[j] /*- 0x80*/ & keyV2) && (Sw_Flash_Data_Val.K9B_Data.OnOff[i].MacK9B[j] == macDevice) )
				{
					int K9Bbutton = find_key_position(Sw_Flash_Data_Val.K9B_Data.OnOff[i].K9B_BtKey[j]);

					sprintf(UART_Buff,"key pass %d\n", K9Bbutton);
					uart_CSend(UART_Buff);
					if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS)
					{
						if(OnOff_CountLast[i].countLastOfMacID[j] != par_signature){
							OnOff_CountLast[i].countLastOfMacID[j] = par_signature;
							clockTime_toggleButton_ms[i] = clock_time_ms();
							RD_K9B_SetSwitch(i, K9Bbutton, bt_function);
							//RD_SetSwitchToggle(i);
							//scanSttReturn =1;

//							#warning "TEST count K9B"
//							count_k9B_press[i]++;
							//RD_MessK9BHc_Press(count_k9B_press[i], i, 0,0);

							signatureLast = par_signature;
						}
					}
					break; // out off this element of switch
				}
			}
		}
	}

	return scanSttReturn;
}

/*
 * Scan on-off new format for RD - use 32 bit counter to set Fun button
 */
uint8_t RD_K9B_ScanOnOffScene(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint8_t type_remote )
{
	uint8_t scanSttReturn = 0;
	static uint32_t clockTime_toggleButton_ms[MAX_NUM_K9ONOFF] ={0};
	static uint32_t signatureLast = 0;
	static K9bOnOff_countLast_st OnOff_CountLast[NUM_OF_ELEMENT] = {{0}};

	uint8_t onOffScene_flag = 0;
	uint8_t onOffScene_Stt[NUM_OF_ELEMENT] = {0};

	if((0x00 == par_signature) || (par_signature != signatureLast))
	{
		uart_CSend("Scan K9b onOff  Scene:\n");
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			//uint8_t numBtSaved = Sw_Flash_Data_Val.K9B_Data.OnOff[i].NumSaveNext;
			uint8_t numBtSaved = MAX_NUM_K9ONOFF;
			if(clock_time_ms() < clockTime_toggleButton_ms[i]) clockTime_toggleButton_ms[i] = clock_time_ms();
			for(int j=0; j < numBtSaved; j++)
			{
				if( ((Sw_Flash_Data_Val.K9B_Data.OnOff[i].K9B_BtKey[j] /*- 0x80*/ & key) || ((0x01 == key) && (6 == type_remote) )) && (Sw_Flash_Data_Val.K9B_Data.OnOff[i].MacK9B[j] == macDevice) )
				{
					uart_CSend("key pass \n");
					if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS)
					{
						if(OnOff_CountLast[i].countLastOfMacID[j] != par_signature){
							OnOff_CountLast[i].countLastOfMacID[j] = par_signature;
							//clockTime_toggleButton_ms[i] = clock_time_ms();
							//RD_SetSwitchToggle(i);
							//scanSttReturn =1;

							onOffScene_flag = 1; // have Scene
							onOffScene_Stt[i] = 1; // on
							signatureLast = par_signature;
						}
					}
					break; // out off this element of switch
				}
			}
		}

		if(1 == onOffScene_flag) // button saved- have scene
		{
			onOffScene_flag =0;

			for(int bt=0; bt< NUM_OF_ELEMENT; bt++)
			{
				if((0x01 == key)  && (6 == type_remote)) // set off all
				{
					RD_SetAndRsp_Switch(bt, 0x00, 0x0000); // don't rsp onoff model
					onOffScene_Stt[bt] =0;
				}
				else			// set  onOffScene_Stt[
				{
					RD_SetAndRsp_Switch(bt, onOffScene_Stt[bt], 0x0000); // don't rsp onoff model
				}
				clockTime_toggleButton_ms[bt] = clock_time_ms();
			}
			Send_Relay_SttCtrAll_ReUp_Message(GW_ADDRESS, onOffScene_Stt);
		}
	}

	return scanSttReturn;
}


uint8_t RD_K9B_ScanLocal(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint32_t bt_function, uint8_t type_remote)
{
	if(Sw_Flash_Data_Val.Type_CtrLocal_K9B == K9B_CTR_LOCAL_ONOFF)
	{
		return(RD_K9B_ScanOnOffV2(macDevice, key, par_signature, bt_function));
	}
	else
	{
		return(RD_K9B_ScanOnOffScene(macDevice, key, par_signature, type_remote));
	}
}

uint8_t RD_K9B_ScanLocalV1(uint32_t macDevice, uint8_t key, uint32_t par_signature, uint32_t bt_function, uint8_t type_remote)
{
	if(Sw_Flash_Data_Val.Type_CtrLocal_K9B == K9B_CTR_LOCAL_ONOFF)
	{
		return(RD_K9B_ScanOnOff(macDevice, key, par_signature));
	}
	else
	{
		return(RD_K9B_ScanOnOffScene(macDevice, key, par_signature, type_remote));
	}
}

uint8_t RD_K9B_ScanPress2HC(uint32_t macDevice, uint8_t key, uint32_t par_signature)
{
	uint8_t scanSttReturn = 0;
	static uint32_t clockTime_toggleButton_ms[MAX_NUM_K9BHC] ={0};
	static uint32_t signatureLast = 0;

	static uint8_t K9BButton_ID=0;
	uint8_t press_access =0;

	static K9bHc_countLast_st Hc_CountLast = {0};
	if( (key>0) && (key <0x80)) // only rising press.
	{
		K9BButton_ID = key;
		press_access = 1;
	}
	if( ((0x00 == par_signature) || (par_signature != signatureLast)) && (1 == press_access) )
	{
		for(int i=0; i< MAX_NUM_K9BHC; i++)
		{
			if(clock_time_ms() < clockTime_toggleButton_ms[i]) clockTime_toggleButton_ms[i] = clock_time_ms();

			if(macDevice == Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B)
			{
				scanSttReturn = 1;
				if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS) // check time last call
				{
					if(Hc_CountLast.countLastOfMacID[i]  != par_signature){
						Hc_CountLast.countLastOfMacID[i] = par_signature;
						clockTime_toggleButton_ms[i] = clock_time_ms();
						uint16_t K9BHC_Add = Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B;
						uint16_t SceneID		= 0x0000;

						int ButtonPos_Buff = RD_CheckButtonPosK9BHC(Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID, key);
						if(ButtonPos_Buff != -1) SceneID = Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[ButtonPos_Buff];


						RD_MessK9BHc_Press(K9BHC_Add, K9BButton_ID, 1, SceneID);

						if(0x0000 != SceneID) RD_Call_Scene(SceneID, K9BButton_ID);


						signatureLast = par_signature;

						char UART_TempSend1[64] = {0};
						sprintf(UART_TempSend1,"send K9B HC: 0x%08x, button: %d, Scene: 0x%04x",macDevice, K9BButton_ID, SceneID);
						uart_CSend(UART_TempSend1);
					}
				}
			}
		}
	}


	for(int i=0; i< MAX_NUM_K9BHC; i++)
	{
		if(macDevice == Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B)
		{
			scanSttReturn = 1;
		}
	}
	return scanSttReturn;
#if 0
	static uint8_t K9BButton_ID=0;
	uint8_t press_access =0;
	switch (key) {
		case 4:
			K9BButton_ID = 3;
			press_access =1;
			break;
		case 2:
			K9BButton_ID = 2;
			press_access =1;
			break;
		case 1:
			K9BButton_ID = 1;
			press_access =1;
			break;
		default:
			break;
	}

	//if( (1 == K9BButton_ID) || (2 == K9BButton_ID) || (3 == K9BButton_ID))
	if(1 == press_access)
	{
		if((macDevice != Sw_Press_K9BHC_Val.MacOld) || (Sw_Press_K9BHC_Val.Button_ID != K9BButton_ID))
		{
			Sw_Press_K9BHC_Val.ClockTimePress_ms = clock_time_ms();
			Sw_Press_K9BHC_Val.Button_ID =0;
			Sw_Press_K9BHC_Val.ButtonPressCounter =0;
		}
		uart_CSend("check press 1764");
		if(Sw_Press_K9BHC_Val.ButtonPressCounter <2) Sw_Press_K9BHC_Val.ButtonPressCounter++; // max is double
		Sw_Press_K9BHC_Val.Button_ID = K9BButton_ID;
		Sw_Press_K9BHC_Val.ClockTimePress_ms = clock_time_ms();
		Sw_Press_K9BHC_Val.MacOld = macDevice;
	}
#endif
#if 0 // old
	uint8_t K9BButton_ID =0;
	if( macDevice == Sw_Flash_Data_Val.K9B_Data.Scene[0].MacK9B)  //RD_Todo
	{
		switch (key) {
			case 4:
				K9BButton_ID = 1;
				break;
			case 2:
				K9BButton_ID = 2;
				break;
			case 1:
				K9BButton_ID = 3;
				break;
			default:
				break;
		}

		if(0x00 == Sw_Woring_K9B_Val.Bt_K9B_CountPress[K9BButton_ID-1] )// start press
		{
			Sw_Woring_K9B_Val.Clock_BtK9BStartPress_ms[K9BButton_ID-1] = clock_time_ms();
			Sw_Woring_K9B_Val.Bt_K9B_CountPress[K9BButton_ID-1] = 1; // one press
		}
		else
		{
			Sw_Woring_K9B_Val.Bt_K9B_CountPress[K9BButton_ID-1] = 2; // double press
		}
	}
#endif
}

void RD_K9B_CheckDeleteOnOff(uint8_t Button_ID)
{
	if(RD_K9B_Pair_OnOffGetFlag() == Button_ID)
	{
		RD_Flash_DeleteAllK9BOnOff(Button_ID);
	}
}
static void RD_LoopK9BHC_Press(void)
{
	if(clock_time_ms() < Sw_Press_K9BHC_Val.ClockTimePress_ms) Sw_Press_K9BHC_Val.ClockTimePress_ms = clock_time_ms(); // resset over time

	if( (Sw_Press_K9BHC_Val.MacOld != 0x00) && (Sw_Press_K9BHC_Val.ButtonPressCounter != 0) && (clock_time_ms() - Sw_Press_K9BHC_Val.ClockTimePress_ms > CLOCK_TIME_OUT_PRESS_K9B_MS))
	{
		for(int i=0; i< MAX_NUM_K9BHC; i++)
		{

			if(Sw_Press_K9BHC_Val.MacOld == Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B)
			{
				uint16_t K9BHC_Add = Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B;
				uint8_t  K9B_ButtonID = Sw_Press_K9BHC_Val.Button_ID;
				uint8_t  K9B_PressCount = Sw_Press_K9BHC_Val.ButtonPressCounter;
				uint16_t SceneID		= 0x0000;
				if(1 == K9B_PressCount) SceneID = Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[K9B_ButtonID-1];
				if(2 == K9B_PressCount) SceneID = Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[K9B_ButtonID-1];

				RD_MessK9BHc_Press(K9BHC_Add, K9B_ButtonID, K9B_PressCount, SceneID);

				if(0x0000 != SceneID) RD_Call_Scene(SceneID, K9B_ButtonID*5+K9B_PressCount);
				Sw_Press_K9BHC_Val.ButtonPressCounter =0;
				Sw_Press_K9BHC_Val.Button_ID =0;
				break;
			}
		}
	}
}
void RD_K9B_CheckHandlePressHc(void)
{
	static uint32_t Timeout_double_Press = 1500;
	for(int i=0; i<3; i++)
	{
		if(clock_time_ms() < Sw_Woring_K9B_Val.Clock_BtK9BStartPress_ms[i]) Sw_Woring_K9B_Val.Clock_BtK9BStartPress_ms[i] = 0;


		if( (2 == Sw_Woring_K9B_Val.Bt_K9B_CountPress[i])  ||\
			( (1 == Sw_Woring_K9B_Val.Bt_K9B_CountPress[i]) && (clock_time_ms() - Sw_Woring_K9B_Val.Clock_BtK9BStartPress_ms[i] >= Timeout_double_Press))	)
		{
			Sw_Woring_K9B_Val.Clock_BtK9BStartPress_ms[i] = clock_time_ms();
			Sw_Woring_K9B_Val.Bt_K9B_CountPress[i]  = 0; //reset

			// RD_Todo handle press
		}
	}
}
void RD_K9B_PairHCSet(uint8_t pairStt, uint16_t K9BAdd)
{
	Sw_Woring_K9B_Val.Add_K9B_HCSet = K9BAdd;
	Sw_Woring_K9B_Val.Pair_K9BHc_Flag = pairStt;
	Sw_Woring_K9B_Val.ClockTimeSetHc_ms = clock_time_ms();
}

void RD_K9B_ScanPairHC(void)
{
	const uint32_t TimeOutK9BPiartHc_ms = 10000;

	if(clock_time_ms() < Sw_Woring_K9B_Val.ClockTimeSetHc_ms ) Sw_Woring_K9B_Val.ClockTimeSetHc_ms =0; // resset count

	// pair time out
	if( (0x01 == Sw_Woring_K9B_Val.Pair_K9BHc_Flag) && ( clock_time_ms() - Sw_Woring_K9B_Val.ClockTimeSetHc_ms > TimeOutK9BPiartHc_ms))
	{
		RD_K9B_PairHCSet(0x00, 0x0000);
		RD_SwitchAc4Ch_BlinkSet(3, 500);
	}
}

void RD_K9B_TimeOutScanK9BHC(void)
{
	const uint32_t TimeOutK9BPiartHc_ms = 10000;

	if(clock_time_ms() < Sw_Woring_K9B_Val.ClockTimeSetHc_ms ) Sw_Woring_K9B_Val.ClockTimeSetHc_ms =0; // resset count

	// pair time out
	if( (0x01 == Sw_Woring_K9B_Val.Pair_K9BHc_Flag) && ( clock_time_ms() - Sw_Woring_K9B_Val.ClockTimeSetHc_ms > TimeOutK9BPiartHc_ms))
	{
		uart_CSend("time out ScanK9BHC\n");
		RD_Mess_ScanK9BHC_Rsp(RD_GATEWAYADDRESS, 0x00, 0x00, 0x00 );
		RD_K9B_PairHCSet(0x00, 0x0000); //clear phase working
		RD_SwitchAc4Ch_BlinkSet(3, 500);
	}
}

void RD_K9B_CheckSaveMacHc(uint32_t K9BMac_Buff, uint8_t Num_Of_Button)
{
	if(0x01 == Sw_Woring_K9B_Val.Pair_K9BHc_Flag)
	{
		uint8_t saveStt = RD_Flash_SaveK9BHc(K9BMac_Buff, Num_Of_Button, Sw_Woring_K9B_Val.Add_K9B_HCSet);
	//	RD_Mess_K9BPairHC_Rsp(RD_GATEWAYADDRESS, 0x01, Sw_Woring_K9B_Val.Add_K9B_HCSet, Num_Of_Button, saveStt);

		RD_K9B_PairHCSet(0x00, 0x0000); //clear phase working
		RD_SwitchAc4Ch_BlinkSet(3, 500);
	}
}

void RD_K9B_CheckScanK9BHc(uint32_t K9BMac_Buff, uint8_t Num_Of_Button, s8 rssi)
{
	if(0x01 == Sw_Woring_K9B_Val.Pair_K9BHc_Flag)
	{
	//	uint8_t saveStt = RD_Flash_SaveK9BHc(K9BMac_Buff, Num_Of_Button, Sw_Woring_K9B_Val.Add_K9B_HCSet);
		char UART_TempSend1[128] = {0};
		sprintf(UART_TempSend1,"send HC mac: %08x, Type: %d, rssi: %d--%x--%x--%x Rssi: %d",K9BMac_Buff, Num_Of_Button, rssi);
		uart_CSend(UART_TempSend1);

		RD_Mess_ScanK9BHC_Rsp(RD_GATEWAYADDRESS, K9BMac_Buff, Num_Of_Button, rssi );
		RD_K9B_PairHCSet(0x00, 0x0000); //clear phase working
		RD_SwitchAc4Ch_BlinkSet(3, 500);
	}
}

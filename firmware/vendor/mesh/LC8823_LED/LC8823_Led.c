/*
 * LC8823_Led.c
 *
 *  Created on: Jun 11, 2022
 *      Author: Hao Dv
 */
#include "LC8823_Led.h"
static uint8_t LC8823_SLEEP         = 1000000/LC8823_FREQ/2;   // time 1/2 delay CLK



LED_Switch_Str LED_Switch_Data;
/*
 * Send 1 bit data
 */
static void LC8823_Send_bit(uint8_t Bit_Value)
{
#if 0
    LC8823_CLK_L
    sleep_us(LC8823_SLEEP);

    if(Bit_Value)   LC8823_DATA_H
    else            LC8823_DATA_L      

    LC8823_CLK_H                                  // make rising and confirm Data send
    sleep_us(LC8823_SLEEP);
    LC8823_CLK_L                                  // pull up to for Next T

#endif

#if 1
    LC8823_CLK_L
    sleep_us(LC8823_SLEEP/2);
    if(Bit_Value)   LC8823_DATA_H
    else            LC8823_DATA_L
    sleep_us(LC8823_SLEEP/2);
    LC8823_CLK_H
    sleep_us(LC8823_SLEEP);
    LC8823_CLK_L
#endif
}

/*
 send  1 byte MSB to LSBLC8823
*/
static void LC8823_Send_Byte(uint8_t byte_Value)
{
    uint8_t Bit_Send =0;
    for(int i =7; i>0; i--)
    {
        Bit_Send = (byte_Value >> i) & 0x01;
        LC8823_Send_bit(Bit_Send);
    }
    LC8823_CLK_H			// pull up after transfer
    LC8823_DATA_H			// pull up after transfer
     
}

void LC8823_Send_StartFrame(void)
{
	// 32 bit 0xFF
    LC8823_Send_Byte(START_FRAME);
    LC8823_Send_Byte(START_FRAME);
    LC8823_Send_Byte(START_FRAME);
    LC8823_Send_Byte(START_FRAME);
}

void LC8823_Send_StopFrame(void)
{
	// 32 bit 0x00
    LC8823_Send_Byte(STOP_FRAME);
    LC8823_Send_Byte(STOP_FRAME);
    LC8823_Send_Byte(STOP_FRAME);
    LC8823_Send_Byte(STOP_FRAME);

    LC8823_CLK_H
    LC8823_DATA_H
}

/*
    Send Data 1 Led
    Brightness:  5 bit LSB  Brightness off led (all R-G-B)  Value Range 0-->32;
    Blue_Gray:   Gray level of Blue Value Range 0-->256;
    Green_Gray:  Gray level of Green Value Range 0-->256;
    Red_Gray:   Gray level of Red Value Range 0-->256;

    Data frame  Brightness->Blue_Gray->Green_Gray->Red_Gray    (total 32 bit)
    send next data to next led
*/
void LC8823_Send_LED_Data(uint8_t Brightness, uint8_t Blue_Gray, uint8_t Green_Gray, uint8_t Red_Gray)
{
	if(Brightness >= 31)		Brightness =31;   // Limit Brightness 31

    uint8_t Brightness_Byte = 0xE0 | Brightness;
    LC8823_Send_Byte(Brightness_Byte);
    LC8823_Send_Byte(Blue_Gray);
    LC8823_Send_Byte(Green_Gray);
    LC8823_Send_Byte(Red_Gray);
}

/*
Set LED value   
    Led_Num: LED number (1-2-3-4)
    Brightness: Dim led (0-31)
    X-Gray: Gray off R-G-B
*/
void LED_Set_Stt(uint8_t Led_Num, uint8_t Brightness, uint8_t Blue_Gray, uint8_t Green_Gray, uint8_t Red_Gray)
{
    LED_Switch_Data.Control = 1;
    //if(Brightness >31)      Brightness = 31;        // limit Brightness 0-31
    LED_Switch_Data.LED[Led_Num -1].Lum        = Brightness;
    LED_Switch_Data.LED[Led_Num -1].Blue_Gray   = Blue_Gray;
    LED_Switch_Data.LED[Led_Num -1].Green_Gray  = Green_Gray;
    LED_Switch_Data.LED[Led_Num -1].Red_Gray    = Red_Gray;
}

/*
    Set only LED DIM value 
    Led_Num: LED number (1-2-3-4)
    OnOff_Stt: Stt same with relay stt 
*/
void LED_Set_OnOff_Stt(uint8_t Led_Num, uint8_t OnOff_Stt)
{
//	#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
//		RD_SwitchAc4Ch_BlinkSet(1, 10); // Resent control led after control relay on Switch x1. flyback pow
//	#endif
    uint8_t Brightness_Val = 31;
    if(OnOff_Stt == 0)
	{
            Brightness_Val = Sw_Flash_Data_Val.Data_Led[Led_Num -1].Lightness_Off;
    	
	}
    else    Brightness_Val = Sw_Flash_Data_Val.Data_Led[Led_Num -1].Lightness_On;
    // set brightness and keep other value
    LED_Set_Stt(Led_Num, Brightness_Val, LED_Switch_Data.LED[Led_Num -1].Blue_Gray, LED_Switch_Data.LED[Led_Num -1].Green_Gray, LED_Switch_Data.LED[Led_Num -1].Red_Gray);
}

void LED_Update_Stt()
{
// control led RGB ( RGB use in touch switch)
#if(TYPE_LED == RGB_LED)
		LC8823_Send_StartFrame();
#if(PCB_LED_VER == PCB_LED_1) // PCB verson 1
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED2_DATA_ID].Lum, LED_Switch_Data.LED[LED2_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED2_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED2_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED3_DATA_ID].Lum, LED_Switch_Data.LED[LED3_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED3_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED3_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED4_DATA_ID].Lum, LED_Switch_Data.LED[LED4_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED4_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED4_DATA_ID].Red_Gray);
#endif
#if(PCB_LED_VER == PCB_LED_2) // PCB verson 2
    // button 1
	#if(TYPE_HARDWARE_SWITCH != X1_TOUCH_SWITCH)        
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
	
    #else       // switch x1 use 4 led with 1 button
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
sleep_ms(3);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED1_DATA_ID].Lum, LED_Switch_Data.LED[LED1_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED1_DATA_ID].Red_Gray);

		#endif

    // button 2
	#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X2_TOUCH_SWITCH )
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED2_DATA_ID].Lum, LED_Switch_Data.LED[LED2_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED2_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED2_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED2_DATA_ID].Lum, LED_Switch_Data.LED[LED2_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED2_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED2_DATA_ID].Red_Gray);
	#endif

    // button 3
	#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH || TYPE_HARDWARE_SWITCH == X3_TOUCH_SWITCH )
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED3_DATA_ID].Lum, LED_Switch_Data.LED[LED3_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED3_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED3_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED3_DATA_ID].Lum, LED_Switch_Data.LED[LED3_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED3_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED3_DATA_ID].Red_Gray);
	#endif

    // button 4
	#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED4_DATA_ID].Lum, LED_Switch_Data.LED[LED4_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED4_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED4_DATA_ID].Red_Gray);
		LC8823_Send_LED_Data(LED_Switch_Data.LED[LED4_DATA_ID].Lum, LED_Switch_Data.LED[LED4_DATA_ID].Blue_Gray, LED_Switch_Data.LED[LED4_DATA_ID].Green_Gray, LED_Switch_Data.LED[LED4_DATA_ID].Red_Gray);
	#endif

#endif
		LC8823_Send_StopFrame();
#endif  // end of define type led RGB

// control led Red-Green   use for socket
#if(TYPE_LED == RED_GREEN_LED)
        // button 1
	#if(LIGHT_CNT >= 1)        // use for device has element >= 1
        if(LED_Switch_Data.LED[LED1_DATA_ID].Lum == LED_DIM_OFF_DF)
        {
            gpio_write(RED_LED, LED_SOCKET_OFF);
            gpio_write(GREEN_LED, LED_SOCKET_ON);
        }
        else
        {
            gpio_write(RED_LED, LED_SOCKET_ON);
            gpio_write(GREEN_LED, LED_SOCKET_OFF);
        }
    #else       // switch x1 use 4 led with 1 button

	#endif
#endif

// control led Dim   use for machine switch
#if(TYPE_LED == PWM_LED) //RD_Todo
		// button 1
        	pwm_set_cycle_and_duty(LED1_PWM, 100, 100 - LED_Switch_Data.LED[LED1_DATA_ID].Lum);
		#if(NUM_OF_ELEMENT >= 2)
        	pwm_set_cycle_and_duty(LED2_PWM, 100, 100 - LED_Switch_Data.LED[LED2_DATA_ID].Lum);
		#endif
		#if(NUM_OF_ELEMENT >= 3)
        	pwm_set_cycle_and_duty(LED3_PWM, 100, 100 - LED_Switch_Data.LED[LED3_DATA_ID].Lum);
		#endif
		#if(NUM_OF_ELEMENT >= 4)
        	pwm_set_cycle_and_duty(LED4_PWM, 100, 100 - LED_Switch_Data.LED[LED4_DATA_ID].Lum);
		#endif
		#if(NUM_OF_ELEMENT >= 5)
			pwm_set_cycle_and_duty(LED5_PWM, 100, 100 - LED_Switch_Data.LED[LED5_DATA_ID].Lum);
		#endif
#endif
}

void LED_Init_Value(void)
{
	for(int i=0; i<5; i++)
	{
		LED_Set_Stt(i+1, Sw_Flash_Data_Val.Data_Led[i].Lightness_Off, Sw_Flash_Data_Val.Data_Led[i].Blue_Gray, Sw_Flash_Data_Val.Data_Led[i].Green_Gray, Sw_Flash_Data_Val.Data_Led[i].Red_Gray);
	}
	LED_Update_Stt();
}



static float HueToRGB(float v1, float v2, float vH)
{
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}

struct RGB HSLToRGB(struct HSL hsl) {
	struct RGB rgb;

	if (hsl.S == 0)
	{
		rgb.R = rgb.G = rgb.B = (unsigned char)(hsl.L * 255);
	}
	else
	{
		float v1, v2;
		float hue = (float)hsl.H / 360;

		v2 = (hsl.L < 0.5) ? (hsl.L * (1 + hsl.S)) : ((hsl.L + hsl.S) - (hsl.L * hsl.S));
		v1 = 2 * hsl.L - v2;

		rgb.R = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
		rgb.G = (unsigned char)(255 * HueToRGB(v1, v2, hue));
		rgb.B = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
	}
	char UART_TempSend[64];
	sprintf(UART_TempSend,"HSL to RGB -> R:%d %G:%d B:%d : \n",rgb.R,rgb.G, rgb.B);
	uart_CSend(UART_TempSend);
	return rgb;
}

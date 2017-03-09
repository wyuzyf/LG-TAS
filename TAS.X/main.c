/* 
 * File:   main.c
 * Author: Zhao
 *
 * Created on 2017年2月20日, 上午10:58
 */

#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include <p33Exxxx.h>
#include <math.h>
#include "ECAN.h"
#include "PWM.h"
#include "ADC.h"



 //#define SAMP_BUFF_SIZE  8   // 模拟输入的缓冲区大小

/*设置成全局变量，因为要用到中断处理函数中的值*/
extern unsigned int x[2],Tx[10],Px[10];    //Tx[9:0]存10次周期值，Px[9:0]存10次脉宽值
extern char duty_cyle_1, duty_cyle_2;
extern float Absolute_angle;
extern int Delta;

//extern int pin0Buff[SAMP_BUFF_SIZE];
//extern int pin1Buff[SAMP_BUFF_SIZE];

extern float AD_Value,AD_Value_1;

extern int ADCValues[2] ;

typedef unsigned long Uword32; /*32bits*/

EcanData ECANTxMsg;


void InitClock(void);
void Delay_ms(Uword32);

//void WDog_Enable(void);

void ECAN_Send_duty(void);       //发送占空比
void ECAN_Send_Voltage(void);   //用ECAN发送电压值
void ECAN_Send_angle(void);     //用ECAN发送利用占空比算出的Absolute angle

void Timer1_Init(void);
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void);





_FOSCSEL(FNOSC_FRC & PWMLOCK_OFF);
_FOSC(FCKSM_CSECMD & POSCMD_XT & OSCIOFNC_OFF & IOL1WAY_OFF);
_FWDT(WDTPOST_PS1024 & WDTPRE_PR32 & PLLKEN_ON & WINDIS_OFF & FWDTEN_ON);
//_FWDT(FWDTEN_OFF);
_FPOR(WDTWIN_WIN75 & ALTI2C1_OFF); //I2C1 use PIN SDA1 and SCL1
//Jtag disable and use PGEC2,PGED2 as debug PIN
_FICD(ICS_PGD2 & JTAGEN_OFF);




int main(void)
{

    InitClock();



     Input_capture_Init_SAS();

     Timer1_Init();
      //  Init_ADC1();

   
   // Init_Time5();

    
    //Define this function in ECAN.h
     InitECAN();
    /* Enable ECAN1 Interrupt */
    IEC2bits.C1IE = 1;
    /* enable Transmit interrupt */
    C1INTEbits.TBIE = 0;

    /* Enable Receive interrupt */
    C1INTEbits.RBIE = 1;
   
    // Delay_ms(3000);

   while(1)
   {
        ClrWdt();


        //if(AD_Value !=0  && AD_Value_1 !=0 )
        //{
            //Calculation_Voltage();
        //}

        
      // if( (duty_cyle_1 !=0) && (duty_cyle_2 !=0))
     // {
       //    Calculation_Delta();
       //    ECAN_Send_duty();
               
      // }
      

        
     
    }


    //return (EXIT_SUCCESS);
}






void InitClock(void)
{
    // Configure the device PLL to obtain 60 MIPS operation.The crystal frequency is 10 MHz.
	// Divide 10 MHz by 2, multiply by 60 and divide by 2. This results in Fosc of 120 MHz.
	// The CPU clock frequency is Fcy = Fosc/2 = 60 MHz.
	PLLFBD = 46; /* M = 48 */
	CLKDIVbits.PLLPOST = 0; /* N1 = 2 */
	CLKDIVbits.PLLPRE = 0; /* N2 = 2 */
	OSCTUN = 0;
	/* Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0x3) */
	__builtin_write_OSCCONH(0x03);
	__builtin_write_OSCCONL(0x01);
	// Wait for Clock switch to occur
	while(OSCCONbits.COSC != 0b011);
	// Wait for PLL to lock
	while(OSCCONbits.LOCK != 1)
	{
	};
}


void Delay_ms(Uword32 ms)
{
    Uword32 i;
    Uword32 j;

    for(j=0;j<ms;ms--)
    {
        ClrWdt();
        for(i=0;i<5000;i++)  
        {
            ClrWdt();
            Nop();
        }
    }
}

void Timer1_Init()
{
    T1CONbits.TON = 0; // Disable Timer
    T1CONbits.TCS = 0; // Select internal instruction cycle clock
    T1CONbits.TGATE = 0; // Disable Gated Timer mode
    T1CONbits.TCKPS = 0b011; // Select 1:8 Prescaler

    //IPC0bits.T1IP = 7;

    TMR1 = 0x00; // Clear timer register
    PR1 = 2999; // Load the period value
    IPC0bits.T1IP = 0x01; // Set Timer 1 Interrupt Priority Level
    IFS0bits.T1IF = 0; // Clear Timer 1 Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer1 interrupt
    T1CONbits.TON = 1; // Start Timer
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    /* Interrupt Service Routine code goes here */
    IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag

    Calculation_Delta();
    ECAN_Send_duty();


}

void ECAN_Send_duty(void)
{
    ECANTxMsg.message_type = CAN_MSG_DATA;
    ECANTxMsg.frame_type = CAN_FRAME_STD;
    ECANTxMsg.buffer = 0;
    ECANTxMsg.id = 0x20C;

   // ECANTxMsg.data[0] = duty_cyle_1;
   // ECANTxMsg.data[1] = duty_cyle_2;

    ECANTxMsg.data[0] = (char)((int)Absolute_angle /100);
    ECANTxMsg.data[1] = (char)((int)Absolute_angle %100);

    ECANTxMsg.data_length = 4;
    ECANSendData(&ECANTxMsg);
}






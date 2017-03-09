
#include <xc.h>
#include <p33Exxxx.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdlib.h>
#include "ADC.h"


    //每个通道的缓冲区声明
//int pin0Buff[SAMP_BUFF_SIZE];
//int pin1Buff[SAMP_BUFF_SIZE];

//int scanCounter = 0;     //
//int sampleCounter = 0;

float AD_Value,AD_Value_1;

int V,V_1;

int i;
int ADCValues[2] ;



//使用CH0扫描2个模拟输入
void Init_ADC1(void)
{
    //Set port configuration，
    ANSELB = 5;      //AN0，AN1
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB2 = 1;

    //Inittialize ADC module

    /*
    AD1CON1bits.ADON = 0;  // 由软件写入0来清零DONE状态位（不允许软件写1）
    AD1CON1bits.SAMP = 0;
    AD1CON1bits.ASAM = 1;    // 采样控制，转换结束后立即开始采样
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SSRC = 7;    // 由内部计数器结束采样并转换
    AD1CON1bits.FORM = 0;    // 数据输出模式，整数
    AD1CON1bits.AD12B = 1;   // 12-bit 4通道 ADC 工作模式
    */
    AD1CON1 = 0x4E4; // Enable 12-bit mode, auto-sample and auto-conversion


    /*
    AD1CON2bits.CSCNA = 1;   // 采样多路开关A选择的CH0+输入
    AD1CON2bits.CHPS = 0;    // 转换CH0
    AD1CON2bits.SMPI = 3;   //3次采样后产生中断
    */
    AD1CON2 = 0x40C;


    /*
    AD1CON3bits.ADRC = 0;    // ADC Clock = Systems Clock
    AD1CON3bits.ADCS = 19;   // ADC 转换 Clock Tad=Tcy*(ADCS+1)= (1/10M)*20 = 2us (500Khz)
    AD1CON3bits.SAMC = 0;   //自动采样时间位
    */
    AD1CON3 = 0xF13;

    /*
    AD1CON4bits.ADDMAEN = 0;            // 转换结果存储在 ADC1BUF0~ ADC1BUFF register
    */
    AD1CON4 = 0;


    //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    AD1CSSH = 0x0000;
    AD1CSSLbits.CSS2 = 1;            // Enable AN0 for channel scan
    AD1CSSLbits.CSS1 = 1;            // Enable AN1 for channel scan

    /* Assign MUXA inputs */
    AD1CHS0bits.CH0SA = 0; // CH0SA bits ignored for CH0 +ve input selection
    AD1CHS0bits.CH0NA = 0; // Select VREF- for CH0 -ve input

    
    /* Enable ADC module and provide ADC stabilization delay */
    AD1CON1bits.ADON = 1;
    Delay_us(20);

    // AD1CON2bits.BUFM = 1;   //将缓冲区分拆
   // AD1CON2bits.BUFS = 1;

    
    IFS0bits.AD1IF = 0;     // 清除 A/D中断标志位
    IEC0bits.AD1IE = 1;     // 使能 A/D 中断
  


}

void Delay_us(unsigned int delay)
{
for (i = 0; i < delay; i++)
{
__asm__ volatile ("repeat #39");
__asm__ volatile ("nop");
}
}



/*
void Init_Time5(void)
{
    TMR5 = 0x0000;
    PR5 = 4999;         // Trigger ADC1 every 125usec
    IFS1bits.T5IF = 0;  //清 Timer 5 中断
    IEC1bits.T5IE = 0;  // 使能 Timer 5 中断

    //Start Timer 3
    T5CONbits.TON = 1;
}
*/


void __attribute__ ( (interrupt, no_auto_psv) ) _AD1Interrupt( void )
{

    IFS0bits.AD1IF = 0; // Clear the ADC1 Interrupt Flag

   // Delay_us(1000);

    ADCValues[0] = ADC1BUF0; // Read the AN2 conversion result

     //Delay_us(1000);

    ADCValues[1] = ADC1BUF1; // Read the AN1 conversion result

    V = *(&ADCValues[0]);
    AD_Value = ( (  V * 1.0) / MAX)  * VDD;

    V_1 = *(&ADCValues[1]);
    AD_Value_1 = ( (  V_1 * 1.0) / MAX ) * VDD ;

    /*
    switch( scanCounter )
    {
        case 0:
            pin0Buff[sampleCounter] = ADC1BUF0;
            break;

        case 1:
            pin1Buff[sampleCounter] = ADC1BUF0;
            break;

        default:
            break;
    }

    scanCounter++;
    if( scanCounter == NUM_CHS2SCAN )
    {
        scanCounter = 0;
        sampleCounter++;
    }

    if( sampleCounter == SAMP_BUFF_SIZE )
    {
        sampleCounter = 0;
    }

    //TglPin();           // Toggle RA6
    
      */
}

/*
void Calculation_Voltage(void)
{
    
    V = *(&pin0Buff[sampleCounter]);
    V_1 = *(&pin1Buff[sampleCounter]);

    AD_Value = (int)(V / 1024) * VDD;
    AD_Value_1 = (int)(V_1 / 1024) * VDD;

    //return AD_Value;
}
*/


/*有俩路电压值，采用双通道同时采样的模式*/
//void ADC1_Sample(void)
//{

    /* 1 选择采样的模拟输入引脚*/
    //ANSELA = 0x0000;
   // TRISAbits.TRISA0 = 1;

   // ANSELAbits.ANSA0 = 1;   //T1  主力矩,CH0
   // ANSELAbits.ANSA1 = 1;   //T2  副力矩,CH1

    /* 2 初始化和使能ADC*/

    //(1)选择ADC模式 bit10
 //   AD1CON1bits.AD12B = 0x01;  //10位4通道模式

    //(2)选择参考电压源一匹配模拟输入的范围
 //   AD1CON2bits.VCFG2 = 1;    //AVDD,AVSS(0-3.3v)

    //(3)选择模拟转换时钟
//    AD1CON3bits.ADCS = 0x00;  //最快的转换速率

    //(4)分配S/H通道
  //  AD1CHS0bits.CH0SA = 0;   //选择AN0为CH0的模拟输入
 //   AD1CHS0bits.CH0NA = 0;

  //  AD1CHS123bits.CH123SA = 0;
  //  AD1CHS123bits.CH123NA = 0;

    //(5)使用多少个通道
  //  AD1CON2bits.CHPS = 0x01;   //转换CH0和CH1

    //(6)确定采样如何发生
  //  AD1CON1bits.SIMSAM = 1;   //同时采样
  //  AD1CSSHbits = 0x0001;
  //  AD1CSSLbits = 0x0001;

    //(7)选择采样方式,自动采样????
  //  AD1CON1bits.ASAM = 1;
  //  AD1CON1bits.SAMP = 0;

    //(8)选择转换触发和采样时间???????
  //  AD1CON3bits.SAMC = 31;

    //(9)转换结果在控制器中的数据格式
  //  AD1CON1bits.FORM = 0x00;    //整数

    //(10)中断速率
   // AD1CON2bits.SMPI = 0x01111;   //每完成16次采样产生一次中断

    //(11)转换结果存储
   // AD1CON4bits.ADDMAEN = 0;    //结果存储在BUF0~BUFF中

//}



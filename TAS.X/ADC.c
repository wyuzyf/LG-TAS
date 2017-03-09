
#include <xc.h>
#include <p33Exxxx.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdlib.h>
#include "ADC.h"


    //ÿ��ͨ���Ļ���������
//int pin0Buff[SAMP_BUFF_SIZE];
//int pin1Buff[SAMP_BUFF_SIZE];

//int scanCounter = 0;     //
//int sampleCounter = 0;

float AD_Value,AD_Value_1;

int V,V_1;

int i;
int ADCValues[2] ;



//ʹ��CH0ɨ��2��ģ������
void Init_ADC1(void)
{
    //Set port configuration��
    ANSELB = 5;      //AN0��AN1
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB2 = 1;

    //Inittialize ADC module

    /*
    AD1CON1bits.ADON = 0;  // �����д��0������DONE״̬λ�����������д1��
    AD1CON1bits.SAMP = 0;
    AD1CON1bits.ASAM = 1;    // �������ƣ�ת��������������ʼ����
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SSRC = 7;    // ���ڲ�����������������ת��
    AD1CON1bits.FORM = 0;    // �������ģʽ������
    AD1CON1bits.AD12B = 1;   // 12-bit 4ͨ�� ADC ����ģʽ
    */
    AD1CON1 = 0x4E4; // Enable 12-bit mode, auto-sample and auto-conversion


    /*
    AD1CON2bits.CSCNA = 1;   // ������·����Aѡ���CH0+����
    AD1CON2bits.CHPS = 0;    // ת��CH0
    AD1CON2bits.SMPI = 3;   //3�β���������ж�
    */
    AD1CON2 = 0x40C;


    /*
    AD1CON3bits.ADRC = 0;    // ADC Clock = Systems Clock
    AD1CON3bits.ADCS = 19;   // ADC ת�� Clock Tad=Tcy*(ADCS+1)= (1/10M)*20 = 2us (500Khz)
    AD1CON3bits.SAMC = 0;   //�Զ�����ʱ��λ
    */
    AD1CON3 = 0xF13;

    /*
    AD1CON4bits.ADDMAEN = 0;            // ת������洢�� ADC1BUF0~ ADC1BUFF register
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

    // AD1CON2bits.BUFM = 1;   //���������ֲ�
   // AD1CON2bits.BUFS = 1;

    
    IFS0bits.AD1IF = 0;     // ��� A/D�жϱ�־λ
    IEC0bits.AD1IE = 1;     // ʹ�� A/D �ж�
  


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
    IFS1bits.T5IF = 0;  //�� Timer 5 �ж�
    IEC1bits.T5IE = 0;  // ʹ�� Timer 5 �ж�

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


/*����·��ѹֵ������˫ͨ��ͬʱ������ģʽ*/
//void ADC1_Sample(void)
//{

    /* 1 ѡ�������ģ����������*/
    //ANSELA = 0x0000;
   // TRISAbits.TRISA0 = 1;

   // ANSELAbits.ANSA0 = 1;   //T1  ������,CH0
   // ANSELAbits.ANSA1 = 1;   //T2  ������,CH1

    /* 2 ��ʼ����ʹ��ADC*/

    //(1)ѡ��ADCģʽ bit10
 //   AD1CON1bits.AD12B = 0x01;  //10λ4ͨ��ģʽ

    //(2)ѡ��ο���ѹԴһƥ��ģ������ķ�Χ
 //   AD1CON2bits.VCFG2 = 1;    //AVDD,AVSS(0-3.3v)

    //(3)ѡ��ģ��ת��ʱ��
//    AD1CON3bits.ADCS = 0x00;  //����ת������

    //(4)����S/Hͨ��
  //  AD1CHS0bits.CH0SA = 0;   //ѡ��AN0ΪCH0��ģ������
 //   AD1CHS0bits.CH0NA = 0;

  //  AD1CHS123bits.CH123SA = 0;
  //  AD1CHS123bits.CH123NA = 0;

    //(5)ʹ�ö��ٸ�ͨ��
  //  AD1CON2bits.CHPS = 0x01;   //ת��CH0��CH1

    //(6)ȷ��������η���
  //  AD1CON1bits.SIMSAM = 1;   //ͬʱ����
  //  AD1CSSHbits = 0x0001;
  //  AD1CSSLbits = 0x0001;

    //(7)ѡ�������ʽ,�Զ�����????
  //  AD1CON1bits.ASAM = 1;
  //  AD1CON1bits.SAMP = 0;

    //(8)ѡ��ת�������Ͳ���ʱ��???????
  //  AD1CON3bits.SAMC = 31;

    //(9)ת������ڿ������е����ݸ�ʽ
  //  AD1CON1bits.FORM = 0x00;    //����

    //(10)�ж�����
   // AD1CON2bits.SMPI = 0x01111;   //ÿ���16�β�������һ���ж�

    //(11)ת������洢
   // AD1CON4bits.ADDMAEN = 0;    //����洢��BUF0~BUFF��

//}



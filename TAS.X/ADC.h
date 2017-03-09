/*
 * File:   ADC.h
 * Author: Zzhao
 *
 * Created on 2017��2��21��, ����4:15
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

//#define NUM_CHS2SCAN    2   //ʹ�ܵ�ͨ����
#define VDD             3.3   //��ѹֵ
#define MAX             4096

//#define SAMP_BUFF_SIZE  8   // ģ������Ļ�������С


void Init_ADC1(void);
//void Init_Time5(void);
void __attribute__ ( (interrupt, no_auto_psv) ) _AD1Interrupt( void );
void Delay_us(unsigned int delay);

void Calculation_Voltage(void);



#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */


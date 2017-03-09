/* 
 * File:   PWM.h
 * Author: Zhao
 *
 * Created on 2017年2月20日, 上午11:15
 */

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif


#define duty_s   10
#define duty_e   80
#define circle   360
#define SAS2_Gear_Ratio 2.6667



void Input_capture_Init_SAS(void);
void __attribute__ ((__interrupt__, no_auto_psv)) _IC1Interrupt(void);
void __attribute__ ((__interrupt__, no_auto_psv)) _IC2Interrupt(void);


int Calculation_Delta(void);



#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */


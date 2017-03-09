
#include <xc.h>
#include <p33Exxxx.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdlib.h>
#include <math.h>

#include "PWM.h"


unsigned int Index,cnt,Fun;
unsigned int Index_1,cnt_1,Fun_1;

unsigned int t,p,t_1,p_1;

unsigned int x[2],Tx[10],Px[10],y[2],Ty[10],Py[10];    //Tx[9:0]��10������ֵ��Px[9:0]��10������ֵ
char duty_cyle_1, duty_cyle_2;

float Absolute_angle;
int Delta;

int SAS1_angle,SAS2_angle;

 int S;





void Input_capture_Init_SAS(void)
{

    cnt = Fun= 0;
    cnt_1 = Fun_1= 0;
    
    //SRbits.IPL = 2;
    IPC0bits.IC1IP = 4;   //��ģ���ж������ȼ�����Ϊ5
                       

    TMR3 = 0;     //Timer3�ļ�������0
    PR3 = 0xffff;    //���ڼĴ��� = 0xffff
    T3CON = 0x8010;   //�������ڲ�ʱ��Fcy=60MHZ,8��Ƶ
    IC1CON1 = 0x00;   //ģ��ر�

    IC1CON1bits.ICI = 1;   //ÿ�������ز�׽��ÿ���β�׽����һ���ж�
    IC1CON1bits.ICM = 3;  //�򵥲�׽ģʽ�������ز�׽ģʽ��
    IC1CON1bits.ICTSEL = 0;   //Timer3�ǲ�׽������ʱ��
    /*initialize input capture module*/
    IFS0bits.IC1IF = 0;   //���жϱ�־λ
    IEC0bits.IC1IE = 1;   //ʹ�����벶׽�ж�

   //����������Ϊ���룬�ҹر�����������
  /// RPINR7bits.IC1R = 24;
    RPINR7bits.IC1R = 42;

/*------------------------------------------------------------------------------*/

   IC2CON1bits.ICI = 1;   //ÿ�������ز�׽��ÿ���β�׽����һ���ж�
    IC2CON1bits.ICM = 2;  //�򵥲�׽ģʽ���½��ز�׽ģʽ��
    IC2CON1bits.ICTSEL = 0;   //Timer3�ǲ�׽������ʱ��

    IFS0bits.IC2IF = 0;   //���жϱ�־λ
    IEC0bits.IC2IE = 1;   //ʹ�����벶׽�ж�


    //����������Ϊ���룬�ҹر�����������
     RPINR7bits.IC2R = 36;

   

}

/*���жϺ����ж�ȡ����ֵ,������ռ�ձ�*/
void __attribute__ ((__interrupt__, no_auto_psv)) _IC1Interrupt(void)
{
  

    IFS0bits.IC1IF = 0;   //����жϱ�־
    if(Fun == 0 )         //������
    {

       //while(IC1CON1bits.ICBNE)
        x[0] = IC1BUF;
        x[1] = IC1BUF;
        Tx[Index++] = x[1] - x[0] ;     //���β�׽֮��Ϊ����ֵ,����������
                                         //����ֵ�ǻ��������

       if(Index >=10)
        {
          IC1CON1bits.ICM = 0;    //�ر�ģ��
          IC1CON1bits.ICM = 1;    //���óɱ��ؼ��ģʽ����ÿ���ϣ��±��ؾ����һ��

           Index =0;
           cnt = 1;
           Fun = 1;
           x[0] = x[1];                //׼��������������ֵΪx[0];

        }
    }

    else    //������
    {

          // while(IC1CON1bits.ICBNE)

          x[cnt++] = IC1BUF;   //���ֵ���� x[1]

         if (cnt>1)         //��ʱ cnt =2
        {
           cnt = 0;   //��Ϊ����ֻ��Ҫ2��ֵ
           Px[Index++] = x[1] - x[0];        //���������ز�׽֮��Ϊ����ֵ,���ֵ��Px[0]
                                             //��ʱ��x[0]�����ϸ�ģʽ��׽���� �����ص�ֵ

            if (Index >=10)
            {
               IC1CON1bits.ICM = 0;    //�ر�ģ��
               IC1CON1bits.ICM = 3;  //�򵥲�׽ģʽ�������ز�׽ģʽ��

             

                t = *(&Tx[6]);
                p = *(&Px[6]);
                duty_cyle_1 = (double)p / (double)t * 100;
                
                Index = 0;
                Fun  = 0;


            }
         }
         
       }

  



}



void __attribute__ ((__interrupt__, no_auto_psv)) _IC2Interrupt(void)
{
   
   IFS0bits.IC2IF = 0;   //����жϱ�־
    if(Fun_1 == 0 )       //������
    {

       //while(IC1CON1bits.ICBNE)
        y[0] = IC2BUF;
        y[1] = IC2BUF;
        Ty[Index_1++] = y[1] - y[0] ;     //���β�׽֮��Ϊ����ֵ,����������
                                         //����ֵ�ǻ��������

       if(Index_1 >=10)
        {
          IC2CON1bits.ICM = 0;    //�ر�ģ��
          IC2CON1bits.ICM = 1;    //���óɱ��ؼ��ģʽ����ÿ���ϣ��±��ؾ����һ��

           Index_1 =0;
           cnt_1 = 1;
           Fun_1 = 1;
           y[0] = y[1];                //׼��������������ֵΪx[0];

        }
    }

    else    //������
    {

          // while(IC1CON1bits.ICBNE)

          y[cnt_1++] = IC2BUF;   //���ֵ���� x[1]

         if (cnt_1>1)         //��ʱ cnt =2
        {
           cnt_1 = 0;   //��Ϊ����ֻ��Ҫ2��ֵ
           Py[Index_1++] = y[1] - y[0];        //���������ز�׽֮��Ϊ����ֵ,���ֵ��Px[0]
                                             //��ʱ��x[0]�����ϸ�ģʽ��׽���� �����ص�ֵ

            if (Index_1 >=10)
            {
               IC2CON1bits.ICM = 0;    //�ر�ģ��
               IC2CON1bits.ICM = 3;  //�򵥲�׽ģʽ�������ز�׽ģʽ��

                Index_1 = 0;
                Fun_1  = 0;

                t_1 = *(&Ty[6]);
                p_1 = *(&Py[6]);
                duty_cyle_2 = (double)p_1 / (double)t_1 * 100;
            }
         }

       }
}


int Calculation_Delta(void)
{
    S = -15;

    int A,B;
    
    int RG_temp;
    int Delta_step;
    int Ideal_delta_step;
    int Delta_step_gap;
    int RG_row;

   

    //(1)����SAS1_angle
    A = (int)( (duty_cyle_1 - duty_s) * circle / duty_e );
    SAS1_angle = (int)(circle - A);

    //(2)����SAS2_angle
    B = (int)( (duty_cyle_2 - duty_s) * circle / duty_e );
    SAS2_angle = (int)(circle - B);

    //(3)Detla calculation
    Delta = SAS2_angle - SAS1_angle;
    if(Delta<0)
    {
        Delta = Delta + circle;
    }

    //(4)RG_temp calculation ,RG is gear ratio(���ֱ�)
    RG_temp =floor(Delta / 30);    //floor����С�ڵ���x���������

    //(5) Delta_step calculationʵ�ʵ�
    Delta_step =(int)fmod(Delta,30);    //ȡ����


    //(6)Ideal_delta_step calculation�������㣨����ģ�
    Ideal_delta_step = SAS2_angle *30 /360;

    //(7)Delta_step_gap calculation ����ĺ���ʵ�ĽǶȲ���֮��
    Delta_step_gap = Ideal_delta_step - Delta_step;

    //(8) RG����
    if(Delta_step_gap < S)
        RG_row = RG_temp + 1;
    else if (Delta_step_gap > 15)
        RG_row = RG_temp - 1;
    else
        RG_row = RG_temp;

    if(RG_row < 0)
        RG_row = 11;
    else if(RG_row > 11)
        RG_row = 0;

    //(9) Absolute angle calculation
    Absolute_angle =  RG_row * (circle/SAS2_Gear_Ratio) + ((float)SAS2_angle /SAS2_Gear_Ratio) ;

    return Absolute_angle;

}







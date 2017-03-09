
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

unsigned int x[2],Tx[10],Px[10],y[2],Ty[10],Py[10];    //Tx[9:0]存10次周期值，Px[9:0]存10次脉宽值
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
    IPC0bits.IC1IP = 4;   //将模块中断那优先级设置为5
                       

    TMR3 = 0;     //Timer3的计数器清0
    PR3 = 0xffff;    //周期寄存器 = 0xffff
    T3CON = 0x8010;   //启动，内部时钟Fcy=60MHZ,8分频
    IC1CON1 = 0x00;   //模块关闭

    IC1CON1bits.ICI = 1;   //每个上升沿捕捉，每俩次捕捉产生一次中断
    IC1CON1bits.ICM = 3;  //简单捕捉模式（上升沿捕捉模式）
    IC1CON1bits.ICTSEL = 0;   //Timer3是捕捉计数器时钟
    /*initialize input capture module*/
    IFS0bits.IC1IF = 0;   //清中断标志位
    IEC0bits.IC1IE = 1;   //使能输入捕捉中断

   //将引脚设置为输入，且关闭其它的外设
  /// RPINR7bits.IC1R = 24;
    RPINR7bits.IC1R = 42;

/*------------------------------------------------------------------------------*/

   IC2CON1bits.ICI = 1;   //每个上升沿捕捉，每俩次捕捉产生一次中断
    IC2CON1bits.ICM = 2;  //简单捕捉模式（下降沿捕捉模式）
    IC2CON1bits.ICTSEL = 0;   //Timer3是捕捉计数器时钟

    IFS0bits.IC2IF = 0;   //清中断标志位
    IEC0bits.IC2IE = 1;   //使能输入捕捉中断


    //将引脚设置为输入，且关闭其它的外设
     RPINR7bits.IC2R = 36;

   

}

/*在中断函数中读取计数值,并计算占空比*/
void __attribute__ ((__interrupt__, no_auto_psv)) _IC1Interrupt(void)
{
  

    IFS0bits.IC1IF = 0;   //清除中断标志
    if(Fun == 0 )         //测周期
    {

       //while(IC1CON1bits.ICBNE)
        x[0] = IC1BUF;
        x[1] = IC1BUF;
        Tx[Index++] = x[1] - x[0] ;     //俩次捕捉之差为周期值,俩个上升沿
                                         //脉宽值是基本不变的

       if(Index >=10)
        {
          IC1CON1bits.ICM = 0;    //关闭模块
          IC1CON1bits.ICM = 1;    //设置成边沿检测模式，即每个上，下边沿均检测一次

           Index =0;
           cnt = 1;
           Fun = 1;
           x[0] = x[1];                //准备测脉宽，上升沿值为x[0];

        }
    }

    else    //测脉宽
    {

          // while(IC1CON1bits.ICBNE)

          x[cnt++] = IC1BUF;   //这个值就是 x[1]

         if (cnt>1)         //此时 cnt =2
        {
           cnt = 0;   //因为数组只需要2个值
           Px[Index++] = x[1] - x[0];        //下沿与上沿捕捉之差为脉宽值,这个值是Px[0]
                                             //此时的x[0]就是上个模式捕捉到的 上升沿的值

            if (Index >=10)
            {
               IC1CON1bits.ICM = 0;    //关闭模块
               IC1CON1bits.ICM = 3;  //简单捕捉模式（上升沿捕捉模式）

             

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
   
   IFS0bits.IC2IF = 0;   //清除中断标志
    if(Fun_1 == 0 )       //测周期
    {

       //while(IC1CON1bits.ICBNE)
        y[0] = IC2BUF;
        y[1] = IC2BUF;
        Ty[Index_1++] = y[1] - y[0] ;     //俩次捕捉之差为周期值,俩个上升沿
                                         //脉宽值是基本不变的

       if(Index_1 >=10)
        {
          IC2CON1bits.ICM = 0;    //关闭模块
          IC2CON1bits.ICM = 1;    //设置成边沿检测模式，即每个上，下边沿均检测一次

           Index_1 =0;
           cnt_1 = 1;
           Fun_1 = 1;
           y[0] = y[1];                //准备测脉宽，上升沿值为x[0];

        }
    }

    else    //测脉宽
    {

          // while(IC1CON1bits.ICBNE)

          y[cnt_1++] = IC2BUF;   //这个值就是 x[1]

         if (cnt_1>1)         //此时 cnt =2
        {
           cnt_1 = 0;   //因为数组只需要2个值
           Py[Index_1++] = y[1] - y[0];        //下沿与上沿捕捉之差为脉宽值,这个值是Px[0]
                                             //此时的x[0]就是上个模式捕捉到的 上升沿的值

            if (Index_1 >=10)
            {
               IC2CON1bits.ICM = 0;    //关闭模块
               IC2CON1bits.ICM = 3;  //简单捕捉模式（上升沿捕捉模式）

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

   

    //(1)计算SAS1_angle
    A = (int)( (duty_cyle_1 - duty_s) * circle / duty_e );
    SAS1_angle = (int)(circle - A);

    //(2)计算SAS2_angle
    B = (int)( (duty_cyle_2 - duty_s) * circle / duty_e );
    SAS2_angle = (int)(circle - B);

    //(3)Detla calculation
    Delta = SAS2_angle - SAS1_angle;
    if(Delta<0)
    {
        Delta = Delta + circle;
    }

    //(4)RG_temp calculation ,RG is gear ratio(齿轮比)
    RG_temp =floor(Delta / 30);    //floor返回小于等于x的最大整数

    //(5) Delta_step calculation实际的
    Delta_step =(int)fmod(Delta,30);    //取余数


    //(6)Ideal_delta_step calculation补偿计算（理想的）
    Ideal_delta_step = SAS2_angle *30 /360;

    //(7)Delta_step_gap calculation 理想的和现实的角度步数之差
    Delta_step_gap = Ideal_delta_step - Delta_step;

    //(8) RG补偿
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







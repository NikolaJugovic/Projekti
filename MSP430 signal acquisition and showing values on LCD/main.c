/**
 * @file main.c
 * @brief Signal acquisition using ADC, and showing values on LCD display
 *
 * In this example signals from 2 potentiometer is conducted to ADC chanels 14 and 15 where are represented as 12bit number.
 * Using LCD display ,converted signals are displayed in range 0-4095. Speed of signals acquisition can be changed from 0.5Hz to 2Hz,
 * with step 0.5Hz.
 *
 * @date 2019
 * @author Nikola Jugovic (jn160408d@student.etf.bg.ac.rs)
 *
 * @version [1.0 - 08/2020] Initial version
 */

#include <msp430.h> 
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "function.h"

#define TIMER_PERIOD   (63) /* ~2ms (1.95ms) */
#define DELAY_1MS   (1005)
#define DELAY_5MS   (1005*5)
#define DELAY_15MS  (1005*15)
/**
 * @brief DATA = it is data it isn't instruction
 * */
#define DATA 1
/**
 * @brief INSTR = it is instruction it isn't data
 * */
#define INSTR 2
#define INIT 3
/**
 * @brief NOT_INIT = it is working it isn't initialization
 * */
#define NOT_INIT 4



volatile  int napon1;
volatile  int napon2;
volatile unsigned int brzina_akv=0;
static volatile uint8_t flag=0;
static volatile uint8_t digits[4] = {0};
/**
 * @brief Different periods used for different speed of acquisition
 * */
const unsigned int timer_periods[] = {
                          32767,        //0.5
                          24575,        //1
                          16383,        //1.5
                          8191          //2
};

#include "lcd_fun.h"


/**
 * @brief Main , using  for initialization buttons,potentiometers,ADC,timer and LCD
 */
  int main(void)
    {
        WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

        P8DIR |= 0xFC;      //LCD set as output

        led_diode();


        P2DIR &= ~BIT4;
        P2DIR &= ~BIT5;
        P2DIR &= ~BIT6;
        P2DIR &= ~BIT7;


        P7SEL |= BIT6;
        P7SEL |= BIT7;

        //ADC initialisation
        ADC12CTL0 = ADC12ON | ADC12SHT0_8 | ADC12MSC;        // ADC turning on+setting number of cycles (256)+
                                                            //activation of ADC at first edge
        ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;   // set SHS = 1 (TA0.0 used for SAMPCON) and Repeat-sequence-of-channels mode
        ADC12MCTL0 = ADC12INCH_14;              // chanem 14
        ADC12MCTL1 = ADC12INCH_15+ADC12EOS;     // chanel 15, end of segment
        ADC12CTL0 |= ADC12ENC;                  // disable conversion              // will be changed with a press of a button
        ADC12IE = ADC12IE1;                     //interrupt enable

        //timer initialisation
        TA1CCR0 = TIMER_PERIOD;
        TA1CCTL0 = CCIE;
        TA1CTL = TASSEL__ACLK | MC__UP;

        lcd();      //initialiaising lcd display

        while (1){
            while (!flag){

                __enable_interrupt();
                prikaz_lcd(napon1,napon2,brzina_akv);

            }
        }
    }

  /**
   * @brief TIMERA0 Interrupt service routine
   *
   * Used for starting ADC
   */
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) CCR0ISR (void) {

    ADC12CTL0 |= ADC12SC;       //startovanje konverzije
    P4OUT |= BIT4;
}
/**
  * @brief TIMERA1 Interrupt service routine
  *
  * Used for debauncing
  */
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TA1CCR0ISR (void) {
    //debouncing variables
    static uint8_t P2IN_OLD4=0;     //button used for start and showing values
    static uint8_t P2IN_OLD5=0;     //increasing acqusition speed
    static uint8_t P2IN_OLD6=0;     //reduction acqusition speed
    static uint8_t P2IN_OLD7=0;     //stopping conversion

   // prikaz_lcd(napon1,napon2);

    //starting timer on S1 button
    if (((P2IN & BIT4) == 0)  && ((P2IN_OLD4 & BIT4) != 0)) {

        TA0CCTL0 =  CCIE;                        // CCR0 int enable
        TA0CCR0 = timer_periods[brzina_akv];
        TA0CTL = TASSEL_1 | MC_1 | TACLR;       // ACLK, up mode, clear TAR

        P4OUT |= BIT3;
    }
    P2IN_OLD4=P2IN;

    if (((P2IN & BIT5) == 0)  && ((P2IN_OLD5 & BIT5) != 0)) {

        if (brzina_akv < 3){
            brzina_akv++;
            TA0CCR0 = timer_periods[brzina_akv];


        }
    }
    P2IN_OLD5=P2IN;

    if (((P2IN & BIT6) == 0)  && ((P2IN_OLD6 & BIT6) != 0)) {
        if (brzina_akv > 0) {
            brzina_akv--;
            TA0CCR0=timer_periods[brzina_akv];

        }
    }
    P2IN_OLD6=P2IN;

    //stopping timer, and ADC converter. Current value staying on LCD display
    if (((P2IN & BIT7) == 0)  && ((P2IN_OLD7 & BIT7) != 0)) {

        TA0CTL &= ~(MC0 | MC1); // stop and clear timer
        TA0CTL |= TACLR;
        P4OUT &= ~BIT3;
        flag=1;
    }
    P2IN_OLD7=P2IN;
}
/*
 * @brief ADC Interrupt service routine
 *  Used for reading converted values
 */
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void){
    uint8_t data,prikaz;
    //static uint8_t P2IN_OLD=0;
    if (ADC12IV == ADC12IV_ADC12IFG1)       //provera jel gotova konverzija
    {
        P4OUT &= ~BIT4;
        napon1=ADC12MEM0;
        napon2=ADC12MEM1;
        flag=0;
    }
}


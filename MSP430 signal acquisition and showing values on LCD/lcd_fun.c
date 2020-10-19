/**
* @file lcd_fun.c
* @brief Declaration of function used for lcd dispay
* @date 08.2020
*@author Nikola Jugovic (jn160408d@student.etf.bg.ac.rs)
*
* @version [1.0 - 08/2020] Initial version
**/

#include <msp430.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "lcd_fun.h"

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

/**
 * @brief Periods used for displaying acqusition speed
 * */
const  float frekv[]={
                         5,
                         10,
                         15,
                         20
};

/**
 * @brief Function that displays numbers and words
 */

void prikaz_lcd( int napon1, int napon2, unsigned int brzina_akv){
    uint8_t decimala,data,ceo_deo,akv;
     akv=frekv[brzina_akv];     //conversion speed
    static volatile uint8_t digits[4] = {0};

    uint16_t nr = napon1;
    uint16_t tmp,dig;

    //acq speed digits
    ceo_deo=akv/10;
    decimala=akv%10;

    lcd_command(0x02, INSTR, NOT_INIT); //nov red
    //prikaz prvog kanala
    lcd_ispis(0x56);    //V
    lcd_ispis(0x31);    //1
    lcd_ispis(0x3D);    //=

    for (tmp=0;tmp<4;tmp++){
        digits[tmp] = nr % 10;
        nr /= 10;
    }
    lcd_ispis(0x30+digits[3]);
    lcd_ispis(0x30+digits[2]);
    lcd_ispis(0x30+digits[1]);
    lcd_ispis(0x30+digits[0]);

    lcd_ispis(0x20);        //empty
    lcd_ispis(0x20);
    //acq speed show
    lcd_ispis(0x66);        //f
    lcd_ispis(0X3D);        //=
    lcd_ispis(0x30+ceo_deo);
    lcd_ispis(0x2E); //.
    lcd_ispis(0x30+decimala);

    lcd_command(0xC0, INSTR, NOT_INIT); //new line

    //second chanel
    lcd_ispis(0x56);    //V
    lcd_ispis(0x32);    //2
    lcd_ispis(0x3D);    //=
    nr=napon2;
    for (tmp=0;tmp<4;tmp++){
            digits[tmp] = nr % 10;
            nr /= 10;
    }
    lcd_ispis(0x30+digits[3]);
    lcd_ispis(0x30+digits[2]);
    lcd_ispis(0x30+digits[1]);
    lcd_ispis(0x30+digits[0]);

}
/**
 * @brief Prints whole number (higher and lower nibble)
 */
void lcd_ispis(uint8_t number){
    uint8_t h = number & 0xF0;
    uint8_t l = (number & 0x0F) << 4;
    h |= BIT2 | BIT3;
    l |= BIT2 | BIT3;
    P8OUT_data(h);
    P8OUT_data(l);
}
/**
 * @brief Print data on the LCD
 */
void P8OUT_data(uint8_t data){
    P8OUT = data;
    __delay_cycles(DELAY_1MS);
    P8OUT &= ~BIT3;    // clear E bit
    __delay_cycles(DELAY_1MS);
}

void lcd_command(uint8_t number, int command, int work) {
    uint8_t high = number & 0xF0;         // high
    uint8_t low = (number & 0x0F) << 4;  // low
    if (command == INSTR) {
        high &= ~BIT2;  // clear RS bit for h
        high |= BIT3;        // set E bit for h
        low &= ~BIT2;  // clear RS bit for l
        low |= BIT3;        // set E bit for l
    }
    if (work == INIT){
        P8OUT_data(low);
    }
    else if (work == NOT_INIT) {
        P8OUT_data(high);
        P8OUT_data(low);
    }
}

/**
 * @brief Function that initialize LCD display
 */
void lcd(){
    // power on
    __delay_cycles(DELAY_15MS);     // Waiting 15ms for VCC
    lcd_command(0x03, INSTR, INIT); // 8-bit interface (rs = 0; r/~w=0; DB_7654 = 0011;)
    __delay_cycles(DELAY_5MS);      // Waiting 5ms ( more 4.1ms is enough)
    uint16_t i = 0;
    uint8_t commands[9];
    commands[0] = 0x03;             //@brief 8-bit interface (rs = 0; r/~w=0; DB_7654 = 0011;)
    commands[1] = 0x03;             //@brief 8-bit interface (rs = 0; r/~w=0; DB_7654 = 0011;)
    commands[2] = 0x02;             //@brief 4-bit interface  (rs = 0; r/~w=0; DB_7654 = 0010;)
    commands[3] = 0x2C;             //@brief 5x8 font and 2 lines (5bits horizontal and 8bits vertical)
    commands[4] = 0x08;             //@brief display off
    commands[5] = 0x01;             //@brief clear display
    commands[6] = 0x06;             //@brief entry mode
    commands[7] = 0x0E;             //@brief display on
    commands[8] = 0x06;             // @brief entry mode
    for (i=0;i<9;++i)
        lcd_command(commands[i],INSTR,(i<3 ? INIT:NOT_INIT));
}

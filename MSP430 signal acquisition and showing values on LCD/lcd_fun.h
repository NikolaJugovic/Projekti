/*
 * lcd_fun.h
 *
 *  Created on: Aug 28, 2020
 *      Author: Nikola Jugovic
 */

#ifndef LCD_FUN_H_
#define LCD_FUN_H_


extern void prikaz_lcd( int napon1, int napon2,unsigned int brzina_akv);
extern void lcd_ispis(uint8_t number);
extern void P8OUT_data(uint8_t data);
extern void lcd();
extern void lcd_command(uint8_t number, int command, int work);

#endif /* LCD_FUN_H_ */

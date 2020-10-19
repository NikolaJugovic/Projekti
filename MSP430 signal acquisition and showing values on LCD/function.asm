
; @file function.h
; @brief Declaration of function used to initialize led diodes
; @date 08.2020
;@author Nikola Jugovic (jn160408d@student.etf.bg.ac.rs)
;
; @version [1.0 - 08/2020] Initial version


			.cdecls C,LIST,"msp430.h","function.h"      ; Include device header file
			.def	led_diode
            .text
led_diode	bis.b   #BIT3,&P4DIR            ; set P4.3 as out
			bis.b   #BIT4,&P4DIR            ; set P4.4 as out
			bis.b   #BIT5,&P4DIR            ; set P4.5 as out
			bis.b   #BIT6,&P4DIR            ; set P4.6 as out
			
			bic.b	#BIT3,&P4OUT            ; clear P4.3
			bic.b	#BIT4,&P4OUT            ; clear P4.4
			bic.b	#BIT5,&P4OUT            ; clear P4.5
			bic.b	#BIT6,&P4OUT            ; clear P4.6
			ret

			.end


/*
 * raw.s
 *
 * Created: 4/23/2016 8:39:13 PM
 *  Author: Ireena
 */ 
#define __SFR_OFFSET 0          // Use 0 for the I/O register offset
#include <avr/io.h>             // Defines I/O port aliases
  
.global fun_c    ; Makes asmfunc_calledfrom_c visible in other source files
  
.section .text                  ; Defines a code section
  
fun_c:           ; Start of asmfunc_calledfrom_c subroutine
    out  PORTB, r24             ; Send value passed to asmfunc_calledfrom_c to PORTB
    ret

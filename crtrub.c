/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains the control sequence needed to operate the
**  various supported display terminals, as well as type determination
**  routines.
**
**  AUTHOR:             Tim E. Sneddon
**
**  Copyright (C) 2011 Tim E. Sneddon <tim@sneddon.id.au>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  CREATION DATE:      08-NOV-2011
**
**  MODIFICATION HISTORY:
**
**      08-NOV-2011 V1.0    Sneddon	Initial coding.
**--
*/
#define MODULE TECO_CRTRUB
#define VERSION "V1.0"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include "tecodef.h"
#include "globals.h"

/*
**++
**  ROUTINE:    decode
**
**  FUNCTIONAL DESCRIPTION:
**
**      Decode escape/ss3/csi sequence if needed.
**
**  RETURNS:    cond_value, longword, write only, by value
**
**  PROTOTYPE:
**
**      decode
**
;       R2 =  SEQUENCE FINAL CODE/VALUE
;       R3 =  SEQUENCE TYPE CODE:
;               0 => <ESCAPE><CTRL-CHAR>
;               1 => <ESCAPE><CHAR>
;                    <ESCAPE><O><UC-CHAR>
;                    <SS3><UC-CHAR>
;                    <ESCAPE><[><CHAR>
;                    <CSI><CHAR>
;               2 => <ESCAPE><?><CHAR>
;                    <ESCAPE><O><LC-CHAR>
;                    <SS3><LC-CHAR>
;               3 => <ESCAPE><[><DIGIT(S)><TILDE>
;                    <CSI><DIGIT(S)><TILDE>

**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  COMPLETION CODES:
**	0	Character is not an escape character.  No arguments are
**		changed.
**	1	Character was an escape character.  The specifics of which
**		can be found in the supplied arguments.
**
**  SIDE EFFECTS:
**	None.
**--
*/
uint32_t decode(code,
		type)
    uint32_t *code;
    uint32_t *type;
{
    uint8_t chr = *code;
    uint32_t etype = ctx.etype;
    int32_t status = 0;

    if ((chr != TECO_C_ESC)
	&& (chr != TECO_C_SS3)
	&& (chr != TECO_C_CSI))
	return status;

    ctx.etype |= TECO_M_ET_CC | TECO_M_ET_LC;
    ctx.etype &= ~TECO_M_ET_CKE;

    if (chr == TECO_C_ESC) {
	/* Fetch the next character in the escape sequence.
	*/
	chr = tlistn();
	if (chr & 0x9f) {
	    /* <escape><ctrl-char> (0)
	    */
	    status = 0;
	} else {
	    status = 1;
	    if (chr == ']') {
		//
	    } else {
		if (chr == '?') {
		    /* <escape><?><char> (2)
	 	    */
		    status = 2;
		    chr = tlistn();
	    	} else if (chr == '0') {
		    if ((chr < 'A') && (chr > 'Z'))
		        status = 2;
		}

		chr &= 0x1f; /* Trim character to 5 bits. */
	    }
	}

	// if chr & 0x9f)
	    // <escape><ctrl-char> (0)
	    // status = 0
	// else
	    // status = 1
	    // if (chr == '[')
		// is <escape><[><char>
		// beql 100$ -- this part needs to fall through to the ss3/csi handler
	    // else if (chr == '?')
		// <escape><?><char>
		// status = 2
		// chr = tlistn();
	    // else if (chr == 'O'
		// chr = tlistn();
		// if (chr < 'A') && (chr > 'Z')
		    // status = 2
	    // 50$:    BICB    #^C<^X1F>,R2    ; Trim character to 5 bits

	//60$:    TSTW    ETYPE(R11)      ; Is control/c trap still on?
	//        BLSS    70$             ; Yes
	//        BICW    #TECO_M_ET$CC,(SP); No, ensure it stays off
	//70$:    BICW    #^C<TECO_M_ET$CC!TECO_M_ET$CKE!TECO_M_ET$LC>,-
	//                (SP)            ; Trim saved "etype"
	//        BICW    #TECO_M_ET$CC!TECO_M_ET$CKE!TECO_M_ET$LC,-
	//                ETYPE(R11)      ; Turn off our bits
	//        BISL    (SP)+,ETYPE(R11); Then restore original "etype"
	//        MOVL    #1,R0           ; Set r0=1 for decoded sequence
    }


// Only handle this part if we are supposed to...Need to fall through here
// if we have not altered 'code'.

//90$:    INCL    R3              ; Set code for <escape><char> (1)
//        CMPB    R2,#SS3         ; Was initial character <ss3>?
//        BEQL    20$             ; Yep, go check the next character...
//100$:   BSBW    TLISTN          ; Get character following <escape><[>
//        BSBB    120$            ; Check for digit (0-9) range
//        BGTRU   50$             ; Not a digit, it's code 1
//        CLRL    R3              ; Else zero our sequence value
//110$:   MOVL    R3,R2           ; Save old value *1
//        MULL2   #10, R3         ; Old value *10
//        ADDL    R4,R3           ; Add new digit to old value
//        BSBW    TLISTN          ; Get the next character
//        BSBB    120$            ; Check for still digit (0-9) range
//        BLEQU   110$            ; A digit, loop...
//        MOVL    R3,R2           ; Else move sequence value to here
//        MOVL    #3,R3           ; Code for <escape><[><digit(s)><tilde> (3)
//        BRB     60$             ; Go exit
}

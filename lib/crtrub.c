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
**  Copyright 2021 Tim E. Sneddon <tim@sneddon.id.au>
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
**
**  CREATION DATE:      08-NOV-2011
**
**  MODIFICATION HISTORY:
**
**      08-NOV-2011 V1.0    Sneddon	Initial coding.
**      06-JUN-2013 V1.0    Sneddon	Added decode.
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
#include <ctype.h>
#include "tecodef.h"
#include "extrn.h"
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
uint32_t decode(cptr)
    uint32_t *cptr;
{
    uint32_t code = *cptr, etype = ctx.etype, type = 0;
    int32_t status;

    if ((code != TECO_C_ESC)
	&& (code != TECO_C_SS3)
	&& (code != TECO_C_CSI))
	return type;

    ctx.etype |= TECO_M_ET_CC | TECO_M_ET_LC;
    ctx.etype &= ~TECO_M_ET_CKE;

    switch (code) {
    case TECO_C_ESC:
	code = tlistn();
	if (iscntrl(code)) {
	    type = 0;
	} else if (code == '[') {
	    code = tlistn();
	    if (isdigit(code)) {
		int i = 0;

		type = 3;
	    	do {
		    i = (i * 10) + (code - '0');
	    	} while (isdigit(code = tlistn()));
		code = i;
	    } else {
		type = 1;
		code = tlistn();
	    }
	} else if (code == '?') {
	    type = 2;
	    code = tlistn();
	} else if (code == 'O') {
	    code = tlistn();
	    if (isupper(code)) {
		type = 1;
	    } else {
		type = 2;
	    }
	}
	break;

    case TECO_C_SS3:
	code = tlistn();
	if (islower(code)) {
	    type = 2;
	} else {
	    type = 1;
	}
	break;

    case TECO_C_CSI:
	code = tlistn();
	if (isdigit(code)) {
	    int i = 0;

	    type = 3;
	    do {
	    	i = (i * 10) + (code - '0');
	    } while (isdigit(code = tlistn()));
	    code = i;
	} else {
	    type = 1;
	}
	break;
    }

    *cptr = code;
    return type;
}

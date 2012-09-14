/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains TECO's I/O routines.
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
**  CREATION DATE:      18-OCT-2011
**
**  MODIFICATION HISTORY:
**
**      18-OCT-2011 V41.00  Sneddon	Initial coding.
**	10-NOV-2011 V41.01  Sneddon	Add tlistn.
**--
*/
#define MODULE TECOIO
#define VERSION "V41.01"
#ifdef vms
#  ifdef VAX11C
#    module MODULE VERSION
#  else
#    pragma module MODULE VERSION
#  endif
#endif
#include <ctype.h>
#include <strings.h>
#include "tecodef.h"
#include "tecomsg.h"
#include "tecoio.h"

/*
** Forward Declarations
*/

    void teco_type();
    uint8_t listen();
    void teco_delch();

/*
** Own Storage
*/
    static char cnv8bt[][2] = {
	{ '8', '0' | 0x80 }, { '8', '1' | 0x80 }, { '8', '2' | 0x80 }, { '8', '3' | 0x80 },
	{ '8', '4' | 0x80 }, { '8', '5' | 0x80 }, { '8', '6' | 0x80 }, { '8', '7' | 0x80 },
	{ '8', '8' | 0x80 }, { '8', '9' | 0x80 }, { '8', 'A' | 0x80 }, { '8', 'B' | 0x80 },
	{ '8', 'C' | 0x80 }, { '8', 'D' | 0x80 }, { '8', 'E' | 0x80 }, { '8', 'F' | 0x80 },
	{ '9', '0' | 0x80 }, { '9', '1' | 0x80 }, { '9', '2' | 0x80 }, { '9', '3' | 0x80 },
	{ '9', '4' | 0x80 }, { '9', '5' | 0x80 }, { '9', '6' | 0x80 }, { '9', '7' | 0x80 },
	{ '9', '8' | 0x80 }, { '9', '9' | 0x80 }, { '9', 'A' | 0x80 }, { '9', 'B' | 0x80 },
	{ '9', 'C' | 0x80 }, { '9', 'D' | 0x80 }, { '9', 'E' | 0x80 }, { '9', 'F' | 0x80 },
	{ 'A', '0' | 0x80 }, { '!', '!'        }, { 'C', '\\'       }, { 'L', '-'        },
	{ 'A', '4' | 0x80 }, { 'Y', '-'        }, { 'A', '6' | 0x80 }, { 'S', '0'        },
	{ 'X', '0'        }, { 'C', '0'        }, { 'a', '_'        }, { '<', '<'        },
	{ 'A', 'C' | 0x80 }, { 'A', 'D' | 0x80 }, { 'A', 'E' | 0x80 }, { 'A', 'F' | 0x80 },
	{ '0', '^'        }, { '+', '-'        }, { '2', '^'        }, { '3', '^'        },
	{ 'B', '4' | 0x80 }, { '/', 'U'        }, { 'P', '!'        }, { '.', '^'        },
	{ 'B', '8' | 0x80 }, { '1', '^'        }, { 'o', '_'        }, { '>', '>'        },
	{ '1', '4'        }, { '1', '2'        }, { 'B', 'E' | 0x80 }, { '?', '?'        },
	{ 'A', '`'        }, { 'A', '\''       }, { 'A', '^'        }, { 'A', '~'        },
	{ 'A', '"'        }, { 'A', '*'        }, { 'A', 'E'        }, { 'C', ','        },
	{ 'E', '`'        }, { 'E', '\''       }, { 'E', '^'        }, { 'E', '"'        },
	{ 'I', '`'        }, { 'I', '\''       }, { 'I', '^'        }, { 'I', '"'        },
	{ 'D', '0' | 0x80 }, { 'N', '~'        }, { 'O', '`'        }, { 'O', '\''       },
	{ 'O', '^'        }, { 'O', '^'        }, { 'O', '~'        }, { 'O', '"'        },
	{ 'O', 'E'        }, { 'O', '/'        }, { 'U', '`'        }, { 'U', '\''       },
	{ 'U', '^'        }, { 'U', '"'        }, { 'Y', '"'        }, { 'D', 'E' | 0x80 },
	{ 's', 's'        }, { 'a', '`'        }, { 'a' ,'\''       }, { 'a', '^'        },
	{ 'a', '~'        }, { 'a', '"'        }, { 'a', '*'        }, { 'a', 'e'        },
	{ 'c', ','        }, { 'e', '`'        }, { 'e', '\''       }, { 'e', '^'        },
	{ 'e', '"'        }, { 'i', '`'        }, { 'i', '\''       }, { 'i', '^'        },
	{ 'i', '"'        }, { 'F', '0' | 0x80 }, { 'n', '~'        }, { 'o', '`'        },
	{ 'o', '\''       }, { 'o', '^'        }, { 'o', '~'        }, { 'o', '"'        },
	{ 'o', 'e'        }, { 'o', '/'        }, { 'u', '`'        }, { 'u', '\''       },
	{ 'u', '^'        }, { 'u', '"'        }, { 'y', '"'        }, { 'F', 'E' | 0x80 },
	{ 'F', 'F' | 0x80 } };

void teco_type(oflags,
	       c)
    const uint32_t oflags;
    const uint8_t c;
{
    uint8_t chr = c;

    if (oflags & TYPEF) {
	/* Type with case flagging */
	if (((ctx.euflag == 0) && islower(chr))
	    || ((ctx.euflag > 0) && isupper(chr))) {
	    io_support.output('\'');
	    chr = toupper(chr);
	}
    }

    if (ctx.etype & TECO_M_ET_BIN) {
	io_support.output(chr);
    } else {
	if (chr > TECO_C_DEL) {
	    if (cnv8bt[chr][1] & 0x80) {
		io_support.output('[');
		io_support.output(cnv8bt[chr][0]);
		io_support.output(cnv8bt[chr][1] & ~0x80);
		io_support.output(']');
	    } else {
		io_support.output(chr);
	    }
	} else if (chr > 0) {
	    if (chr < TECO_C_SPACE) {
                if (((chr <= TECO_C_BEL) || (chr > TECO_C_CR)) &&
                    !((chr == TECO_C_BEL) && (oflags & TECO_M_TYPE_B))) {

                    if (chr == TECO_C_ESC) {
                        chr = '$';
                    } else {
                        io_support.output('^');
                        chr |= 0x40;
                    }
                }
	    }

	    io_support.output(chr);
	}
    }
}

uint8_t listen(isfirst)
    const uint32_t isfirst;
{
    uint8_t chr;
    uint32_t isdelim = 0;
    int32_t status;

    // if (ctx.indir)
	// chr = teco_fs_listen();

    /*
    ** Has the secondary input channel been closed due to EOF?
    */
    if (!ctx.indir) {
	/*
	** Yes.  Begin listening on the terminal...
	*/
	io_support.input(&chr);

	if (ctx.etype & TECO_M_ET_GRV) {
	    if (chr == ctx.eeflg) {
		chr = TECO_C_GRV;
		isdelim = 1;
	    }
	}

	if (chr != TECO_C_DEL) {
	    type(chr);

	    if (isdelim)
		chr = TECO_C_ESC;

	    if (isfirst) {
		if (ctx.edit & TECO_M_ED_IMD) {
		    // attempt escape sequence convert...
		}
	    } else {
		/*
		** Does the command register have enough characters to be a 'HELP' command?
		*/
		if (ctx.qpntr->qrg_size >= 4) {
		    /* Yes.  Attempt to determine...
		    */
		    if (((chr == TECO_C_ESC) && (ctx.temp == TECO_C_ESC))
		        || ((chr == TECO_C_LF) && (ctx.temp == TECO_C_LF))) {

			status = strncasecmp("HELP", (const char *)ctx.qpntr->qrg_ptr, 4);
			if (status == 0) {
			    if (ctx.temp == TECO_C_ESC)
				print("\r\n", 2);

			    /*
			    ** Remove HELP (4) and ESCape or CR (1) and call the OS
			    ** specific handler.
			    */
			    // teco_os_help(...);

			    /*
			    ** Setup the interpreter to restart.
			    */
			    chr = TECO_C_ESC;
			    ctx.temp = TECO_C_ESC;
			    ctx.qpntr->qrg_size = 0;
			}
		    }
		}
	    }
	}
    }

    return chr;
}

void teco_delch(void)
{
    if (ctx.qpntr->qrg_size > 0) {
	if (ctx.etype & TECO_M_ET_CRT) {
	    // scope handling code goes here,,,
	} else {
	    type(ctx.qpntr->qrg_ptr[ctx.qpntr->qrg_size-1]);
	}

	ctx.qpntr->qrg_size--;
    }
}

uint8_t tlistn(void)
{
    uint8_t chr;
    uint32_t etype = ctx.etype;
    int32_t status;

    ctx.etype &= ~(TECO_M_ET_CC | TECO_M_ET_CKE | TECO_M_ET_NCH);

    /* This needs fixing!  It is not possible to signal 'status' in a consistent
    ** way.  At the moment the termios version of 'input' can only return
    ** TECO__NORMAL or TECO__ERR (to indicate a system error).  This needs to be
    ** corrected so better status values can be handled.
    */
    status = io_support.input(&chr);
    if (status != TECO__NORMAL)
	ERROR_MESSAGE(ERR);

    ctx.etype = etype;
    return chr;
}

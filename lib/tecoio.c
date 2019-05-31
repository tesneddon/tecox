/*
**++
**  MODULE DESCRIPTION:
**
**      This module contains TECO's I/O routines.
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
**      18-OCT-2011 V41.00  Sneddon     Initial coding.
**      10-NOV-2011 V41.01  Sneddon     Add tlistn.
**      24-JUL-2014 V41.02  Sneddon     Support changes to input() callback.
**--
*/
#define MODULE TECOIO
#define VERSION "V41.02"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "tecodef.h"
#include "tecomsg.h"
#include "globals.h"
#include "macros.h"

/*
** Forward Declarations
*/
    int32_t teco_init();
    static void errors();
    static void ctrlcast();
    static void winch();
    void teco_type();
    uint8_t listen();
    void teco_delch();
    int8_t teco_input();
    static int32_t close_indir();
    static int32_t close_input();
    int32_t teco_putbf();
    static int32_t save_data();
    int32_t teco_getbf();
    static uint32_t getbyt();
    int32_t teco_yank();
    int32_t teco_bakup();

/*
** Own Storage
*/
    QRGDEF qreg_array[TECO_K_NUMQRG+1], qreg_local[TECO_K_NUMQRG];
    static TECFAB indir_cmd = { 0 }, *cmdprm = &indir_cmd;
    static TECFAB inpnor = { 0 }, inpalt = { 0 };
    static TECFAB oupnor = { 0 }, oupalt = { 0 };
    static int ctrlz_cnt = 0;
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

int32_t teco_init()
{
    struct sigaction action;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = errors;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);

    memset(&ctx, 0, sizeof(TECODEF));

    ctx.flags |= TECO_M_EGINH;
    // xlate hook...

    ctx.inpntr = &inpnor;
    ctx.oupntr = &oupnor;

    memset(qreg_array, 0, sizeof(qreg_array));
    memset(qreg_local, 0, sizeof(qreg_local));
    ctx.lclptr = qreg_local;
    ctx.qarray = qreg_array;
    ctx.qpntr = &ctx.qarray[TECO_K_QRG_FAKE];

    ctx.etype |= TECO_M_ET_XIT;
    ctx.symspc = '$'; // need to get this from OS-specific stuff, maybe init routine...

    memset(&scope, 0, sizeof(SCOPEDEF));

    io_support.init();
    io_support.crtset();

    ctx.quote = TECO_C_ESC;

    action.sa_handler = ctrlcast;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, 0);

    action.sa_handler = winch;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGWINCH, &action, 0);

    return TECO__NORMAL;
} /* teco_init */

static void errors(signum)
    int signum;
{
    if (ctx.errcod == TECO__NORMAL)
        return;

    if (ctx.errcod > TECO__NORMAL) {
        ctx.errptr = ctx.qcmnd->qrg_ptr;
        ctx.errpos = ctx.scanp - ctx.errptr;
    }

    teco_putmsg();

    if (ctx.errcod > TECO__NORMAL) {
        crlfno();
        goto_unwind();
    }
} /* errors */

static void ctrlcast(signum)
    int signum;
{
    if (ctx.etype & TECO_M_ET_CC) {
        ctx.etype &= ~TECO_M_ET_CC;
        ctx.etype |= TECO_M_ET_CCO;
    } else {
        ctx.flags != TECO_M_STOP;

        // ctrlo_flag = TECO_TRUE;
        // cancel I/O on terminal output channel
        if (!(ctx.etype & TECO_M_ET_XIT)) {
            // ctrlc_flag = -1;
        }
    }
} /* ctrlcast */

static void winch(signum)
    int signum;
{
    /* Do nothing at the moment.  However, update flags
     * with new window size.
     */
} /* winch */

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
} /* teco_type */

/**
 * Get a command character
 */
uint8_t listen(isfirst)
    const uint32_t isfirst;
{
    uint8_t chr;
    uint32_t isdelim = 0;
    int32_t status;

    chr = teco_input();
    if (!ctx.indir) {
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
                ** Does the command register have enough characters to be
                ** a 'HELP' command?
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
} /* listen */

/**
 * delete handler
 */
void teco_delch()
{
    int count = 1;
    uint8_t chr;

    if (ctx.qpntr->qrg_size > 0) {
        chr = ctx.qpntr->qrg_ptr[ctx.qpntr->qrg_size-1];

        if (ctx.etype & TECO_M_ET_CRT) {
            int i;

            if (chr > TECO_C_DEL) {
                if ((cnv8bt[chr][1] & 0x80) && !(ctx.etype & TECO_M_ET_8BT))
                    count = 4;
            } else if (chr > TECO_C_SPACE) {
                count = 1;
            } else if (chr == TECO_C_ESC) {
                count = 1;
            } else if (chr > TECO_C_CR) {
                count = 2; //goto 30$
            } else if (chr <= TECO_C_BEL) {
                count = 2;
            } else if (chr < TECO_C_LF) {
                count = 2; //goto 30$
            } else if (chr == 10) {
                count = 2; //goto 60$
            } else if (chr < 12) {
                count = 2;  //goto 50$
            } else {
                count = 2; //50$(), 60$() ; do three cursor up
            }

            for (i = 0; i < count; i++) {
                uint8_t *p = scope.seqerc;

                do {
                    io_support.output(*p);
                } while (*++p != '\0');
            }
        } else {
            type(chr);
        }

        ctx.qpntr->qrg_size -= count;
    }
} /* teco_delch */

uint8_t tlistn()
{
    uint8_t chr;
    uint32_t etype = ctx.etype;
    int32_t status;

    ctx.etype &= ~(TECO_M_ET_CC | TECO_M_ET_CKE | TECO_M_ET_NCH);
    chr = teco_input();
    ctx.etype = etype;

    return chr;
} /* tlistn */

/**
 * get files (EB, EI, EN, EP, ER, EW)
 */
int32_t teco_getfl(chr)
    uint8_t chr;
{
    int32_t status = TECO__NORMAL;

    if (ctx.filbuf.qrg_size == 0) {
        switch (chr) {
        case 'I':
            status = close_indir();
            break;

        case 'N':
            ctx.qnmbr = &ctx.filbuf;
            status = io_support.en_next();
            break;

        case 'P':
        case 'R':
            ctx.inpntr = (chr == 'P') ? &inpalt : &inpnor;

            ctx.flags &= ~TECO_M_EOFLAG;
            if (ctx.inpntr->tecfab != 0) {
                if (!(ctx.inpntr->tecsts & TECO_M_TECBUF)) {
                    if (io_support.eofl(ctx.inpntr->tecfab))
                        ctx.flags |= TECO_M_EOFLAG;
                }

                ctx.qnmbr = &ctx.filbuf;
                io_support.set_filename(ctx.inpntr->tecfab);
            }
            break;
        }
    } else {
        switch (chr) {
        case 'I':
            close_indir();
            status = io_support.getfl(&cmdprm->tecfab, TECO_K_GETFL_READ);
            if (status == TECO__NORMAL)
                ctx.indir = 1;
            break;

        case 'N':
            status = io_support.en_preset();
            break;

        case 'R':
            //close_input();
            if (ctx.inpntr == 0) prinz("null pointer");
            status = io_support.getfl(&ctx.inpntr->tecfab, TECO_K_GETFL_READ);
            break;
        }
    }

    return status;
} /* teco_getfl */
#if 0
/**
 * close input file
 */
static void close_input() {
    ctx.flags2 &= ~TECO_M_EOFLAG;
    if (ctx.inpntr->tecfab != 0) {
        if (ctx.inpntr->tecsts & TECO_M_TECBUF) {
            ctx.
        } else {
        }
    }
} /* close_input */
#endif
/**
 * terminal (and indirect) input
 */
int8_t teco_input()
{
    int8_t chr, xitchr = TECO_C_SUB;
    int32_t status;

    if (ctx.temp == TECO_C_CR) {
        /*  
        ** All CRs get translated to CR/LFs, so return the LF portion.
        */
        chr = TECO_C_LF;
    } else {
        if (ctx.indir == 0) {
            status = io_support.input(0, &chr);
            if (status == TECO__NORMAL) {
                /*  
                ** Modify immediate exit character if UNIX specific ^D
                ** (instead of ^Z) support enabled.
                */
                if (ctx.etype & TECO_M_ET_UNIX)
                    xitchr = TECO_C_EOT;

                if (chr == xitchr) {
                    if (ctx.temp == xitchr)
                        ctrlz_cnt++;
                    else
                        ctrlz_cnt = 1;

                    if (ctrlz_cnt >= TECO_K_CTRLZ_MAX)
                        exit(TECO__NORMAL);
                }   
            }
        } else {
            /*  
            ** Indirect command input is active, so read the character
            ** from the secondary command input.
            */
            status = io_support.input(indir_cmd.tecfab, &chr);
            if (chr == EOF) {
                /*  
                ** We are at the end of file.  This isn't something we
                ** show to the user, we just silently switch back to the
                **
                ** So, close off the indirect input and call ourselves to
                ** fetch the next character off the primary input.
                */
                close_indir();
                chr = teco_input();
            }   
        }   
    }   

    return chr;
} /* teco_input */

/**
 * close indirect w/ error checking
 */
static int32_t close_indir() {
    int32_t status = TECO__NORMAL;

    if (ctx.indir == 1) {
        ctx.indir = 0;
        status = io_support.clsfl(&cmdprm->tecfab);
    }

    return status;
} /* close_indir */

int32_t teco_getbf(ptr,
                   len)
    uint8_t *ptr;
    int32_t len;
{
    int32_t status = TECO__NORMAL;

    ctx.flags2 &= ~(TECO_M_EOFLAG | TECO_M_FFFLAG);

    if (ctx.inpntr == 0)
        ERROR_MESSAGE(NFI);
#if 0
    if (len != 0) {
        do {
            uint8_t chr;

            len--;

            if (!(ctx.inpntr->tecsts & TECO_M_TECBUF)) {
                if (ctx.inpntr->tecsts & TECO_M_TECEOF) {
                    ctx.flags |= TECO_M_EOFLAG;
                    break;
                }
            }

            status = getbyt(&chr);
            if (status == TECO__EOF) {
                ctx.inpntr->tecsts |= TECO_M_TECEOF;

                io_support.clsfl();

                // success_or_err

                ctx.flags2 |= TECO_M_EOFLAG;
                break;
            }
	    // success_or_err
	    // 3177!!
	    if (ctx.inpntr->tecsts & TECO_M_TECB2) {
		if (chr == TECO_C_CR) {
		    /* ignore the <cr> (for now...) */
		}
	    }

	    if (chr != TECO_C_FF) {
		// store in text buffer
		ctx.zz++;

// r0    = status
// r1    = character just read
// r2/r7 = buffer ptr
// r6    = inpntr
// r3/r8 = room counter/how much to read
// r4

		if (chr == TECO_C_LF) {
		    if (ctx.inpntr->tecsts & TECO_M_TECB2) {
			do {
			    /* backup the buffer pointer */
			    p--;
			    ctx.zz--;
			    len++;

			    
			} while ();


			// bsbb 130$
			// store CR+LF in buffer
			ctx.zz += 2;
			len -= 2;
		    }

		    // 100$
		}
	    }
        } while ((chr != TECO_C_FF) && (still enough room));
    }

    ctx.curfre = ctx.zmax - ctx.zz;
#endif
    return status;
} /* teco_getbf */
#if 0
/**
 * Get an input byte
 */
static uint32_t getbyt(chr)
    uint8_t *chr;
{
    TECQUE *entry = ctx.inpntr->tecque.next;
    uint8_t chr;
    int32_t status;

    if (ctx.inpntr->tecsts & TECO_M_TECBUF) {
	/* Fetch from buffered data if there is some */
        chr = *(entry->ptr++);

	if (--entry->size == 0) {
	    remque(entry);

	    if (mtque(ctx.inpntr->tecque))
		ctx.inpntr->tecsts &= ~TECO_M_TECBUF;

	    free(entry);
	}
    } else if (ctx.inpntr->tecsts & TECO_M_TECECR) {
	/* Do <lf> if extra <cr> */
	ctx.inpntr->tecsts &= ~TECO_M_TECECR;

	ctx.inpntr->tecctl[0] = 10; 
	ctx.inpntr->tecctl[1] = 0; /* indicate left margin */
    } else {
	chr = ctx.inpntr->tecdsp(); /* WHAT */
	ctx.inpntr->tecctl[0] = chr;
    }

    return status;
} /* getbyt */
#endif
/**
 * put output
 *
 * @param txp Text buffer pointer
 * @param len Text buffer character count
 * @param ffflag emit <ff> flag here
 */
int32_t teco_putbf(txp,
                   len,
                   ffflag)
    uint8_t *txp;	// r3
    int32_t len;	// r4
    uint32_t ffflag;	// r9
{
    int32_t status;

    if (ctx.oupntr == 0) {
	status = TECO__NFO;
    } else {
        status = save_data(txp, len, ffflag);
	if (status == TECO__NORMAL) {
	    if (!(ctx.oupntr->tecsts & TECO_M_TECBUF))
		status = io_support.putbf(txp, len, ffflag);
	}
    }

    return status;
} /* teco_putbf */

static int32_t save_data(txp,
                         len,
                         ffflag)
    uint8_t *txp;
    int32_t len;
    uint32_t ffflag;
{
    TECQUE *entry;
    int32_t status = TECO__NORMAL;
    uint8_t *bufptr = txp, *ffp;
    int32_t buflen, bufrem = len;

    do {
        ffp = memchr(bufptr, TECO_C_FF, bufrem);
        if (ffp != 0) {
            /*
             * Found an embedded <ff>, so split the buffer into multiple
             * pages.
             */
            buflen = ffp - bufptr;
        } else {
            buflen = bufrem;
        }

        entry = malloc(sizeof(TECQUE) + buflen - 1);
        if (entry == 0) {
            status = TECO__MEM;
            break;
        }

        entry->ptr = entry->data;
        entry->size = len;
        memcpy(entry->data, bufptr, buflen);
        insque(entry, &ctx.oupntr->tecque);

        bufrem -= buflen;
        bufptr += buflen;
    } while (ffp != 0);

    return status;
} /* save_data */

int32_t yanky() {
    ctx.temp &= 0xFFFF00FF;

    return 0; // TODO:teco_yank();
} /* yanky */

#if 0
// 0 = EOF; 1 =  readsomething
/**
 * 
 */
int32_t teco_yank()
{
    int32_t status;

    if (ctx.flags2 & TECO_M_EOFLAG)
        return 0;

    for(;;) {
	uint32_t zz = ctx.zz;
	uint32_t zmax = ctx.zmax;

	txrem = zmax / 4;
	txrem = txrem >= 256 ? txrem : 256;
	txrem = txrem <= 8192 ? txrem : 8192;

	zmax -= zz;

	if ((int8_t)(ctx.temp >> 8) < 0)
	    txrem = zmax;

        status = teco_getbf(&ctx.txstor[zz], txrem);
        if (status != TECO__NORMAL) {
            // IOERR
        }

        if (ctx.flags2 & (TECO_M_FFFLAG | TECO_M_EOFLAG)) {
            /* If the FF or EOF flags are set? */
	    if ((int8_t)(ctx.temp >> 8) < 0) {
                /* Not a full buffer load, so say we read something. */
                status = 1;
                break;
            }

            if (ctx.edit & TECO_M_ED_EXP) {
		/* Allowing arbitrary expansions */
		status = txadj(512);
		if (status == 0)
		    status = 1;
		    break;
		}

		yankf();
            }
        } else {
            status = 1;
            break;
        }
    }

    return status;
} /* teco_yank */
#endif

int32_t teco_bakup(txp,
		   len,
		   ffflag,
		   npg,
		   rempg)
    uint8_t *txp;
    int32_t len;
    int32_t ffflag;
    int32_t npg;
    int32_t *rempg;
{
    int32_t status = TECO__NORMAL;

    if (ctx.inpntr == 0)
	return TECO__NFI;
    if (ctx.oupntr == 0)
	return TECO__NFO;

    status = teco_putbf(txp, len, ffflag);
    if (status == TECO__NORMAL) {
        if (!(ctx.oupntr->tecsts & TECO_M_TECBUF)) {
	    status = io_support.rewfl(ctx.oupntr->tecfab);
	    if (status == TECO__NORMAL)
		ctx.oupntr->tecsts |= TECO_M_TECBUF;
	}

	// while
	    // remque -- output tail
	    // insque -- input head
    }

    return status;
} /* teco_bakup */

/*
**++
**  MODULE DESCRIPTION:
**
**	PDP-11 TECO
**
**  PDP-11 TECO STARTED OUT AS A BRUTE FORCE TRANSLATION OF OS/8
**  TECO BY HANK MAURER AND BOB HARTMAN AT FORD OF COLOGNE,
**  WEST GERMANY. OS/8 TECO CAME FROM A PROGRAM ORIGINALLY
**  WRITTEN BY RUSSELL HAMM, WAY BACK WHEN... IT WAS MODIFIED
**  FOR OS/8 BY THE O.M.S.I. CREW, SPEEDED UP, SHORTENED, AND
**  MAKE PDP-10 COMPATIBLE BY RICHARD LARY OF DEC WITH ASSISTANCE
**  FROM MARIO DENOBLI OF THE P?S. THE BRUTE FORCE TRANSLATION
**  WAS FURTHER MUNGED AND ALTERED BY MARK BRAMHALL OF DEC TO
**  BRING IT INTO MORE STANDARD PDP-11 CODE AND MAKE A HIGH/LOW
**  SEGMENT SPLIT. MEMORY SIZE EXPANSION WAS ADDED. FURTHER PDP-10
**  COMPATABILITY WAS DONE BY ANTON CHERNOFF. VARIOUS NEW COMMANDS
**  AND FEATURES WERE ADDED AGAIN. FINALLY HERE WE ARE... TECO-11!
**  The PDP-11 Version was Brute-Forced to VAX Native mode
**  From TECO.MAC 29-JAN-1985 by Rick Murphy
**
**  Now further down the line, VAX TECO-32 has been translated
**  from MACRO-32 to C making it portable to almost any operating
**  system with a C compiler. 28-Mar-2005, Tim E. Sneddon.
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
**  CREATION DATE:      09-OCT-2011
**
**  MODIFICATION HISTORY:
**
**      09-OCT-2011 V41.00  Sneddon	Initial coding.
**	10-NOV-2011 V41.01  Sneddon	Fix up radix support and add integer
**					handling.
**	14-DEC-2011 V41.02  Sneddon	Add support for a number of 'E'
**					commands.
**	25-SEP-2012 V41.03  Sneddon	Correctly unwind PDL stack. Add ^B,
**					^C, nA, D, K.
**	08-OCT-2012 V41.04  Sneddon	Add getstg for O and others.
**	07-NOV-2012 V41.05  Sneddon	Add U, X.
**	06-JUN-2013 V41.06  Sneddon	Add :^T.
**	09-JUN-2013 V41.07  Sneddon	Switch to snprintf. Change to :^T.
**					Fix definition of tecocmd.
**	11-JUN-2013 V41.08  Sneddon	Add EG and fixe bug in getstg.
**	21-JAN-2014 V41.09  Sneddon	Fix rather glaring bug in number
**					handler.
**	22-JAN-2014 V41.10  Sneddon	Added conditional and iteration
**					support. Add "I".
**	23-JAN-2014 V41.11  Sneddon	Fixed bug in ")" handling.  Fixed
**					some flag test bugs and tidy up.
**					Rewrite number getter, '\'.  Fix bug
**					in 0L.
**	28-JAN-2014 V41.12  Sneddon	Add illegal commands and ^U.
**--
*/
#define MODULE TECO
#define VERSION "V41.12"
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
#include <time.h>
#include "tecodef.h"
#include "tecomsg.h"
#include "globals.h"

/*
** Forward Declarations
*/

    int32_t teco();
    int32_t teco_init();
    static void errors();
    void teco_interp();
    static uint8_t scan();
    static uint32_t tstnxt();
    static void ncom();
    static void nlines();
    static void skpquo();
    static void skpset();
    static void gettx();
    static void qref();
    static void zerod();
    void txadj();
    void qset();
    static void getstg();
    static void push();
    static void pop();
    static void poplcl();
    static uint32_t search();

/*
** Macro Definitions
*/

#define bzchk(n) \
do { \
    if ((n) > ctx.zz) \
	ERROR_MESSAGE(POP); \
} while (0)

#define chkclo() \
do { \
    if (ctx.flags & TECO_M_CLNF) { \
	ctx.flags &= ~TECO_M_CLNF; \
	ncom(-1); \
    } \
} while (0)

#define chkstp() \
do { \
    if (ctx.flags & TECO_M_STOP) { \
	ctx.flags &= ~TECO_M_STOP; \
	ERROR_MESSAGE(XAB); \
    } \
} while (0)

#define clrend() memset(&ctx, 0, (size_t)&ctx.pst - (size_t)&ctx)

#define crlfno() \
do { \
    type(TECO_C_CR); \
    type(TECO_C_LF); \
} while (0)

#define getn() \
do { \
    if (!(ctx.flags & TECO_M_NFLG)) \
	ncom(1); \
    ctx.flags &= ~TECO_M_NFLG; \
} while (0)

#define getquo() \
do { \
    if (ctx.flags & TECO_M_QFLG) { \
	ctx.flags &= ~TECO_M_QFLG; \
	ctx.quote = scan(); \
    } \
} while (0)

#define resquo() ctx.quote = TECO_C_ESC

#define irest() \
do { \
    ctx.flags &= ~TECO_M_QFLG; \
    resquo(); \
} while (0)

#define scnupp(chr) (toupper(scan(chr)))

#define setcmd(q) \
do { \
    ctx.qcmnd = (q); \
    ctx.qlengt = ctx.qcmnd->qrg_size; \
    ctx.scanp = ctx.qcmnd->qrg_ptr; \
} while (0)

#define trace(c) \
do { \
    chkstp(); \
    if (ctx.flags & TECO_M_TFLG) \
	type((c)); \
} while (0)

#define OP_ADD  0    /* Define operators for ncom */
#define OP_SUB  1
#define OP_MUL  2
#define OP_DIV  3
#define OP_AND  4
#define OP_OR   5

#define SUR_FAIL 0x8000
#define SUR_OK   0x4000
#define SUR_REV  0x2000
#define SUR_BND  0x1000			/* Bounded search flag */
#define SUR_NPG  0x0800

/*
** Global Storage
*/

    TECODEF ctx;
    SCOPEDEF scope;

/*
** Own Storage.
*/

    const static uint8_t tecocmd[] = "\001Welcome to TECO!\001\033\033";  // Remove this in future...
    QRGDEF qreg_array[TECO_K_NUMQRG+1], qreg_local[TECO_K_NUMQRG];

int32_t teco(void)
{
    uint8_t chr;
    uint32_t docrlfno, more, storechr;
    uint32_t status;

    teco_init();

    /*
    ** Load the command decoder into Q-reg Y.
    */
    qref(0, 'Y');
    qset(0, tecocmd, sizeof(tecocmd) - 1);

    /*
    ** Set the initial command string.
    */
    ctx.qnmbr = ctx.qpntr;
    qset(0, "MY", 2);

    /*
    ** Load the command line into Q-reg Z.
    */
    qref(0, 'Z');
    io_support.getcmd();

    for (;;) {
	setcmd(ctx.qpntr);
	if (ctx.qpntr->qrg_size > 0) {
	    ctx.qlcmd = ctx.qpntr->qrg_size;
	    status = set_unwind();
	    if (status == TECO__NORMAL)
		teco_interp();

	    poplcl(ctx.lclptr);
	    while (ctx.pdl != 0) {
		pop(ctx.pdl->type);
		poplcl(ctx.lclptr);
	    }
	    ctx.qpntr->qrg_size = 0;
	}

	if (!ctx.indir) {
	    if (docrlfno) {
		docrlfno = 0;
		crlfno();
	    }

	    if (ctx.flags2 & TECO_M_EXITEND) break;
	    ctx.etype &= ~TECO_M_ET_XIT;

	    if (!(ctx.edit & TECO_M_ED_WCH)) {
		// do WATCH update...
	    }

	    if (ctx.evflag > 0) {
		// do verify
	    }

	    type('*');
	}

	clrend();

	ctx.quote = TECO_C_ESC;
	ctx.temp = 0;
	ctx.qnmbr = ctx.qpntr;

	more = storechr = 1;
	while (more) {
	    if ((chr = listen(!ctx.qpntr->qrg_size)) == TECO_C_ESC) {
		if (ctx.temp == TECO_C_ESC) {
		    more = 0;
		    if (!ctx.indir) crlfno();
		}
	    } else if (!ctx.indir) {
		switch (chr) {
		default:
		    break;

		case TECO_C_BEL:
		    if (ctx.temp == TECO_C_BEL) {
			ctx.qlcmd = ctx.qpntr->qrg_size - 1;
			ctx.qpntr->qrg_size = 0;

			docrlfno = 1;
			more = storechr = 0;
		    }
		    break;

		case '/':
		    if (ctx.errpos) {
			docrlfno = 1;
			more = storechr = 0;

			// write out help here...
		    }

		case '*':
		    if (ctx.temp == TECO_C_BEL) {
			storechr = 0;
			if (--ctx.qpntr->qrg_size == 0) {
			    docrlfno = 1;
			    more = 0;
			} else {
			    crlfno();
			    print(ctx.qpntr->qrg_ptr, ctx.qpntr->qrg_size);
			}
		    }
		    break;

		case '?':
		    if (ctx.errpos) {
			print(ctx.errptr, ctx.errpos);
			type('?');
			docrlfno = 1;
			storechr = more = 0;
		    }
		    break;

		case TECO_C_DEL:
		    storechr = 0;

		    if (ctx.qpntr->qrg_size == 0) {
			docrlfno = 1;
			more = 0;
		    }

		    teco_delch();
		    break;
		}
	    }

	    if (storechr) {
		qset(1, &chr, 1);

		// store using qset?
		// ctx.qpntr->qrg_ptr[ctx.qpntr->qrg_size++] = chr;
		// check for overflow and last ten characters...

		ctx.errpos = ctx.qlcmd = 0;
	    } else {
		storechr = 1;
	    }

	    ctx.temp = chr;
	}
    }

    // reTODO: what kind of status should we be returning here?
    return TECO__NORMAL;
}

int32_t teco_init(void)
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

    ctx.etype |= TECO_M_ET_XIT;
    ctx.symspc = '$'; // need to get this from OS-specific stuff, maybe init routine...

    memset(qreg_array, 0, sizeof(qreg_array));
    memset(qreg_local, 0, sizeof(qreg_local));
    ctx.lclptr = qreg_local;
    ctx.qarray = qreg_array;
    ctx.qpntr = &ctx.qarray[TECO_K_QRG_FAKE];

    io_support.init();

    memset(&scope, 0, sizeof(SCOPEDEF));

    ctx.quote = TECO_C_ESC;

    return TECO__NORMAL;
}

static void errors(signum)
    int signum;
{
    if (ctx.errcod == TECO__NORMAL)
        return;

    if (ctx.errcod < TECO__NORMAL) {
        ctx.errptr = ctx.qcmnd->qrg_ptr;
        ctx.errpos = ctx.scanp - ctx.errptr;
    }

    teco_putmsg();

    if (ctx.errcod < TECO__NORMAL) {
        crlfno();
        goto_unwind();
    }
}

void teco_interp(void)
{
    uint8_t chr, *end = ctx.scanp + ctx.qlengt;

#if 0
    chr = scan();
    do {
#else
    while (ctx.scanp < end) {
	chr = scan();
#endif
	switch (chr) {
	default:
	    if ((int8_t)chr < 0) {
		/* Is an 8-bit character... */
		ERROR_MESSAGE(ILL);
	    }
	    ERROR_MESSAGE(NYI);
	    break;

    	case TECO_C_BEL:
    	case TECO_C_VT:
    	case TECO_C_DLE:
    	case TECO_C_FS:
    	case TECO_C_GS:
    	case '$':
    	case '}':
    	case '{':
    	case '~':		/* Illegal command characters */
    	    ERROR_MESSAGE(ILL);
    	    break;

    	case TECO_C_NUL:
    	case TECO_C_LF:
    	case TECO_C_CR:
    	case TECO_C_SPACE:
    	case TECO_C_DEL:	/* Non-command characters */
    	    break;

	case TECO_C_SOH: {	/* "^A" is output message to terminal */
	    int len;

	    ctx.flags &= ~TECO_M_NFLG;
	    ctx.quote = TECO_C_SOH;
	    skpquo();

	    len = (ctx.scanp - ctx.oscanp) -1;

	    if (len != 0)
		prinb(ctx.oscanp, len);

	    break;
	}

	case '\002': {		/* "CTRL/B" is today's date */
	    time_t now;
	    struct tm *tm;

	    now = time(0);
	    tm = localtime(&now);
	    ncom(((tm->tm_year-1900)*16+tm->tm_mon)*32+tm->tm_mday);
	    break;
	}

	case '\003':		/* "CTRL/C" is exit from macro/teco */
	    if (tstnxt(chr) || (ctx.mpdcnt == 0)) exit(TECO__NORMAL);
	    ctx.errcod = 1;
	    goto_unwind();
	    break;

	case '\004':		/* "CTRL/D" means decimal radix */
	    ctx.nmrbas = RADIX_DEC;
	    break;

	case '\005': {		/* "CTRL/E" is form feed flag */
	    uint32_t n = ctx.flags2 & TECO_M_FFFLAG;

	    if (ctx.flags & TECO_M_NFLG) {
		ctx.flags &= ~TECO_M_NFLG;
		if (n != 0)
		    ctx.flags2 &= ~TECO_M_FFFLAG;
		else
		    ctx.flags2 |= TECO_M_FFFLAG;
	    }
	    ncom(-n);
	    break;
	}

	case '\006':		/* "CTRL/F" is switch register value */
	    ncom(0);
	    break;

	case '\010': {		/* "CTRL/H" is time of day. */
	    time_t now;
	    struct tm *tm;

	    now = time(0);
	    tm = localtime(&now);
	    ncom(((((tm->tm_hour*60)+tm->tm_min)*60)+tm->tm_sec)/2);
	    break;
	}

	case '\t': {		/* "TAB" is special form if "i" */
	    uint32_t len;
	    uint8_t saved;

	    /*
	    ** Fetch quoted string and drop back to catch the TAB.
	    */
	    skpquo();
	    ctx.oscanp--;

	    /*
	    ** Ensure that we are dropping back to a quote as it is acceptable
	    ** to execute this command as "@^i%text%".
	    */
	    saved = *ctx.oscanp;
	    *ctx.oscanp = '\t';

	    len = (ctx.scanp - ctx.oscanp) - 1;
	    txadj(len);
	    memcpy(&ctx.txstor[ctx.p+ctx.lschsz], ctx.oscanp, len);
	    *ctx.oscanp = saved;
	    break;
	}

	case '\016':		/* "CTRL/N" is eof flag */
	    ncom(ctx.flags2 & TECO_M_EOFLAG ? TECO__TRUE : TECO__FALSE);
	    break;

	case '\017':		/* "CTRL/O" means octal radix */
	    ctx.nmrbas = RADIX_OCT;
	    break;

	case '\021': {		/* "^Q" converts 'L' commands to 'C' commands */
	    uint32_t p = ctx.p;

	    nlines();
	    ncom(ctx.p - p);

	    ctx.p = p;
	    break;
	}

	case '\022':		/* "^R" read/set radix flag */
	    if (ctx.flags & TECO_M_NFLG) {
		ctx.flags &= ~TECO_M_NFLG;

		switch(ctx.n) {
		default: ERROR_MESSAGE(IRA);
		case  8: ctx.nmrbas = RADIX_OCT; break;
		case 10: ctx.nmrbas = RADIX_DEC; break;
		case 16: ctx.nmrbas = RADIX_HEX; break;
		}
	    } else {
		switch (ctx.nmrbas) {
		case RADIX_DEC: ncom(10); break;
		case RADIX_OCT: ncom(8);  break;
		case RADIX_HEX: ncom(16); break;
		}
	    }
	    break;

	case '\023':		/* "CTRL/S" is -(length) of last string */
	    ncom(ctx.lschsz);
	    break;

	case '\024':		/* "CTRL/T" means value of next input
					character */
	    /*
	    ** Is there an argument?
	    */
	    if (ctx.flags & TECO_M_NFLG) {
		uint32_t etype = ctx.etype;

		/*
		** Yes.  Type out character whose ASCII value is 'n'.
		*/
		ctx.flags &= ~TECO_M_NFLG;
		if (ctx.flags & TECO_M_CLNF)
		    ctx.etype |= TECO_M_ET_BIN;

		typb(ctx.n);

		ctx.etype = etype;
		ctx.flags &= ~(TECO_M_CLNF | TECO_M_CLN2F);
	    } else {
		/*
		** No.  Character input from terminal.
		*/
		if (ctx.flags & TECO_M_CLNF) {
		    uint32_t code;

		    ctx.flags &= ~TECO_M_CLNF;

		    code = tlistn();
		    ctx.m = decode(&code);
		    ctx.flags != TECO_M_CFLG;
		    ncom(code);
		} else {
		    ncom(tlistn());
		}
	    }
	    break;

    	case '\025': {	    	/* "CTRL/U" is q-reg text insert */
    	    uint8_t append = 0, chr, *ptr;
    	    intmax_t len;

    	    qref(0, scnupp());
    	    skpquo();

    	    ptr = ctx.oscanp;
    	    len = (ctx.scanp - ctx.oscanp) - 1;

    	    if (ctx.flags & TECO_M_NFLG) {
    	    	if (len == 0)
    	    	    ERROR_MESSAGE(IIA);
    	    	chr = (uint8_t) ctx.n;
    	    	ptr = &chr;
    	    	len = 1;
    	    }

    	    if (ctx.flags & TECO_M_CLNF)
    	    	append = 1;

    	    qset(append, ptr, len);

    	    ctx.flags &= ~(TECO_M_NFLG | TECO_M_CLNF);
    	    break;
    	}

	case '\037':		/* "CTRL/_" is the unary complement operator */
	    if (!(ctx.flags & TECO_M_NFLG))
		ERROR_MESSAGE(NAB);

	    ctx.n = ~ctx.n;
	    break;

	case TECO_C_FF:		/* "FF" is output form-feed */
	    typb(chr);
	    break;

	case TECO_C_ESC:	/* "ESC" simply eats everything */
	    if (tstnxt(chr)) {
    	    	/*
    	    	** We have reached the end of the command, skip to the end
    	    	** and take our leave...
     	    	*/
		skpset(TECO_C_NUL, TECO_C_NUL);
    	    	continue;
	    } else {
		ctx.flags &= ~(TECO_M_CFLG | TECO_M_OFLG |
			       TECO_M_CLNF | TECO_M_CLN2F |
			       TECO_M_NFLG);
		irest();
	    }
	    break;

	case '!':		/* "!" is the comment delimiter */
	    ctx.quote = chr;
	    skpquo();
	    break;

    	case '"': {		/* '"' is the conditional */
	    uint8_t cmd;
	    uint32_t success = 0;

    	    if (!(ctx.flags & TECO_M_NFLG))
    	    	ERROR_MESSAGE(NAQ);

    	    ctx.flags &= ~TECO_M_NFLG;

    	    cmd = scnupp();
    	    switch (cmd) {
    	    default:
    	    	ERROR_MESSAGE(IQC);
    	    	break;

    	    case 'A':		/* "A" is a-z */
    	    	success = isalpha(ctx.n);
    	    	break;

    	    case 'D':		/* "D" is 0-9 */
    	    	success = isdigit(ctx.n);
    	    	break;

    	    case 'E':		/* "E" is ok if = */
    	    case 'F':		/* "F" is false (0) */
    	    case 'U':		/* "U" is unsuccessful (0) */
    	    case '=':		/* "=" is ok if = */
    	    	success = ctx.n == 0;
    	    	break;

    	    case 'G':		/* "G" is ok if > */
    	    case '>':		/* ">" is ok if > */
    	    	success = ctx.n > 0;
    	    	break;

    	    case 'L':		/* "L" is ok if < */
    	    case 'S':		/* "S" is successful (-1) */
    	    case 'T':		/* "T" is true (-1) */
    	    case '<':		/* "<" is ok if < */
    	    	success = ctx.n < 0;
    	    	break;

    	    case 'N':		/* "N" is okay if <> */
    	    	success = ctx.n != 0;
    	    	break;

    	    case 'C':		/* "C" is symbol character */
    	    case 'R':		/* "R" is a-z, 0-9 */
    	    	success = isalnum(ctx.n);
    	    	break;

    	    case 'V':		/* "V" is lower case a-z */
     	    	success = islower(ctx.n);
    	    	break;

    	    case 'W':		/* "W" is upper case a-z */
    	    	success = isupper(ctx.n);
    	    	break;
    	    }

    	    if (!success) {
    	    	/*
    	    	** Conditional failed.  Attempt to find "|" (else).  If
    	    	** not found, just jump to "'" (end if);
    	    	*/
    	    	ctx.cndn = 0;
    	    	do {
    	    	    skpset(TECO_C_VBR, TECO_C_APS);
    	    	} while (ctx.cndn != 0);
    	    }
    	    break;
    	}

    	case '\'':		/* "'" is the end of a conditional */
    	    ctx.flags &= ~TECO_M_NFLG;
    	    irest();
    	    break;

    	case '|':		/* "vertical bar" is skip to next ' */
    	    ctx.cndn = 0;
    	    do {
     	    	skpset(TECO_C_APS, TECO_C_NUL);
    	    } while (ctx.cndn != 0);
    	    break;

    	case '<':		/* "<" starts an interation */
    	    push(TECO_K_PDL_ITR);
    	    ctx.itrst = ctx.scanp;
    	    if (ctx.flags & TECO_M_NFLG) {
    	    	ctx.flags &= ~TECO_M_NFLG;
    	    	ctx.itrcnt = ctx.n;
    	    	if (ctx.n <= 0) {
		    skpset(TECO_C_RAB, TECO_C_NUL);
		    pop(TECO_K_PDL_ITR);
    	     	}
    	    } else {
    	    	ctx.itrcnt = 0;
    	    }
    	    break;

    	case '>':		/* ">" ends an iteration */
    	    if (ctx.itrst == 0)
    	    	ERROR_MESSAGE(BNI);

    	    if (ctx.itrcnt != 0) {
    	    	if (--ctx.itrcnt == 0) {
    	    	    pop(TECO_K_PDL_ITR);
    	    	} else {
    	    	    ctx.scanp = ctx.itrst;
    	    	}
    	    } else {
    	    	ctx.scanp = ctx.itrst;
    	    }

    	    ctx.flags &= ~(TECO_M_CFLG | TECO_M_OFLG | TECO_M_NFLG |
    	    	    	   TECO_M_CLNF | TECO_M_CLN2F);
    	    irest();
    	    break;

	case ';':		/* ";" is special iteration end */
	    if (!ctx.itrst)
		ERROR_MESSAGE(SNI);
	    if (!(ctx.flags & TECO_M_NFLG))
		ERROR_MESSAGE(NAS);
	    if (ctx.flags & TECO_M_CLNF)
		ctx.n = -ctx.n;
	    if (ctx.n < 0) {
		ctx.flags &= ~(TECO_M_CFLG  | TECO_M_OFLG | TECO_M_CLNF |
			       TECO_M_CLN2F | TECO_M_NFLG);
		irest();
	    } else {
		skpset(TECO_C_RAB, TECO_C_NUL);
		pop(TECO_K_PDL_ITR);
	    }
	    break;

	case '&':		/* "&" is logical 'and' */
	case '#':		/* "#" is logical or */
	case '*':		/* "*" is multiplication */
	case '+':		/* "+" is addition */
	case '-':		/* "-" is subtraction */
	case '/':		/* "/" is division */
	    switch (chr) {
	    case '&': ctx.nopr = OP_AND; break;
	    case '#': ctx.nopr = OP_OR;  break;
	    case '/': ctx.nopr = OP_DIV; break;
	    case '*': ctx.nopr = OP_MUL; break;
	    case '-': ctx.nopr = OP_SUB; break;
	    default:  ctx.nopr = OP_ADD; break;
	    }

	    /*
    	    ** Save any current number in accumulator and clear
    	    ** pending numbers.
	    */
	    ctx.nacc = ctx.n;
	    ctx.np = 0;

	    /*
    	    ** Indicate operator pending, but no number pending.
    	    ** Actual computation is handled by ncom.
	    */
	    ctx.flags &= ~TECO_M_NFLG;
	    ctx.flags |= TECO_M_OFLG;
	    break;

	case '%':		/* "%" is add to q-reg value */
	    getn();
	    qref(0, scnupp());
	    ctx.qnmbr->qrg_value += ctx.n;
	    ncom(ctx.qnmbr->qrg_value);
	    break;

	case '(':		/* "(" is start of new expression */
	    if (!(ctx.flags & TECO_M_OFLG)) {
		ncom(0);
		ctx.flags |= TECO_M_OFLG;
	    }
	    ctx.pcnt++;
	    push(TECO_K_PDL_PAREN);

	    ctx.nopr = OP_ADD;
	    ctx.nacc = ctx.n;

	    ctx.flags &= ~TECO_M_NFLG;
	    break;

	case ')':		/* ")" is end of expression */
	    if ((!ctx.flags & TECO_M_NFLG) || !ctx.pcnt)
		ERROR_MESSAGE(NAP);

	    ctx.pcnt--;
	    pop(TECO_K_PDL_PAREN);

    	    ctx.flags |= TECO_M_OFLG;
	    ncom(ctx.n);
	    break;

	case ',':		/* "," is the m,n separator */
	    if (!(ctx.flags & TECO_M_NFLG))
		ERROR_MESSAGE(NAC);

	    ctx.m = ctx.n;
	    ctx.n = 0;

	    ctx.flags |= TECO_M_CFLG;
	    ctx.flags &= ~TECO_M_NFLG;

	    break;

	case '.':		/* "." is current position */
	    ncom(ctx.p);
	    break;

	case '9':
	case '8':
	    if (ctx.nmrbas == RADIX_OCT)
		ERROR_MESSAGE(ILN);
	case '7':
	case '6':
	case '5':
	case '4':
	case '3':
	case '2':
	case '1':
	case '0': {
	    intmax_t np = ctx.np, i = chr - '0';

	    /*
	    ** Correct a hexidecimal digit.
	    */
	    if (i > 9)
		i -= ('A' - 10) - '0';

    	    if (ctx.flags & TECO_M_NFLG) {
    	    	ctx.flags &= ~TECO_M_NFLG;

	    	ctx.np *= 4;
	    	switch (ctx.nmrbas) {
	    	case RADIX_HEX: ctx.np += ctx.np;
	    	case RADIX_OCT: np = 0;
	    	case RADIX_DEC: ctx.np += np;
	    	}

	    	/*
    	    	** Save np, so it can be restored following ncom and flip
    	    	** operator flag so ncom correctly calculates accumulated
		** numbers.
	        */
	    	np = (ctx.np * 2) + i;
		ctx.flags |= TECO_M_OFLG;
		ncom(np);
		ctx.np = np;
	    } else {
	    	ncom(i);
	    	ctx.np = i;
	    }
	    break;
	}

	case ':':
	    ctx.flags |= TECO_M_CLNF;

	    if (tstnxt(chr)) {
		ctx.flags |= TECO_M_CLN2F;
		ctx.flags &= ~TECO_M_CLNF;
	    }
	    break;

	case '=': {		/* "=" is the number printer */
	    uint8_t nmrbas = ctx.nmrbas;

	    if (!(ctx.flags & TECO_M_NFLG)) ERROR_MESSAGE(NAE);
	    ctx.flags &= ~TECO_M_NFLG;
	    for (ctx.nmrbas = 0; tstnxt(chr) &&
				 (ctx.nmrbas <= RADIX_MAX); ctx.nmrbas++)
		;
	    zerod(TECO_K_ZEROD_TERM);
	    ctx.nmrbas = nmrbas;
	    if (ctx.flags & TECO_M_CLNF)
		ctx.flags &= ~TECO_M_CLNF;
	    else
		crlfno();
	    break;
	}

	case '@':		/* "@" is quote flag setter */
	    ctx.flags |= TECO_M_QFLG;
	    break;

	case 'A':
	case 'a':		/* "A" is append */
	    if (ctx.flags & TECO_M_CLNF) {
		// 30$: -- line 1833
	    } else if (!ctx.flags & TECO_M_NFLG) {
		// 40$:
	    } else {
		/*
		** Return the ASCII code of the n'th character from .
		*/
		int32_t p = ctx.p + ctx.n;
		ncom((p >= 0) && (p <= ctx.zz) ? ctx.txstor[p] : -1);
	    }
	    break;

	case 'B':
	case 'b':		/* "B" is 0 - beginning of buffer */
	    ncom(0);
	    break;

	case 'C':
	case 'c':		/* "C" is move pointer characters */
	    getn();
	    ctx.p += ctx.n;
	    bzchk(ctx.p);
	    break;

	case 'D':
	case 'd':		/* "D" is delete characters */
	    if (ctx.flags & TECO_M_CFLG) {
		gettx();
		ctx.p = ctx.m;
		txadj(ctx.n * -1);
	    } else {
		getn();
	    	if (ctx.n < 0) {
		    bzchk(ctx.n);
		    ctx.p += ctx.n;
	    	} else {
		    ctx.n *= -1;
		}
	    	txadj(ctx.n);
	    }
	    break;

	case 'E':
	case 'e': {		/* "E" is special commands */
	    uint8_t cmd = scnupp();
	    uint32_t *flag = 0;

	    switch (cmd) {
	    case 'D':		/* "ED" is editor level */
		flag = &ctx.edit;
		break;

	    case 'E':		/* "EE" is delim flag */
		if (ctx.flags & TECO_M_NFLG) {
		    ctx.flags &= ~TECO_M_NFLG;
		    ctx.etype |= TECO_M_ET_GRV;
		    ctx.eeflg = ctx.n;
		} else {
		    ncom(ctx.eeflg);
		}
		break;

	    case 'G':		/* "EG" is process special function */
		getstg(&ctx.filbuf);
		ncom(io_support.gexit());
		break;

	    case 'H':		/* "EH" is edit help level */
		flag = &ctx.ehelp;
		break;

	    case 'B':
	    case 'I':
	    case 'N':
	    case 'R':
	    case 'W':
		getstg(&ctx.filbuf);
		irest();
		ctx.temp = ctx.n;
		io_support.getfl(cmd);
		break;

	    case 'J': {		/* "EJ" is return environment characteristics */
		int n;

		switch (ctx.n) {
		case -1: n = 256 * (TECO_K_ARCH + TECO_K_OS); break;
		default: n = io_support.ejflg(ctx.n);
		}

		ctx.n = n;
		break;
	    }

	    case 'O':		/* "EO" is version of TECO */
		ncom(TECO_K_VERSION);
		break;

	    case 'S':		/* "ES" is edit search flag */
		flag = &ctx.esflag;
		break;

	    case 'T':		/* "ET" is edit typeout flag */
		flag = &ctx.etype;
		break;

	    case 'U':		/* "EU" is case flagging flag */
		flag = &ctx.euflag;
		break;

	    case 'V':		/* "EV" is edit verify flag */
		flag = &ctx.evflag;
		break;

	    default:
		ERROR_MESSAGE(IEC);

	    }

	    /*
	    ** Handle common flag processing, if we are operating on one.
	    */
	    if (flag) {
		int n = 0;

		if (ctx.flags & TECO_M_NFLG) {
		    n = ctx.n;

		    if (ctx.flags & TECO_M_CFLG) {
			ctx.flags &= ~TECO_M_CFLG;

			n |= *flag;
			n &= ~ctx.m;
		    }
		}

		// if cmd = 'T'
		    // then we need to call io_support.etflg();
		    //  this code will adjust the terminal, etc.

		if (ctx.flags & TECO_M_NFLG) {
		    ctx.n = n; // should be result from flagrw?
		} else {
		    ncom(n); // should be result from flagrw?
		}
	    }

	    break;
	}

    	case 'F':
    	case 'f': {		/* "F" is prefix for special searches */
    	    uint8_t cmd = scnupp();

    	    switch (cmd) {
    	    default:
    	    	ERROR_MESSAGE(IFC);
    	    	break;

    	    case '<':		/* "F<" flows to iteration's start */
    	    	if (ctx.itrst == 0) {
    	    	    ctx.scanp = ctx.qcmnd->qrg_ptr;
    	    	} else {
    	    	    ctx.scanp = ctx.itrst;
    	    	}
    	    	break;

    	    case '>':		/* "F>" flows to iteration's end */
    	    	if (ctx.itrst != 0) {
    	    	    if (--ctx.itrcnt == 0) {
    	    	   	skpset(TECO_C_RAB, TECO_C_NUL);
     	    	    	trace(TECO_C_RAB);
    	    	    } else {
    	    	    	ctx.scanp = ctx.itrst;
    	    	    	ctx.flags &= ~(TECO_M_CFLG | TECO_M_OFLG  |
    	    	    	    	       TECO_M_CLNF | TECO_M_CLN2F |
   	    	    	    	       TECO_M_NFLG);
    	    	    	irest();
    	    	    }
    	    	} else {
    	    	    skpset(TECO_C_NUL, TECO_C_NUL);
    	    	}
    	    }
    	    break;
    	}

	case 'G':
	case 'g':		/* "G" is get q-reg into text */
	    ctx.flags &= ~TECO_M_NFLG;
	    qref(1, scnupp());
	    if (ctx.flags & TECO_M_CLNF) {
		ctx.flags &= ~TECO_M_CLNF;
		prinf(ctx.qnmbr->qrg_ptr, ctx.qnmbr->qrg_size);
	    } else {
		txadj(ctx.qnmbr->qrg_size);
		memcpy(&ctx.txstor[ctx.p+ctx.lschsz], ctx.qnmbr->qrg_ptr,
		       ctx.qnmbr->qrg_size);
	    }
	    break;

	case 'H':
	case 'h':		/* "H" is equivalent to 'B,Z' */
	    ctx.m = 0;
	    ctx.n = ctx.zz;
	    ctx.flags |= TECO_M_CFLG | TECO_M_NFLG;
	    break;

    	case 'I':
    	case 'i': {		/* "I" is insert text */
    	    uint8_t *ptr;
    	    uint32_t len;

    	    skpquo();
    	    ptr = ctx.oscanp;
    	    len = (ctx.scanp - ctx.oscanp) -1;

    	    if (ctx.flags & TECO_M_NFLG) {
    	    	ctx.flags &= ~TECO_M_NFLG;

    	    	if (len != 0) ERROR_MESSAGE(IAA);

    	    	ptr = (uint8_t *)&ctx.n;
    	    	len = 1;
    	    }

    	    txadj(len);
    	    memcpy(&ctx.txstor[ctx.p+ctx.lschsz], ptr, len);
    	    break;
    	}

	case 'J':		/* "J" is move pointer */
	case 'j':
	    if (ctx.flags & TECO_M_NFLG) {
		ctx.flags &= ~TECO_M_NFLG;
		bzchk(ctx.n);
		ctx.p = ctx.n;
	    } else {
		ctx.p = 0;
	    }
	    break;

	case 'K':
	case 'k':		/* "K" is the line deleter */
	    gettx();
	    ctx.p = ctx.m;
	    txadj(-ctx.n);
	    break;

	case 'L':
	case 'l':		/* "L" is advance n lines */
	    nlines();
	    //if (ctx.n == 0) {
	    //	ctx.p = 0;
	    //}
	    break;

	case 'M':
	case 'm':		/* "M" is the macro command */
	    qref(0, scnupp());
	    push(TECO_K_PDL_MACRO);

	    if (!(ctx.flags & TECO_M_CLNF)
		&& !(ctx.qnmbr->qrg_flags & TECO_M_QRG_LOCAL)) {
		ctx.lclptr = 0;
	    } else {
		ctx.flags &= ~TECO_M_CLNF;
	    }

	    ctx.mpdcnt++;
	    ctx.itrst = 0;

	    setcmd(ctx.qnmbr);
	    teco_interp();
	    break;

	case 'O':
	case 'o': {		/* "O" is goto tag */
	    uint8_t *tag;
	    uint32_t taglen;

	    getstg(&ctx.tagbuf);
	    tag = ctx.tagbuf.qrg_ptr;
	    taglen = ctx.tagbuf.qrg_size;
	    if (ctx.flags & TECO_M_NFLG) {
		uint8_t *tend, *tp;

		ctx.flags &= ~TECO_M_NFLG;
		tp = tag;
		tend = tp + taglen;
		while (tp < tend) {
		    if (*tp == ',') {
			if (--ctx.n <= 0) {
			    taglen = tp - tag;
			    break;
			}
			tag = ++tp;
		    } else {
			tp++;
		    }
		}
		if (tp >= tend) tag = 0;
	    }
	    if (tag != 0) {
		ctx.scanp = ctx.itrst != 0 ? ctx.itrst : ctx.qcmnd->qrg_ptr;
		do {
		    skpset('!', TECO_C_NUL);
		    trace('!');
		    ctx.quote = '!';
		    skpquo();
		} while (!memcmp(tag, ctx.oscanp, (ctx.scanp-ctx.oscanp)-1));
	    }
	    irest();
	    break;
	}

	case 'Q':
	case 'q': {		/* "Q" is value/size in q-register */
	    int n = -1;

	    qref(0, scnupp());

	    if (ctx.flags & TECO_M_NFLG) {
		if ((ctx.n >= 0) && (ctx.n < ctx.qnmbr->qrg_size))
		    n = ctx.qnmbr->qrg_ptr[ctx.n];
	    } else if (ctx.flags & TECO_M_CFLG) {
		n = ctx.qnmbr->qrg_size;
	    } else {
		n = ctx.qnmbr->qrg_value;
	    }

	    ctx.flags &= ~(TECO_M_CLNF | TECO_M_NFLG);

	    ncom(n);

	    break;
	}

	case 'R':
	case 'r': {		/* "R" is move pointer characters */
	    int p;

	    getn();
	    p = ctx.p - ctx.n;
	    bzchk(p);
	    ctx.p = p;
	    break;
	}

    	case 'S':
    	case 's': {		/* "S" is search */
    	    uint32_t status;

    	    if (ctx.flags & TECO_M_REPFLG) {
    	    	ctx.flags &= ~TECO_M_REPFLG;
    	    	skpquo();
    	    	if (status & SUR_OK) {
    	    	    // do some jigger-pokery and call .sch.r();
    	    	}
    	    }
    	    irest();
    	    ncom(status & SUR_OK ? TECO__TRUE : TECO__FALSE);
            if (!(ctx.flags & (TECO_M_CLNF|TECO_M_CLN2F))) {
                if (ctx.itrst) {
                    uint32_t tflg = ctx.flags & TECO_M_TFLG;
                    ctx.flags &= ~TECO_M_TFLG;
                    if (tstnxt(';')) {
                        ctx.scanp--;
                        ctx.flags |= tflg;
                        ctx.flags &= ~(TECO_M_CLNF|TECO_M_CLN2F);
                        break;
                    } else if (tstnxt(':')) {
                        if (tstnxt(';')) {
                            ctx.scanp -= 2;
                            ctx.flags |= tflg;
                            ctx.flags &= ~(TECO_M_CLNF|TECO_M_CLN2F);
                            break;
                        } else {
                            ctx.scanp--;
                        }
                    }
                    ctx.flags |= tflg;
                    ctx.flags &= ~TECO_M_NFLG;
                    if (ctx.n == TECO__TRUE)
                        break;
                    /* TODO: crlfno(); */
                    ERROR_MESSAGE(SEAR_ITER);
                    chr =';';
                    continue;
                }
                ctx.flags &= ~TECO_M_NFLG;
                if (ctx.n >= 0) {
                    ERROR_MESSAGE(SRH);// ...need to supply the search string?
                }
                if (ctx.esflag == 0) {
                    ctx.flags &= ~(TECO_M_CLNF|TECO_M_CLN2F);
                    break;
                }
                ctx.flags &= ~(TECO_M_NFLG|TECO_M_CFLG);
                chr = 'V';
                continue;
            }
            ctx.flags &= ~(TECO_M_CLNF|TECO_M_CLN2F);
            break;
        }

	case 'T':
	case 't':		/* "T" is the printer */
	    gettx();
	    prinf(&ctx.txstor[ctx.m], ctx.n);
	    break;

	case 'U':
	case 'u':		/* "U" is q-reg number setter */
	    if (!(ctx.flags & TECO_M_NFLG))
		ERROR_MESSAGE(NAU);
	    ctx.flags &= ~TECO_M_NFLG;
	    qref(0, scnupp());
	    ctx.qnmbr->qrg_value = ctx.n;
	    if (ctx.flags & TECO_M_CFLG) {
		ctx.flags &= ~TECO_M_CFLG;
		ncom(ctx.m);
	    }
	    break;

	case 'X':
	case 'x':		/* "X" is q-reg text insert */
	    qref(0, scnupp());
	    gettx();
	    qset(ctx.flags & TECO_M_CLNF, &ctx.txstor[ctx.m], ctx.n);
	    ctx.flags &= ~TECO_M_CLNF;
	    break;

	case 'Z':
	case 'z':		/* "Z" is end of buffer value */
	    ncom(ctx.zz);
	    break;

	case '[':{		/* "[" is q-reg push */
	    QRGDEF *qpdl;

	    chkclo();
	    qref(0, scnupp());

	    qpdl = malloc(sizeof(QRGDEF));
	    if (!qpdl) {
		if (ctx.flags & TECO_M_NFLG) {
		    ctx.n = 0;
		} else {
		    ERROR_MESSAGE(PDO);
		}
	    } else {
		memcpy(qpdl, ctx.qnmbr, sizeof(QRGDEF));
		qpdl->qrg_flags |= TECO_M_QRG_REF;
		qpdl->qrg_next = ctx.qpdl;
		ctx.qpdl = qpdl;
	    }

	    break;
	}

	case '\\':		/* "\" is number inserter/getter */
	    ctx.lschsz = 0;
	    if (ctx.flags & TECO_M_NFLG) {
		ctx.flags &= ~TECO_M_NFLG;
		zerod(TECO_K_ZEROD_TXBUF);
	    } else {
    	    	intmax_t np = 0;

    	    	ncom(0);
    	    	if (ctx.p >= ctx.zz) break;
		if (ctx.txstor[ctx.p] == '-') {
		    ctx.nopr = OP_SUB;
		    ctx.p++;
    	    	    ctx.lschsz--;
    	    	} else {
    	    	    ctx.nopr = OP_ADD;
		    if (ctx.txstor[ctx.p] == '+') {
    	    	    	ctx.p++;
    	    	    	ctx.lschsz--;
    	    	    }
		}

		while (ctx.p < ctx.zz) {
    	    	    uint8_t c = ctx.txstor[ctx.p];
    	    	    intmax_t i;

    	    	    if (ctx.nmrbas == RADIX_HEX) {
    	    	        c = toupper(c);
    	    	    	if ((c <= 'A') && (c >= 'F')) {
    	    	    	    if (!isdigit(c)) break;
   	   	    	}
    	    	    } else {
    	    	    	if (!isdigit(c)) break;
    	    	    	if ((ctx.nmrbas == RADIX_OCT) && (c >= '8')) {
    	    	    	    break;
    	    	    	}
    	    	    }

    	    	    i = c - '0';
    	    	    if (i > 9)
    	    	    	i -= ('A' - 10) - '0';

	    	    ctx.np *= 4;
	    	    switch (ctx.nmrbas) {
	    	    case RADIX_HEX: ctx.np += ctx.np;
	    	    case RADIX_OCT: np = 0;
	    	    case RADIX_DEC: ctx.np += np;
    	    	    }
	    	    np = (ctx.np * 2) + i;
		    ctx.flags |= TECO_M_OFLG;
		    ncom(np);
		    ctx.np = np;

    	    	    ctx.p++;
    	    	    ctx.lschsz--;
    	    	}
#if 0
		if (ctx.p < ctx.zz) {
		    if (ctx.txstor[ctx.p] == '-') {
			nopr = OP_SUB;
			ctx.p++;
		    } else if (ctx.txstor[ctx.p] == '+') {
			ctx.p++;
		    }
		}

		while (ctx.p < ctx.zz) {
		    np = n;
		    n = ctx.txstor[ctx.p] - '0';
		    if (n > 9) {
			n -= ('A' - 10) -'0';
    	    	    } else {
    	    	    	if (!isdigit(ctx.txstor[ctx.p]))
    	    	    	    break;
    	    	    }
            	    radadj = np * 4;
		    if (ctx.nmrbas == RADIX_HEX) {
			radadj += radadj;
		    } else if (ctx.nmrbas == RADIX_DEC) {
			radadj += np;
		    }
		    n += radadj * 2;
		    ctx.p++;
		}

		ctx.lschsz = p - ctx.p;

		ncom(nopr == OP_SUB ? -n : n);
#endif
	    }
	    break;

	case ']': {		/* "]" is q-reg pop */
	    QRGDEF *qpdl = ctx.qpdl;

	    chkclo();
	    qref(0,scnupp());

	    if (!qpdl) {
		if (ctx.flags & TECO_M_NFLG) {
		    ctx.n = 0;
		} else {
		    ERROR_MESSAGE(CPQ);
		}
	    } else {
		ctx.qpdl = qpdl->qrg_next;

		// do matching...etc...
	    }

	    break;
	}

	case '^':		/* "^" means next is control character */
	    ctx.flags2 |= TECO_M_MAKCTL;
	    break;
	}

	if (!(ctx.flags & TECO_M_NFLG))
	    ctx.n = 0;

#if 0
	chr = scan();
    } while (ctx.scanp < end);
#else
    }
#endif

    if (ctx.mpdcnt != 0) {
	QRGDEF *lclptr = ctx.lclptr;

	pop(TECO_K_PDL_MACRO);

	if (lclptr && (lclptr != ctx.lclptr))
	    poplcl(lclptr);

	ctx.qlengt = ctx.qcmnd->qrg_size;
    } else {
	if (ctx.pdl != 0)
	    ERROR_MESSAGE(UTC);
    }
}


static uint8_t scan(void) {
    uint8_t chr;

    /*
    ** End of this command?
    */
    if ((ctx.scanp - ctx.qcmnd->qrg_ptr) >= ctx.qlengt) {
	/*
	** Yes, check for macro.
        */
	if (ctx.mpdcnt)
	    ERROR_MESSAGE(UTM);
	ERROR_MESSAGE(UTC);
    } else {
	/*
	** No.  Process the next character.
        */
	chr = *ctx.scanp++;
	trace(chr);

	if (ctx.flags2 & TECO_M_MAKCTL) {
	    ctx.flags2 &= ~TECO_M_MAKCTL;
	    chr = toupper(chr);
	    if ((chr < '@') || (chr > '_'))
		ERROR_MESSAGE(IUC);
	    chr &= ~0x40;
	}
    }

    return chr;
}


static uint32_t tstnxt(chr)
    uint8_t chr;
{
    if ((ctx.scanp - ctx.qcmnd->qrg_ptr) < ctx.qlengt) {
	if (toupper(*ctx.scanp) == chr) {
	    ctx.scanp++;
	    trace(*ctx.scanp);

	    return 1;
	}
    }

    return 0;
}

static void ncom(n)
    intmax_t n;
{
    ctx.np = 0;

    if (ctx.flags & TECO_M_OFLG) {
	switch (ctx.nopr) {
	case OP_DIV: ctx.n = n ? ctx.nacc / n : 0; break;
	case OP_MUL: ctx.n = ctx.nacc * n; break;
	case OP_AND: ctx.n = ctx.nacc & n; break;
	case OP_OR:  ctx.n = ctx.nacc | n; break;
	case OP_SUB: n = -n;
	default:     ctx.n = ctx.nacc + n; break;
	}
    } else {
	ctx.nacc = 0;
	ctx.nopr = OP_ADD;
	ctx.n = n;
    }

    ctx.flags |= TECO_M_NFLG;
    ctx.flags &= ~TECO_M_OFLG;
}

static void nlines(void)
{
    intmax_t n;
    uint32_t p;

    getn();

    n = ctx.n;
    if (n > 0) {
	/* Go forward 'n' lines.
	*/
	while ((ctx.p < ctx.zz) && (n > 0)) {
	    if ((ctx.txstor[ctx.p] >= TECO_C_LF)
    	    	&& (ctx.txstor[ctx.p] <= TECO_C_FF)) {
		n--;
    	    }
	    ctx.p++;
	}
    } else if (n <= 0) {
	/*
    	** Go back 'n' lines.
	*/
	while (ctx.p > 0) {
    	    ctx.p--;
	    if ((ctx.txstor[ctx.p] >= TECO_C_LF)
    	    	&& (ctx.txstor[ctx.p] <= TECO_C_FF)) {
		if (++n > 0) {
    	    	    ctx.p++;
    	    	    break;
    	    	}
	    }
	}
    }
}

static void skpquo(void) {
    getquo();

    ctx.oscanp = ctx.scanp;
    while (ctx.quote != scan())
	;

    resquo();
}

static void skpset(trm1,
		   trm2)
    const uint8_t trm1;
    const uint8_t trm2;
{
    uint8_t chr;
    uint32_t flags = ctx.flags;
    uint16_t flags2 = ctx.flags2;
    uint32_t itrdep = 0;

    irest();

    /* Check that if we're processing to end of string that we haven't
    ** run out of command buffer.
    */
    if ((trm1 == TECO_C_NUL) && (ctx.scanp >= (ctx.qcmnd->qrg_ptr + ctx.qcmnd->qrg_size)))
	return;

    ctx.flags &= ~TECO_M_TFLG;

    chr = scnupp();
    while ((chr != trm1) && (chr != trm2)) {
	/* Are we processing to end of string?
	*/
	if (trm1 == TECO_C_NUL) {
	    /* Yes.  Exit if we've reached the end.
	    */
	    if (ctx.scanp >= (ctx.qcmnd->qrg_ptr + ctx.qcmnd->qrg_size))
		break;
	} else {
	    switch (chr) {
	    default:
		break;

	    case TECO_C_SOH: 	/* SKIP QUOTED STRING USING CURRENT CHARACTER */
	    case '!':
		ctx.quote =chr;
		skpquo();
		break;

	    case '@':		/* SET THE SPECIAL QUOTED STRING FLAG */
		ctx.flags |= TECO_M_QFLG;
		break;

	    case '"':		/* INTO ONE MORE CONDITIONAL, SKIP ONE CHARACTER */
		chr = scnupp();
		ctx.cndn++;
		break;

	    case TECO_C_APS:	/* END A CONDITIONAL */
		ctx.cndn--;
		break;

	    case 'E':		/* PROCESS "E" COMMANDS */
		chr = scnupp();
		if ((chr == '_') || (chr == 'W') || (chr == 'R')
		 || (chr == 'N') || (chr == 'I') || (chr == 'G')
		 || (chr == 'B'))
		    skpquo();
		break;

	    case TECO_C_NAK:	/* SKIP Q-REG NAME (,QUOTED STRING) */
	    case 'G':
	    case 'M':
	    case 'Q':
	    case 'U':
	    case 'X':
	    case ']':
	    case '[':
		chr = scnupp();
		if (chr == '.')
		    chr = scnupp();

		/* If not processing a ^U then break here.  Otherwise continue
		** on to process the quoted string argument.
		*/
		if (chr != TECO_C_NAK)
		    break;

	    case TECO_C_TAB:	/* SKIP QUOTED STRING */
	    case 'I':
	    case 'N':
	    case 'O':
	    case 'S':
	    case '_':
		skpquo();
		break;

	    case '^':		/* RE-CHECK NEXT AS A CONTROL CHARACTER */
		ctx.flags2 |= TECO_M_MAKCTL;
		break;

	    case TECO_C_LAB:	/* SIGNAL START OF AN ITERATION */
		itrdep++;
		break;

	    case TECO_C_RAB:	/* SIGNAL END OF AN ITERATION */
		if (itrdep == 0)
		    ERROR_MESSAGE(BNI);

		itrdep--;
		break;
	    }
	}

	chr = scnupp();
    }

    /* Restore flags to original state.  However, make sure to save
    ** QFLG if set.
    */
    if (ctx.flags & TECO_M_QFLG)
	flags |= TECO_M_QFLG;
    ctx.flags = flags;
    ctx.flags2 = flags2;
}

static void gettx(void) {
    intmax_t n = 0;

    if (ctx.flags & TECO_M_CFLG) {
    	ctx.flags &= ~TECO_M_CFLG | TECO_M_NFLG;
    } else {
	ctx.m = ctx.p;
	nlines();
	ctx.n = ctx.p;
	ctx.p = ctx.m;
    }

    if (ctx.n < ctx.m) {
	n = ctx.n;
	ctx.n = ctx.m;
	ctx.m = n;
    }

    bzchk(ctx.n);

    ctx.n -= ctx.m;
}

static void getstg(out)
    QRGDEF *out;
{
    uint32_t insflg = 0;
    uint16_t inslen = 0;
    uint8_t *insptr = 0;
    uint8_t chr;
    int32_t upper, lower, skip = 0;

    out->qrg_size = 0;
    getquo();
    chr = scan();
    while (chr != ctx.quote) {
	inslen = 1;
	insptr = &chr;
	upper = lower = 0;

	if (skip) {
	    skip = 0;
	} else {
	    switch (chr) {
	    default:
		break;

	    case TECO_C_ENQ:		/* ^E */
		chr = scnupp();
		if ((chr == 'U') || (chr == 'Q')) {
		    qref();
		    if (chr == 'U') {
			insptr = (uint8_t *)&ctx.qnmbr->qrg_value;
		    } else {
			insptr = ctx.qnmbr->qrg_ptr;
			inslen = ctx.qnmbr->qrg_size;
		    }
		}
		break;

	    case TECO_C_DC1:		/* ^Q */
	    case TECO_C_DC2:		/* ^R */
		skip = 1;
		inslen = 0;
		break;

	    case TECO_C_SYN:		/* ^V */
		lower = 1;
		inslen = 0;
		break;

	    case TECO_C_ETB:		/* ^W */
		upper = 1;
		inslen = 0;
		break;

	    case '^':
		if (!(ctx.edit & TECO_M_ED_CTL)) {
		    ctx.flags2 |= TECO_M_MAKCTL;
		    inslen = 0;
		}
		break;
	    }
	}

	if (inslen != 0) {
	    ctx.qnmbr = out;
	    qset(insflg, insptr, inslen);
	    insflg = 1;
	}

	chr = scan();
	chr = upper ? toupper(chr) : lower ? tolower(chr) : chr;
    }

    resquo();
}

/*
** FUNCTIONAL DESCRIPTION:
**
**	Reference a Q-register.  The caller needs to supply the Q-reg
** number, usually through a call to scnupp.  However, if qrg_num is
** a '.' it will call scnupp itself for the next number.  Could create
** problems if improperly used.
**
**	NOTE: Candidate for a rewrite!
**
** SIDE EFFECTS:
**
**	If qrg_num is '.' then this routine will increase the command
** pointer.
*/
static void qref(additional,
		 _qrg_num)
    const uint32_t additional;
    const uint8_t _qrg_num;
{
    uint8_t qrg_flags = 0;
    uint8_t qrg_num = _qrg_num;
    QRGDEF *qarray;
    uint32_t qreg;

    /*
    ** Is the Q-register local?
    */
    if (qrg_num == '.') {
	/*
	** Yup.  Set Q-register array to local list.  Fetch
	** Q-register number, allocating the local Q-register
	** list if necessary.
	*/
	qrg_flags = TECO_M_QRG_LOCAL;
	if (!ctx.lclptr) {
	    ctx.lclptr = malloc(sizeof(QRGDEF) * TECO_K_NUMQRG);
	    if (!ctx.lclptr) ERROR_MESSAGE(MEM);
	    memset(ctx.lclptr, 0, sizeof(QRGDEF) * TECO_K_NUMQRG);
	}
	qarray = ctx.lclptr;
	qrg_num = scnupp();
    } else {
	/*
	** No.  Use global Q-registers.
	*/
	qarray = ctx.qarray;
    }

    /*
    ** Is Q-register number valid?
    */
    if (!isalnum(qrg_num)) {
	/*
	** No.  Could it be a "special" additional register?
	*/
	if (additional && !(qrg_flags & TECO_M_QRG_LOCAL)) {
	    /*
	    ** Yes.  Let's check...
	    */
	    switch (qrg_num) {
	    case '*': ctx.qnmbr = &ctx.filbuf; break;
	    case '_': ctx.qnmbr = &ctx.schbuf; break;
	    default: ERROR_MESSAGE(IQN); break;
	    }
	} else {
	    /*
	    ** No.  Sorry, must be either A-Z or 0-9.
	    */
	    ERROR_MESSAGE(IQN);
	}
    } else {
	/*
	** Calculate Q-register's location in its array.
	*/
	if (isdigit(qrg_num))
	    qreg = qrg_num - '0';
	else
	    qreg = qrg_num - ('9' - 1);
	ctx.qnmbr = &qarray[qreg];
    }

    /*
    ** Set Q-register name and scope.
    */
    ctx.qnmbr->qrg_num = qrg_num;
    ctx.qnmbr->qrg_flags = qrg_flags;
}

void txadj(size)
    const int32_t size;
{
    int32_t extent = size;
    uint8_t *txstor;

    if (extent < 0) {
	extent = -extent;

	if ((ctx.p + extent) > ctx.zz)
	    ERROR_MESSAGE(DTB);

	memcpy(&ctx.txstor[ctx.p], &ctx.txstor[ctx.p+extent],
	       ctx.zz - (ctx.p + extent));

	ctx.zz -= extent;
	ctx.curfre += extent;
    } else if (extent > 0) {
	if (extent > ctx.curfre) {
	    txstor = realloc(ctx.txstor, ctx.zmax + extent);
	    if (txstor == 0)
		ERROR_MESSAGE(MEM);

	    ctx.txstor = txstor;
	    ctx.zmax += extent;
	    ctx.curfre += extent;
	}

	ctx.zz += extent;
	ctx.curfre -= extent;

	memmove(&ctx.txstor[ctx.p], &ctx.txstor[ctx.p+extent],
    	    	ctx.zz - ctx.p);

	ctx.p += extent;
	ctx.lschsz = -extent;
    } else {
	ctx.lschsz = 0;
    }
}

void qset(append,
	  ptr,
	  len)
    const uint32_t append;
    const uint8_t *ptr;
    const uint16_t len;
{
    uint32_t buflen = append ? ctx.qnmbr->qrg_size + len : len;
    uint8_t *bufptr = ctx.qnmbr->qrg_ptr;
    uint16_t extent = TECO_K_QRG_EXTENT;

    if (buflen > USHRT_MAX)
	ERROR_MESSAGE(MEM);

    if (buflen > ctx.qnmbr->qrg_alloc) {
	extent += buflen;
	if (extent > USHRT_MAX)
	    extent = USHRT_MAX;

	bufptr = realloc(bufptr, extent);
	if (!bufptr)
	    ERROR_MESSAGE(MEM);

	ctx.qnmbr->qrg_alloc = extent;
	ctx.qnmbr->qrg_ptr = bufptr;
    }

    if (append)
	bufptr += ctx.qnmbr->qrg_size;

    ctx.qz -= ctx.qnmbr->qrg_size;
    ctx.qz += buflen;

    ctx.qnmbr->qrg_size = buflen;

    memmove(bufptr, ptr, len);
}

static void zerod(flags)
    uint32_t flags;
{
    char *format;
    char outbuf[33];
    int32_t outlen;

    if (ctx.nmrbas == RADIX_OCT)
	format = "%o";
    else if (ctx.nmrbas == RADIX_HEX)
	format = "%X";
    else
	format ="%d";

    outlen = snprintf(outbuf, sizeof(outbuf), format, ctx.n);
    if (flags == TECO_K_ZEROD_TXBUF) {
	txadj(outlen);
	memcpy(&ctx.txstor[ctx.p+ctx.lschsz], outbuf, outlen);
    } else {
	prinb(outbuf, outlen);
    }
}

static void push(type)
    uint32_t type;
{
    PDLDEF *pdl;

    if ((type != TECO_K_PDL_PAREN) && (ctx.pcnt >0))
	ERROR_MESSAGE(MRP);

    pdl = calloc(1, sizeof(PDLDEF));
    if (!pdl)
	ERROR_MESSAGE(MEM);

    switch (type) {
    case TECO_K_PDL_ITR:
	pdl->itrst = ctx.itrst;
	pdl->itrcnt = ctx.itrcnt;
	break;

    case TECO_K_PDL_MACRO:
	pdl->scanp = ctx.scanp;
	pdl->mpdcnt = ctx.mpdcnt;
	pdl->qcmnd = ctx.qcmnd;
	pdl->lclptr = ctx.lclptr;
	pdl->itrst = ctx.itrst;
	break;

    case TECO_K_PDL_PAREN:
	pdl->nopr = ctx.nopr;
	pdl->nacc = ctx.nacc;
	break;
    }

    pdl->next = ctx.pdl;
    pdl->type = type;
    ctx.pdl = pdl;
}

static void pop(type)
    uint32_t type;
{
    PDLDEF *pdl = ctx.pdl;

    if (!pdl) ERROR_MESSAGE(PDO);
    if (pdl->type != type) ERROR_MESSAGE(UTM);

    switch (type) {
    case TECO_K_PDL_ITR:
	ctx.itrst = pdl->itrst;
	ctx.itrcnt = pdl->itrcnt;
	break;

    case TECO_K_PDL_MACRO:
	ctx.scanp = pdl->scanp;
	ctx.mpdcnt = pdl->mpdcnt;
	ctx.qcmnd = pdl->qcmnd;
	ctx.lclptr = pdl->lclptr;
	ctx.itrst = pdl->itrst;
	break;

    case TECO_K_PDL_PAREN:
	ctx.nopr = pdl->nopr;
	ctx.nacc = pdl->nacc;
	break;
    }

    ctx.pdl = pdl->next;
    free(pdl);
}

static void poplcl(lclptr)
    QRGDEF *lclptr;
{
    uint32_t i;

    if (lclptr == 0) return;
    for (i = 0; i < TECO_K_NUMQRG; i++) {
	if (lclptr[i].qrg_alloc != 0)
	    free(lclptr[i].qrg_ptr);
    }
    if (lclptr != qreg_local) free(lclptr);
}

static uint32_t search()
{

    uint8_t *cp, *end;
    int32_t hits, limit, flags;

    if ((limit = ctx.m) < 0) {
        limit *= -1;
        flags |= SUR_REV;
    }
    if (ctx.flags & TECO_M_CFLG) {
        ctx.flags &= ~TECO_M_CFLG;
        flags |= SUR_BND;
    } else {
        limit = 0;
        /*
        ** Is this an old style 'no move' search??
        */
        if (ctx.flags & TECO_M_CLN2F) {
            /*
            ** Yup, set bound for no movement and set
            ** bound search flag.
            */
            limit++;
            flags |= SUR_BND;
        }
    }
    getn();
    getstg(&ctx.schbuf);
    if (ctx.n == 0) ERROR_MESSAGE(ISA);
    cp = ctx.p + ctx.txstor;            // R3
    end = ctx.zz + ctx.txstor;          // 8(SP)
    hits = ctx.n;                       // SP
#if 0
    if (hits < 0) {
        direction = -1;
        flags |= SUR_REV;
        if (!(flags & SUR_BND)) {
            flags |= SUR_NPG;
        }
        hits *= -1;
    } else {
        direction = 1;                  // 4(SP)
    }
    ctx.lschsz = 0;

    do {
        // sp = ctx.schbuf.arg_ptr;
        // send = sp + ctx.schbuf.qrg_size;
        // ctln = -1;
        while (cp < end) {
            if (++ctln != 0) {
                goto 120$;
            } else {
                if (cp
            }
        }
    } while (--hits != 0);

    // does it match end of string?
    flags |= SUR_FAIL;
    if (!(flags & SUR_BND)
    	&& !(ctx.edit & TECO_M_ED_SRH)) {
            r5 = 0;
            ctx.p = r5;
        }
    }
#endif
    return flags;


/*
    hits                //   (SP)
    dir                 //  4(SP)
    end                 //  8(SP)
    limit               // 12(SP)
    flags               // 16(SP)
*/
}

/*
This is the beginning of a new port of TECO-32.  I have come a long way
as a programmer since then and would like to do some things differently.

So, the main module will contain a giant switch, this catches the
different characters along the way.  No jump table.  Change some of the
error handling a coding style.  Use set/longjmp as before.  That
worked well.

Will probably need to use catopen, etc. for messages.  Base it on the
code on the VAX as that has the correct logic.  I spent plenty of time
studying the TECO-32 source code and don't want to waste that.


tecodef.h comes from the original source.  Retain it and comments in
the code too.  It is good to keep correclation with the original
source for bug fixes and deviations from the original intention.


*/


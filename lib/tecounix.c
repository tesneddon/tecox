/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains the UNIX specific code for TECO.
**
**  AUTHOR:             Tim E. Sneddon
**
**  Copyright (C) 2013 Tim E. Sneddon <tim@sneddon.id.au>
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
**      08-NOV-2011 V41.00  Sneddon	Initial coding.
**	15-DEC-2011 V41.01  Sneddon	Originaly termios.c, now covers
**					a broader range of support.
**	06-JUN-2013 V41.02  Sneddon	Re-arrange getcmd.
**	10-JUN-2013 V41.03  Sneddon	Add :EG support.
**--
*/
#define MODULE TECOUNIX
#define VERSION "V41.03"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "tecodef.h"
#include "tecomsg.h"
#include "globals.h"

/*
** Forward Declarations
*/

    static int32_t init();
    static int32_t restore();
    static int32_t input();
    static int32_t output();
    static int32_t ejflg();
    static int32_t etflg();
    static int32_t getcmd();
    static int32_t gexit();
    static void sigcont_handler();

/*
** Global Storage.
*/

    int argc = 0;
    char **argv = 0;
    IO_SUPPORT io_support = {
	init,
	restore,
	output,
	input,
	ejflg,
	etflg,
	getcmd,
	gexit,
    };

/*
** Static Storage.
*/

    static int ctrlz_cnt = 0;
    struct termios attr;
    struct termios pattr;

static int32_t init(void)
{
    struct sigaction action;
    int status;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigcont_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);

    sigaction(SIGCONT, &action, NULL);

    if (isatty(STDIN_FILENO)) {
	if (tcgetattr(STDIN_FILENO, &pattr) == 0) {
	    attr = pattr;

	    attr.c_lflag &= ~(ICANON | ECHO);
	    attr.c_iflag |= ISIG;
	    attr.c_iflag &= ~(IGNCR | ICRNL | INLCR);
#ifdef VDSUSP
	    attr.c_cc[VDSUSP] = _POSIX_VDISABLE;
#endif

	    /*
	    ** If UNIX terminal mode is enabled set interrupt as ^Z
	    ** else set to default TECO ^Y.
	    */
	    attr.c_cc[VSUSP] = (ctx.flags & TECO_M_ET_UNIX) ? TECO_C_SUB : TECO_C_EM;

	    if (tcsetattr(STDIN_FILENO, TCSANOW, &attr) == 0) {
		status = TECO__NORMAL;
	    } else {
		status = TECO__ERR;
		ctx.syserr = errno;
	    }
	}
    } else {
	status = TECO__ERR;
	ctx.syserr = errno;
    }

    ctx.etype |= TECO_M_ET_LC; // Should this be in crtrub?

    return (int32_t)status;
}

static int32_t restore(void)
{
    int status = TECO__NORMAL;

    fflush(stdout);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &pattr) != 0) {
	status = TECO__ERR;
	ctx.syserr = errno;
    }

    return status;
}

static int32_t input(chr)
    uint8_t *chr;
{
    int status, xitchr = TECO_C_SUB;

    if (ctx.temp == TECO_C_CR) {
	/*
	** All CRs get translated to CR/LFs, so return the LF portion.
	*/
	*chr = TECO_C_LF;
    } else {
	*chr = getc(stdin);
	if ((status = ferror(stdin)) == 0) {
	    /*
	    ** Modify immediate exit character if UNIX specific ^D
	    ** (instead of ^Z) support enabled.
	    */
	    if (ctx.etype & TECO_M_ET_UNIX)
		xitchr = TECO_C_EOT;

	    if (*chr == xitchr) {
		if (ctx.temp == xitchr)
		    ctrlz_cnt++;
		else
		    ctrlz_cnt = 1;

		if (ctrlz_cnt >= TECO_K_CTRLZ_MAX)
		    exit(TECO__NORMAL);
	    }

	    status = TECO__NORMAL;
	} else if (status == -1) {
	    status = TECO__ERR;
	    ctx.syserr = errno;
	} else {
	    status = TECO__ERR;
	    ctx.syserr = EIO;
	}
    }

    return (int32_t)status;
}

static int32_t output(chr)
    uint8_t chr;
{
    int status = TECO__NORMAL;

    if (putc(chr, stdout) == EOF) {
	status = TECO__ERR;
	ctx.syserr = EIO;
    }

    return (int32_t)status;
}

static int32_t ejflg(n)
    int32_t n;
{
    switch (n) {
    case  0: n = getpid(); break;
    case  1: n = 0; break;
    case  2: n = getuid(); break;
    default: n = getgid(); break;
    }

    return n;
}

static int32_t etflg(n)
    int32_t n;
{

// need to investigate what we do and what we return if we are not a
//  terminal...

    // if (isatty(STDIN_FILENO)) {
	// can we find out if we have been daemonized?
	// also we are most likely detached if stdin is not a tty...

    // check that we are not running in the background...

    // conn_check
	// retrieve (via call to termios) the current attr
	// extract the DETACH setting and
	//  then apply it to TECO_M_ET_DET in ctx.etype
	// we will need to maintain our own flag that is updated by
	// signal handler calls that catch SIGSTOP and SIGCONT

    // cancel ctrl/o?
	// yup, so set ctlofg
	// termios does support the concept of ^O.  However, neither Linux
	//  or POSIX support it (maybe HP-UX and Solaris?).  Need to implement
	//  the code anyway...setting/clearing ^O is done with FLUSHO
	//  in c_lflag...
    // retrieve current bits an xor them to get changes

    // fetch current terminal characteristics...

    if (n & TECO_M_ET_DET) {
	kill(getpid(), SIGSTOP);
	ctx.flags2 |= TECO_M_OUTDNE;
    }

    if (n & TECO_M_ET_TRU) {
	// call TECO_TRULN
	// set wrap
    }

    if (n & TECO_M_ET_8BT) {
	// enable 8 bit mode...
    }

/* TODO: just a dud status for the moment... */

    return TECO__NORMAL;
}

static int32_t getcmd()
{
    int32_t i = 0;

    qset(0, argv[i], strlen(argv[i]));
    while (++i < argc) {
	qset(1, " ", 1);
	qset(1, argv[i], strlen(argv[i]));
    }

    return TECO__NORMAL;
}

static int32_t gexit()
{
    int32_t cmdlen, status = 0, varlen;
    uint8_t *cp, *sp;
    uint8_t *cmd, *var;

    if (!(ctx.flags & TECO_M_CLNF)) {
	char *shell;

	if ((shell = getenv("TEC_SHELL")) == 0)
	    shell = getenv("SHELL");

	cmd = malloc(ctx.filbuf.qrg_size);
	if (cmd == 0)
	    ERROR_MESSAGE(MEM);
	memcpy(cmd, ctx.filbuf.qrg_ptr, ctx.filbuf.qrg_size);
	cmd[ctx.filbuf.qrg_size] = '\0';

    	switch (fork()) {
	default:
    	    wait((int *)0);
	    exit(EXIT_SUCCESS);
	    break;

	case 0:
	    execl(shell, shell, "-c", cmd, (char *) 0);

	case -1:
	    /*
	    ** The only way we will get here is if something went terribly
	    ** wrong, so no need to check, just assume the worst.
	    */
    	    ctx.syserr = errno;
	    ERROR_MESSAGE(ERR);
	    break;
	}
    }

    if (strncasecmp(cmd, "SPA", 3) == 0) {
    } else {
    	char *name, *value = "";
    	name = value;
    }

    return status;
}

static void sigcont_handler(signum)
    int signum;
{
    if (isatty(STDIN_FILENO)) {
	tcsetattr(STDIN_FILENO, TCSANOW, &attr);
    }
}


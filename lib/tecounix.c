/*
**++
**  MODULE DESCRIPTION:
**
**      This module contains the UNIX specific code for TECO.
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
**      08-NOV-2011 V41.00  Sneddon     Initial coding.
**      15-DEC-2011 V41.01  Sneddon     Originaly termios.c, now covers
**                                      a broader range of support.
**      06-JUN-2013 V41.02  Sneddon     Re-arrange getcmd.
**      10-JUN-2013 V41.03  Sneddon     Add :EG support.
**      21-JAN-2014 V41.04  Sneddon     Add EI and base getfl support.
**      24-JUL-2014 V41.05  Sneddon     Support change to input() callback.
**                                      Add some more useful comments!  Add
**                                      support for EN.
**      02-OCT-2014 V41.06  Sneddon     Add syserr.
**      28-OCT-2015 V41.07  Sneddon     Add crtset.
**      01-MAR-2016 V41.08  Sneddon     Adjusted getcmd to perform basename
**                                      on argv[0].
**--
*/
#define MODULE TECOUNIX
#define VERSION "V41.08"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include <curses.h>
#include <errno.h>
#include <glob.h>
#include <libgen.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <termios.h>
#include <unistd.h>

    struct UNIT {
        FILE *fptr;
        char path[PATH_MAX];
    };
#define UNIT_S_UNIT (sizeof(struct UNIT))
    typedef struct UNIT *FILEHANDLE;

#define OS_MODULE_BUILD
#include "tecodef.h"
#include "tecomsg.h"
#include "globals.h"

/*
** Forward Declarations
*/

    static int32_t init();
    static int32_t restore();
    static int8_t input();
    static int32_t output();
    static int32_t ejflg();
    static int32_t etflg();
    static int32_t getcmd();
    static int32_t gexit();
    static int32_t getfl();
    static void sigcont_handler();
    static void close_indir();
    static int32_t cvt_errno();
    static int32_t syserr();
    static int32_t crtset();

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
        getfl,
        syserr,
        crtset,
    };

/*
** Static Storage.
*/

    static struct {
        size_t gl_pathi;
        int status;
        glob_t glob;
    } en = { -1, 0 };
    static int ctrlz_cnt = 0;
    struct termios attr;
    struct termios pattr;
    struct UNIT indir_cmd;

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
                status = cvt_errno(errno);
            }
        }
    } else {
        status = cvt_errno(errno);
    }

    ctx.etype |= TECO_M_ET_LC; // Should this be in crtrub?

    return (int32_t)status;
} /* init */

static int32_t restore(void)
{
    int status = TECO__NORMAL;

    fflush(stdout);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &pattr) != 0) {
        status = cvt_errno(errno);
    }

    return status;
} /* restore */

static int8_t input()
{
    int chr, status, xitchr = TECO_C_SUB;

    if (ctx.temp == TECO_C_CR) {
        /*
        ** All CRs get translated to CR/LFs, so return the LF portion.
        */
        chr = TECO_C_LF;
    } else {
        if (ctx.indir == 0) {
            chr = fgetc(stdin);
            if ((status = ferror(stdin)) == 0) {
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
            } else if (status == -1) {
                status = cvt_errno(errno);
            } else {
                status = cvt_errno(EIO);
            }
        } else {
            /*
            ** Indirect command input is active, so read the character
            ** from the secondary command input.
            */
            chr = fgetc(indir_cmd.fptr);
            if (chr == EOF) {
                /*
                ** We are at the end of file.  This isn't something we
                ** show to the user, we just silently switch back to the
                ** primary input stream.
                **
                ** So, close off the indirect input and call ourselves to
                ** fetch the next character off the primary input.
                */
                close_indir();
                chr = input();
            }
        }
    }

    return (int8_t)chr;
} /* input */

static int32_t output(chr)
    uint8_t chr;
{
    int status = TECO__NORMAL;

    if (putc(chr, stdout) == EOF) status = cvt_errno(errno);
    return (int32_t)status;
} /* output */

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
} /* ejflg */

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
} /* etflg */

static int32_t getcmd()
{
    int32_t i;
    char *argv0, *prog_name;

    argv0 = strdup(argv[0]);
    if (prog_name == 0)
        ERROR_MESSAGE(MEM);
    prog_name = basename(argv0);

    qset(0, prog_name, strlen(prog_name));
    for (i = 1; i < argc; i++) {
        qset(1, " ", 1);
        qset(1, argv[i], strlen(argv[i]));
    }

    free(argv0);

    return TECO__NORMAL;
} /* getcmd */

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
            execlp(shell, shell, "-c", cmd, (char *) 0);

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
        //
    } else {
        char *name = ctx.filbuf.qrg_ptr, *value = "";

        // split it up...

        if (strncasecmp(cmd, "INI", 3) == 0) {
            // we can only retrieve this.  Other functions have no change.
            // actually, will remove and set TEC_INIT.
        } else if (strncasecmp(cmd, "MEM", 3) == 0) {
            char *memenv;

            memenv = getenv("TEC_MEMORY");
            if (value == 0) {
                // look for TEC_MEMORY
                // if found
                    // return it
                // else
                    // look for ~/.tec_memory
                    // if found
                        // return it
            } else if (value[0] == '\0') {
                // if TEC_MEMORY, unsetenv
                // if ~/.tec_memory, unlink it
            } else {
                // if TEC_MEMORY
                    // return it
                // else if ~/.tec_memory
                    // return it
            }
        } else if (strncasecmp(cmd, "ENV", 3) == 0) {
            if (value == 0) {
                // getenv and return
            } else if (value[0] == '\0') {
                // unsetenv and return
            } else {
                // setenv to value and return
            }
        }
    }

    return status;
} /* gexit */

static int32_t getfl(chr)
    uint8_t chr;
{
    int status = TECO__NORMAL;

    if (ctx.filbuf.qrg_size == 0) {
        switch (chr) {
        case 'I':
            close_indir();
            break;

        case 'N':
            if (en.gl_pathi == -1) {
                /*
                ** The EN buffer has not been initialised.  Under V40
                ** on VMS, the message returned is actually the result of
                ** a bug and 99.9% of the time it fails with the error:
                **
                **    ?ERR    %SYSTEM-W-ILLSER, illegal service call number ""
                **
                ** On RSTS/E it responds:
                **
                **    ?ERR    ?Illegal file name ""
                **
                ** So, on UNIX we respond with the system errno, EBADF.
                */
                status = cvt_errno(EBADF);
            } else {
                if (en.status == GLOB_NOMATCH) {
                    status = TECO__FNF;
                } else if (en.gl_pathi >= en.glob.gl_pathc) {
                    status = TECO__FNF;
                } else {
                    char *path = en.glob.gl_pathv[en.gl_pathi++];

                    ctx.qnmbr = &ctx.filbuf;
                    qset(0, path, strlen(path));
                }
            }
            break;
        }
    } else {
        char path[PATH_MAX];

        memcpy(path, ctx.filbuf.qrg_ptr, ctx.filbuf.qrg_size);
        path[ctx.filbuf.qrg_size] = '\0';

        // path = realpath(path);
        // if path == 0
            // status = cvt_errno(errno)
        // else
            // continue...

        switch (chr) {
        case 'I':
            close_indir();
            indir_cmd.fptr = fopen(path, "r");
            if (indir_cmd.fptr == 0)
                status = cvt_errno(errno);
            else
                ctx.indir = 1;
            break;

        case 'N': {
            int flags;

            if (en.gl_pathi >= 0) {
                en.gl_pathi = -1;
                globfree(&en.glob);
            }

            flags = GLOB_MARK;
#ifdef GLOB_TILDE_CHECK
            flags |= GLOB_TILDE_CHECK;
#endif
            en.status = glob(path, flags, 0, &en.glob);
            switch (en.status) {
            case GLOB_NOSPACE: status = cvt_errno(ENOMEM); break;
            case GLOB_NOSYS:   status = cvt_errno(ENOSYS); break;
            case GLOB_ABORTED: status = cvt_errno(EIO); break;
            case GLOB_NOMATCH:
                /*
                ** To remain compatible with TECO-32 V40 we return
                ** this error as FNF when the user attempts to
                ** fetch the first iteration.
                */
            default:
                en.gl_pathi = 0;
                break;
            }
            break;
        }

        }

        // if success -- make this more general when we add other files...
        strcpy(indir_cmd.path, path);
    }

    return status;
} /* getfl */

static void close_indir() {
    if (ctx.indir != 0) {
        ctx.indir = 0;
        if (fclose(indir_cmd.fptr) != 0) {
            ERROR(cvt_errno(errno));
        }
    }
} /* close_indir */

static void sigcont_handler(signum)
    int signum;
{
    if (isatty(STDIN_FILENO)) {
        tcsetattr(STDIN_FILENO, TCSANOW, &attr);
    }
} /* sigcont_handler */

static int32_t cvt_errno(err)
    int err;
{
    int32_t status;

    switch (err) {
    case ENOENT:    status = TECO__FNF; break;
    case ENOMEM:    status = TECO__MEM; break;
    default:
        status = TECO__ERR;
        ctx.syserr = err;
        break;
    }

    return status;
} /* cvt_errno */

/*
** FUNCTIONAL DESCRIPTION:
**      This routine resturns the eror messages for UNIX-like systems.  The
** message is in two parts.  The messge id and the message itself.
**
**      For POSIX errno values, 1 - 34, we supply a message id of the
** POSIX constant name.  For all other messages, we return an id that is
** the message number, zero filled to 5 places.  For example, an errno of
** 16 would return:
**
**     "EBUSY", "Device or resource busy"
**
** Whereas, on a GNU/Linux system, an errno of 36 would return:
**
**     "00036", "File name too long"
*/
int32_t syserr(id, message)
    uint8_t **id;
    uint8_t **message;
{
    static char *posix[] = { "EPERM", "ENOENT", "ESRCH", "EINTR", "EIO",
        "ENXIO", "E2BIG", "ENOEXEC", "EBADF", "ECHILD", "EAGAIN", "ENOMEM",
        "EACCES", "EFAULT", "ENOTBLK", "EBUSY", "EEXIST", "EXDEV", "ENODEV",
        "ENOTDIR", "EISDIR", "EINVAL", "ENFILE", "EMFILE", "ENOTTY",
        "ETXTBSY", "EFBIG", "ENOSPC", "ESPIPE", "EROFS", "EMLINK", "EPIPE",
        "EDOM", "ERANGE" };

    if (id) {
        if (ctx.syserr <= (sizeof(posix) / sizeof(posix[0]))) {
            *id = strdup(posix[ctx.syserr-1]);
        } else {
            asprintf(id, "%05d", (int) ctx.syserr);
        }
    }

    if (message) {
        *message = strdup(strerror(ctx.syserr));
    }

    return TECO__NORMAL;
} /* syserr */

static int32_t crtset() {
    static const uint8_t *grptbl[3][GRPTBL_MAX] = {
        {
            /* VT100, VK100, etc. */

            "h", /* "NL" */     "e", /* "LF" */     "g", /* PLUS OR MINUS */
            "i", /* "VT" */     "`", /* DIAMOND */  "c", /* "FF" */
            "b", /* "HT" */     "~", /* CENTERED DOT */
            "d", /* "CR" */     "|", /* NOT EQUALS */
            "m", /* LOWER LEFT CORNER */
            "y", /* LESS THAN OR EQUAL TO */
            "j", /* LOWER RIGHT CORNER */
            "z", /* GREATER THAN OR EQUAL TO */     "{", /* PI */
        }, {
            /* VT52 */
            "h", /* RIGHT ARROW */          "k", /* DOWN ARROW */
            "g", /* PLUS OR MINUS */        "j", /* DIVIDED BY */
            "~", /* PARAGRAPH */            "a", /* SOLID RECTANGLE */
            "f", /* DEGREES */              "i", /* ELIPSIS */
            "`", /* "CD" */                 "d", /* "5/" */
            "[", /* NO SPECIAL GRAPHIC... */"<", /* NO SPECIAL GRAPHIC... */
            "]", /* NO SPECIAL GRAPHIC... */">", /* NO SPECIAL GRAPHIC... */
            "c", /* "3/" */
        }, {
            /* UTF-8 */

            "\xe2\x90\xa4", /* "NL" */      "\xe2\x90\x8a", /* "LF" */
            "\xe2\x88\x93", /* PLUS OR MINUS */
            "\xe2\x90\x8b", /* "VT" */      "\xe2\x8b\x84", /* DIAMOND */
            "\xe2\x90\x8c", /* "FF" */      "\xe2\x90\x89", /* "HT" */
            "\xe2\x8b\x85", /* CENTERED DOT */
            "\xe2\x90\x8d", /* CR */        "\xe2\x89\xa0", /* NOT EQUALS */
            "\xe2\x94\x95", /* LOWER LEFT CORNER */
            "\xe2\x89\xa4", /* LESS THAN OR EQUAL TO */
            "\xe2\x89\x9a", /* LOWER RIGHT CORNER */
            "\xe2\x89\xa5", /* GREATER THAN OR EQUAL TO */
            "\xcf\x80",     /* PI */
        },
    };
    int status;
    char rscap[4];

    scope.vtsize = 24;
    scope.htsize = 80;

    if (setupterm(0, -1, &status) == ERR) {
        // 1 = hardcopy
        // 0 = unknown terminal
        // -1 error

        return 0;
    }

    /*
    ** FIXSEQ - CLEAN UP MODES.
    **      NULL => NO FIXING UP IS NEEDED
    */
    scope.seqfix = 0;

    strcpy(rscap, "rs1");
    while (rscap[2]++ <= '3') {
        int fixlen = (scope.seqfix == 0) ? 0 : strlen(scope.seqfix);
        char *fix, *rs;

        rs = tigetstr(rscap);
        if (rs == 0)
            break;

        fix = realloc(scope.seqfix, fixlen+strlen(rs)+1);
        if (fix == 0)
            break;

        strcat(fix, rs);
        scope.seqfix = fix;
    }

    scope.seqhom = tigetstr("home");
    scope.seqeol = tigetstr("el");
    scope.seqeos = tigetstr("ed");
    scope.seqscu = tigetstr("ind");
    scope.seqscd = tigetstr("ri");

    /*
    ** MONSEQ - TURN ON MARK'D REGION MODE (SET REVERSE VIDEO ATTRIBUTE).
    **      NULL => FEATURE IS NOT AVAILABLE
    */
    scope.seqmon = tigetstr("rev");

    /*
    ** MOFSEQ - TURN OFF MARK'D REGION MODE (CLEAR ALL ATTRIBUTES).
    **      NULL => FEATURE IS NOT AVAILABLE
    */
    scope.seqmof = tigetstr("sgr0");

    /*
    ** CONSEQ - TURN ON CURSOR CHARACTER ATTRIBUTES.
    **      NULL => FEATURE IS NOT AVAILABLE
    */
    scope.seqcon = tigetstr("cvvis");

    /*
    ** COFSEQ - TURN OFF CURSOR CHARACTER ATTRIBUTES.
    **      NULL => FEATURE IS NOT AVAILABLE
    */
    scope.seqcof = tigetstr("sgr0");

    scope.seqerc = tigetstr("ech");
//    scope.seqerl = tigetstr("");

    /*
    ** SAVCUR - SAVE CURSOR POSITION AND ATTRIBUTES.
    ** RESCUR - RESTORE CURSOR POSITION AND ATTRIBUTES.
    **      NULL => FEATURE IS NOT AVAILABLE
    */
    scope.scur = tigetstr("sc");
    scope.rcur = tigetstr("rc");

    if ((scope.scur != 0) && (scope.rcur != 0))
        scope.t_flags |= TECO_M_TC_SAVE;


    /* CLFSEQ - CURSOR LEFT */
    scope.seqclf = tigetstr("cub1");

    /* CRTSEQ - CURSOR RIGHT */
    scope.seqcrt = tigetstr("cuf1");

    /* CUPSEQ - CURSOR UP */
    scope.seqcup = tigetstr("cuu1");

    /* CURCDN - CURSOR DOWN */
    scope.seqcdn = tigetstr("cud1");

    //dca
#if 0
uint32_t crtset() {

    /*
    ** Set default values.
    */
    scope.scroln = 0;

    /*
    ** Call host-specific setup routine.
    */
    io_support.crtset();

} /* crtset */

#endif
#if 0
    status = regcomp();
    if (status != 0) {
    } else {
        status = regexec();

        if (status == 0) {

    // if TERM =v%1%%
        // setup gon and gof
        // setup table like real TECO-32
    // else if TERM = vt52
        // setup
    // else
        } else if (strstr(term, "vt52") != 0) {
            scope.seqgon = strdup("\x1bF\xff");
            scope.seqgof = strdup("\x1bG\xff");
            scope.grptbl = &grptbl[1];

            // check status...
        } else {
            static const char *vars[] = {
                "LC_ALL", "LC_TYPE", "LANG",
            };

            scope.seqgon = scope.seqgof = 0;

            // check LC_ALL, LC_TYPE, LANG
            // if unicode
                // setup graphic table of unicode characters
        }


        regfree();
    }
    //?
#endif
} /* crtset */

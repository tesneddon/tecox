/*
**++
**  MODULE DESCRIPTION:
**
**      TECO internal constant and structure definitions.
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
**  CREATION DATE:      01-FEB-2004
**
**  MODIFICATION HISTORY:
**
**      01-FEB-2004 V41.00  Sneddon   Initial coding.
**      08-NOV-2011 V41.01  Sneddon   Remove dispatch stuff from previous port.
**                                    Add TT driver structure.
**      10-NOV-2011 V41.02  Sneddon   Changes to radix constants.
**      11-NOV-2011 V41.03  Sneddon   Add np back to TECODEF.
**      15-DEC-2011 V41.04  Sneddon   Add IO_SUPPORT as a replacement and
**                                    extension of TT driver structure.
**      31-JAN-2012 V41.05  Sneddon   Changed all flags in TECODEF to be
**                                    uint32_t.
**      01-FEB-2012 V41.06  Sneddon   Moving external routine declarations in
**                                    here as well.
**      07-MAR-2012 V41.07  Sneddon   Added x86-64/amd64.
**      16-MAY-2013 V41.08  Sneddon   Added OpenBSD/hppa.
**      09-OCT-2013 V41.08A Sneddon   Replaced [u]intXX_t with C99 [u]intmax_t.
**                                    Also removed TECO_K_WORD_SIZE.
**      11-JUN-2013 V41.09  Sneddon   Add gexit to IO_SUPPORT.
**      09-DEC-2013 V41.10  Sneddon   Add version macros in here.
**      20-JAN-2013 V41.11  Sneddon   Add [u]intmax_t for VAXC.  Removed mem
**                                    zone stuff.
**      21-JAN-2014 V41.12  Sneddon   Add getfl to IO_SUPPORT.
**      24-JUL-2014 V41.13  Sneddon   Expanded tabs. Remove old code. Tweak
**                                    IO_SUPPORT interface for input().  Add
**                                    Add ERROR macro.
**      02-OCT-2014 V41.14  Sneddon   Add syserr to IO_SUPPORT.
**	16-MAY-2019 V41.15  Sneddon   Moved character constants to tecochr.h
**      12-MAR-2021 V41.16  Sneddon   Moved macros and function prototypes
**                                    lib/macros.h and lib/extrn.h.
**--
*/
#ifndef __TECODEF_LOADED
#define __TECODEF_LOADED 1

#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include "tecochr.h"

/*
** TECO Version Macros.
*/
#define TECO_K_VERSION 41
#define TECO_T_VERSION "V41"

/* TECO operating system and processor architecture constants.
 *
 * The following mess of '#if defined()'s is used to determine the
 * values to be returned by the '-1EJ' command. The table below
 * provides a more 'readable' explanation of what is going on.
 *
 * The table below should be used to build the one in the TECO manual.
 *
 *  -- rewrite this bit and include about [u]intXX_t & OS/ARCH...
 *
 *
 *  -- re-arrange the table below to better suit common OS
 *     numbers across different hardware platforms...
 *
 * -------------------------------------------------------
 * Computer (m)         Operating System (n)
 * -------------------------------------------------------
 * 0    PDP-11          0       RSX-11D
 *                      1       RSX-11M
 *                      2       RSX-11S
 *                      3       IAS
 *                      4       RSTS/E
 *                      5       VAX/VMS (compatability mode)
 *                      6       RSX-11M+
 *                      7       RT-11
 * 1    PDP-8           0       OS/8
 * 2    DEC-10          0       TOPS-10
 * 3    DEC-20          0       TOPS-20
 * 4    VAX             0       OpenVMS VAX (native mode)
 *                              OpenVMS Alpha (compatability mode)
 *                              OpenVMS I64 (compatability mode)
 *                      1       OpenBSD
 *                      2       Ultrix-32
 * 5    Alpha           0       OpenVMS Alpha
 *                      1       OpenBSD
 *                      2       OSF/1
 *                              Digital UNIX
 *                              Tru64 UNIX
 *                      3       FreeBSD
 * 6    IA-64           0       OpenVMS I64
 *                      1       HP-UX
 *                      2       GNU/Linux
 * 7    PA-RISC         0       HP-UX
 *                      1       GNU/Linux
 *                      2       OpenBSD
 * 8    x86             0       GNU/Linux
 *                      1       OpenBSD
 *                      2       Solaris
 *                      3       FreeBSD
 *                      4       NetBSD
 *                      5       Plan 9
 *                      6       Darwin
 * 9    SPARC           0       Solaris
 * 10   MIPS            0       Ultrix
 *                      1       OSF/1
 * 11   Cray            0       UNICOS
 * 12   PowerPC         0       MacOS X
 *                      1       Darwin
 * 13   x86-64          0       GNU/Linux
 *                      4       NetBSD
 */
#if (defined(vax) || defined(VAX) || defined(__vax) || defined(__VAX))
# define TECO_K_ARCH 4
# if (defined(vms) || defined(VMS) || defined(__vms) || defined(__VMS))
#  define TECO_K_OS 0
#  if (defined(__DECC))
#   include <inttypes.h>
#  else
    /*
    ** Define integral types not found in VAXC
    */
    typedef signed char int8_t;
    typedef unsigned char uint8_t;
    typedef short int int16_t;
    typedef unsigned short int uint16_t;
    typedef int int32_t;
    typedef unsigned int uint32_t;
    typedef int intmax_t;
    typedef unsigned int uintmax_t;
#  endif
# elif (defined(__OpenBSD__))
#  define TECO_K_OS 1
# elif (defined(ultrix))
#  define TECO_K_OS 2
# else
#  error "unknown OS on VAX architecture"
# endif
#elif (defined(__alpha) || defined(__ALPHA) || defined(__Alpha_AXP))
# define TECO_K_ARCH 5
# if (defined(vms) || defined(VMS) || defined(__vms) || defined(__VMS))
#  define TECO_K_OS 0
#  include <inttypes.h>
# elif (defined(__OpenBSD__))
#  define TECO_K_OS 1
# elif ((defined(__digital__) && defined(__unix__)) || defined(__osf__))
#  define TECO_K_OS 2
# elif (defined(__FreeBSD__))
#  define TECO_K_OS 3
# else
#  error "unknown OS on Alpha architecture"
# endif
#elif (defined(ia64) || defined(__ia64) || defined(__ia64__))
# define TECO_K_ARCH 6
# if (defined(vms) || defined(VMS) || defined(__vms) || defined(__VMS))
#  define TECO_K_OS 0
#  include <inttypes.h>
# elif (defined(__hpux))
#  define TECO_K_OS 1
# else
#  error "unknown OS on IA-64 architecture"
# endif
#elif (defined(__hppa) || defined(__hppa__))
# define TECO_K_ARCH 7
# if (defined(__hpux))
#  define TECO_K_OS 0
# elif (defined(linux) || defined(__linux) || defined(__linux__))
#  define TECO_K_OS 1
#  include <stdint.h>
# elif (defined(__OpenBSD__))
#  define TECO_K_OS 2
# else
#  error "unknown OS on PA-RISC architecture"
# endif
#elif (defined(i386) || defined(__i386) || defined(__i386__))
# define TECO_K_ARCH 8
# if (defined(linux) || defined(__linux) || defined(__linux__))
#  define TECO_K_OS 0
#  include <stdint.h>
# elif (defined(__OpenBSD__))
#  define TECO_K_OS 1
# elif ((defined(sun) || defined(__sun)) && (defined(unix) || defined(__unix)))
#  define TECO_K_OS 2
# elif (defined(__FreeBSD__))
#  define TECO_K_OS 3
# elif (defined(__NetBSD__))
#  define TECO_K_OS 4
# else
#  error "unknown OS on x86 architecture"
# endif
#elif (defined(sparc) || defined(__sparc))
# define TECO_K_ARCH 9
# if ((defined(sun) || defined(__sun)) && (defined(unix) || defined(__unix)))
#  define TECO_K_OS 0
# else
#  error "unknown OS on SPARC architecture"
# endif
#elif (defined(mips))
# define TECO_K_ARCH 10
# if (defined(ultrix))
#  define TECO_K_OS 0
# else
#  error "unknown OS on MIPS architecture"
# endif
#elif (defined(cray) || defined(CRAY) || defined(_CRAY))
# define TECO_K_ARCH 11
# if (defined(_UNICOS))
#  define TECO_K_OS 0
# else
#  error "unknown OS on Cray architecture"
# endif
#elif (defined(__POWERPC__) || defined(_ARCH_PPC) || defined(__ppc__))
# define TECO_K_ARCH 12
# if (defined(__APPLE__))
#  define TECO_K_OS 0
# else
#  error "unknown OS on PowerPC architecture"
# endif
#elif (defined(__amd64__) || defined(__x86_54__))
# define TECO_K_ARCH 13
# if (defined(linux) || defined(__linux) || defined(__linux__))
#  define TECO_K_OS 0
#  include <stdint.h>
# elif (defined(__NetBSD__))
#  define TECO_K_OS 4
# else
#  error "unknown OS on x86-64 architecture"
# endif
#else
# error "unknown architecture"
#endif

#ifdef TRUE
# undef TRUE
#endif
#define TRUE 1
#ifdef FALSE
# undef FALSE
#endif
#define FALSE 0

/*
**      Define q-register data structure
*/
    typedef struct _qrgdef {
        struct _qrgdef  *qrg_next;      /* Q-register next in PDL           */
        intmax_t        qrg_value;      /* Q-register's 'VALUE'             */
        uint8_t         *qrg_ptr;       /* Q-register's Text Pointer        */
        uint16_t        qrg_size;       /* Q-register's size                */
        uint16_t        qrg_alloc;      /* Q-register's space alloc         */
        uint8_t         qrg_flags;      /* Q-register flags                 */
#define TECO_M_QRG_REF   0x1            /* Has been pushed                  */
#define TECO_M_QRG_LOCAL 0x2            /* It's a local q-register          */
        uint8_t         qrg_num;        /* Original q-reg number            */
        uint16_t        qrg_space;      /* Spare                            */
    } QRGDEF;

#define TECO_K_QRG_EXTENT 256           /* Q-reg storage allocation extent  */
#define TECO_K_NUMQRG ((('Z' - 'A') + 1) + (('9' - '0') + 1))
                                        /* NUMBER OF Q-REGS PER GROUP       */
                                        /*  (A-Z,0-9)                       */
#define TECO_K_QRG_FAKE (TECO_K_NUMQRG) /* REGISTER FOR TEC$DO_COMMAND      */

#define TECO__TRUE -1
#define TECO__FALSE 0

/*
** Flags to control the behaviour of push/pop
*/
#define TECO_K_PDL_ITR   0
#define TECO_K_PDL_MACRO 1
#define TECO_K_PDL_PAREN 2

    typedef struct _pdldef {            /* Push-down list block             */
        struct _pdldef  *next;          /* Pointer to next block in stack   */
        uint32_t        type;           /* Tyep of block PAREN,MACRO,ITR    */
        uint8_t         *itrst;
        uint32_t        itrcnt;
        uint32_t        nopr;
        int32_t         nacc;
        uint8_t         *scanp;
        uint32_t        mpdcnt;
        QRGDEF          *qcmnd;
        QRGDEF          *lclptr;
    } PDLDEF;

/*
** Flags to control the behaviour of teco_qref().
*/
#define TECO_K_QREF_NORMAL 0
#define TECO_K_QREF_EXTENDED 1

/* Flags to control the behaviour of the teco_type() and teco_print()
 * output routines.
 */
#define TECO_M_TYPE_T 0x0               /* Type without case flagging and   */
                                        /*  ringing bells                   */
#define TECO_M_TYPE_F 0x1               /* Type with case flagging          */
#define TECO_M_TYPE_B 0x2               /* Type with ringing bells          */

/* Flags to control zerod output mode.
 */
#define TECO_K_ZEROD_TERM 1             /* Write number to terminal         */
#define TECO_K_ZEROD_TXBUF 2            /* Write number to text buffer      */

/* Flags to control the behaviour of the teco_qskp() routine.
 */
#define TECO_K_QSKP_NONE 0              /* No quote restoration             */
#define TECO_K_QSKP_IREST 1             /* Restore quote to ESC             */
#define TECO_K_QSKP_SAVE 2              /* Save quote before processing and */
                                        /*  restore before return           */

#define TECO_T_HELP_CMD "HELP"
#define TECO_K_HELP_SIZE (sizeof(TECO_T_HELP_CMD)-1)

#define TECO_K_CTRLZ_MAX 3
#define TECO_K_ESC_MAX 2

/*
** Radix control flags.
*/
#define RADIX_MIN 0
#define RADIX_DEC 0                     /* Decimal radix                    */
#define RADIX_OCT 1                     /* Octal radix                      */
#define RADIX_HEX 2                     /* Hexidecimal radix                */
#define RADIX_MAX 2

/*
** TECO File Access Block
*/

    typedef struct _tecque {
        struct _tecque *next, *prev;
        int32_t size;
        uint8_t *ptr;
        uint8_t data[1];
    } TECQUE;

#ifndef OS_MODULE_BUILD
    typedef void *FILEHANDLE;
#endif
    typedef struct _tecfab {
        uint32_t tecsts;                /* Status                           */
        TECQUE tecque;                  /* Data line queue                  */
        void *tecdsp;
        uint8_t tecctl[4];              /* Control bytes */
        FILEHANDLE tecfab;              /* OS-specific I/O handle           */
    } TECFAB;

#define TECO_M_TECEOF   0x0001          /* at end-of-file                   */
#define TECO_M_TECNO1ST 0x0002          /* not first time through           */
#define TECO_M_TECBUF   0x0004          /* use buffered data instead of file*/
#define TECO_M_TECICR   0x0008          /* <cr> ignored, need <cr><lf> on   */
                                        /*  eof                             */
#define TECO_M_TECECR   0x0010          /* extra <cr> output, do <lf> next  */
#define TECO_M_TECNXT   0x0020          /* pre-fetched character exists     */
                                        /*  (@ + 3)                         */
#define TECO_M_TECFMT   0x0040          /* /-cr, /cr, or /ft specified      */
#define TECO_M_TECRW    0x0080          /* /rw - rewind magtape before opens*/
#define TECO_M_TECSH    0x0100          /* /sh - shared open                */
#define TECO_M_TECB2    0x0200          /* /b2 - basic-plus-2 mode          */
#define TECO_M_TECNV    0x0400          /* /nv - always create a new version*/
#define TECO_M_TECSTM   0x0800          /* /stm - stream format specified   */
#define TECO_M_TECVAR   0x1000          /* /var - variable format specified */


/*
**      Define the impure area, pointed to
**      by R11.
*/
    typedef struct _tecodef {
        uint8_t         *scanp;         /* COMMAND LINE EXECUTION POINTER   */
        uint32_t        mpdcnt;         /* MACRO PUSHDOWN COUNTER           */
        QRGDEF          *lclptr;        /* LOCAL LEVEL Q-REG ARRAY POINTER  */
        uint8_t         *itrst;         /* ITERATION START                  */
        int32_t         itrcnt;         /* ITERATION COUNT                  */
        uint32_t        nopr;           /* ARITHMETIC OPERATOR              */
        int32_t         nacc;           /* EXPRESSION ACCUMULATOR           */
        uint32_t        flags;          /* Flags word                       */
#define TECO_M_NFLG   0x1               /* NUMBER FLAG                      */
#define TECO_M_OFLG   0x2               /* OPERATOR FLAG                    */
#define TECO_M_CFLG   0x4               /* COMMA FLAG                       */
#define TECO_M_CLNF   0x8               /* COLON FLAG                       */
#define TECO_M_CLN2F  0x10              /* Two colons seen                  */
#define TECO_M_QFLG   0x20              /* QUOTED STRING FLAG               */
#define TECO_M_TFLG   0x40              /* TRACE FLAG                       */
#define TECO_M_STOP   0x80              /* STOP INDICATOR                   */
#define TECO_M_REPFLG 0x100             /* REPLACE FLAG                     */
        uint32_t        temp;           /* GENERAL TEMPORARY READ/WRITE     */
                                        /*  WORD                            */
        uint8_t         *oscanp;        /* BACKUP FOR "SCANP"               */
        uint8_t         quote;          /* QUOTE CHARACTER                  */
                                        /*  (NORMALLY ESCAPE)               */
        uint8_t         __fill_1;
        uint16_t        pcnt;           /* PAREN PUSH COUNTER               */
        int32_t         m;              /* NUMBER                           */
        int32_t         n;              /*  ARGUMENTS                       */
        int32_t         np;             /* VALUE OF CURRENT NUMBER          */
        uint16_t        qlengt;         /* COMMAND LINE LENGTH              */
        uint16_t        cndn;           /* COUNTER FOR " NESTING            */
        uint8_t         *pst;           /* CHARACTER POSITION AT SEARCH     */
                                        /*  START                           */
/* ------------------------------------    END OF EACH COMMAND CLEAR AREA   */
        QRGDEF          *qcmnd;         /* COMMAND LINE OR MACRO Q-REG      */
                                        /*  NUMBER                          */
        QRGDEF          *qnmbr;         /* CURRENT Q-REG NUMBER             */
        PDLDEF          *pdl;           /* MACRO PUSH-DOWN LIST             */
        QRGDEF          *qpdl;          /* Q-REGISTER PUSH-DOWN LIST        */
        uint32_t        p;              /* CURRENT TEXT POINTER (.)         */
        uint32_t        zz;             /* TEXT BUFFER SIZE IN USE          */
        uint32_t        errpos;         /* ERROR POSITION                   */
        uint32_t        lschsz;         /* -(LENGTH) OF LAST SKIPPED        */
        uint32_t        evflag;         /* EDIT VERIFY FLAG                 */
        uint32_t        euflag;         /* CASE FLAGGING FLAG               */
        uint32_t        etype;          /* EDIT TYPEOUT FLAG                */
#define TECO_M_ET_BIN   0x1             /*    +1., output in binary (image) */
                                        /*         mode                     */
#define TECO_M_ET_CRT   0x2             /*    +2., do scope type rubout on  */
                                        /*         control/u                */
#define TECO_M_ET_LC    0x4             /*    +4., accept lower case input  */
#define TECO_M_ET_NCH   0x8             /*    +8., no echo during input for */
                                        /*         ctrl/t                   */
#define TECO_M_ET_CCO   0x10            /*   +16., cancel control/o on      */
                                        /*         output                   */
#define TECO_M_ET_CKE   0x20            /*   +32., return -1 if error/no    */
                                        /*         input on ctrl/t          */
#define TECO_M_ET_DET   0x40            /*   +64., detach and detached flag */
#define TECO_M_ET_XIT   0x80            /*  +128., "no prompt yet" flag     */
#define TECO_M_ET_TRU   0x100           /*  +256., truncate long output     */
                                        /*         lines                    */
#define TECO_M_ET_IAS   0x200           /*  +512., interactive scope        */
                                        /*         available for "watch"    */
#define TECO_M_ET_RFS   0x400           /* +1024., refresh scope available  */
#define TECO_M_TECO8    0x800           /* +2048., reserved for TECO-8      */
#define TECO_M_ET_UNIX  0x800           /* +2048., enable UNIX mode on UNIX */
#define TECO_M_ET_8BT   0x1000          /* +4096., terminal is an 8-bit     */
                                        /*         terminal                 */
#define TECO_M_ET_GRV   0x2000          /* +1892., accept "`" as escape     */
                                        /*         during command input     */
#define TECO_M_ET_UNU   0x4000          /*+16384., unused                   */
#define TECO_M_ET_CC    0x8000          /*+32768., allow program trap       */
                                        /*         control/c                */
        uint32_t        esflag;         /* EDIT SEARCH FLAG                 */
        uint32_t        ehelp;          /* EDIT HELP LEVEL                  */
        uint32_t        edit;           /* EDIT LEVEL FLAG                  */
#define TECO_M_ED_CTL   0x1             /*  +1., don't allow "^" as meaning */
                                        /*       control character          */
#define TECO_M_ED_YNK   0x2             /*  +2., allow yanks, etc. to       */
                                        /*       clobber text buffer        */
#define TECO_M_ED_EXP   0x4             /*  +4., don't allow arbitrary      */
                                        /*       expansion(s)               */
#define TECO_M_ED_TECO8 0x8             /*  +8., not used by TECO-11        */
#define TECO_M_ED_SRH   0x10            /* +16., don't reset "dot" on       */
                                        /*       search failure             */
#define TECO_M_ED_IMD   0x20            /* +32., allow immediate mode       */
                                        /*       commands                   */
#define TECO_M_ED_INC   0x40            /* +64., only move "dot" by one on  */
                                        /*       iterative search           */
#define TECO_M_ED_WCH   0x80            /*+128., don't allow automatic "w"  */
                                        /*       command before prompt      */
        uint32_t        sflg;           /* SEARCH MODE FLAG                 */
        uint16_t        eeflg;          /* Alternate escape character       */
        uint16_t        flags2;         /* SECONDARY FLAG WORD              */
#define TECO_M_FFFLAG   0x1             /* FORM FEED FLAG                   */
#define TECO_M_EOFLAG   0x2             /* END-OF-FILE FLAG                 */
#define TECO_M_EXITOK   0x4             /* EXIT WITH DIRY BUFFER IS OKAY    */
#define TECO_M_EXITEND  0x8             /* EXIT AT END OF COMMAND           */
#define TECO_M_EGINH    0x10            /* Inhibit EG exits                 */
#define TECO_M_OUTDNE   0x20            /* Output was done (screen dirty)   */
#define TECO_M_MAKCTL   0x40            /* Make next character read into a  */
                                        /*  control character               */
        uint8_t         symspc;         /* EXTRA SYMBOL SPECIAL CHARACTER   */
        uint8_t         nmrbas;         /* RADIX (  0=>DECIMAL,             */
                                        /*          1=>OCTAL,               */
                                        /*          2=>HEXIDECIMAL)         */
        uint16_t        nwatch;         /* NUMBER OF LINES TO DISPLAY       */
        uint8_t         crtype;         /* SCOPE TYPE                       */
#define TECO_K_CRTYPE_VT52  0x00        /* VT52                             */
#define TECO_K_CRTYPE_VT61  0x01        /* VT61 [TECO-10 only]              */
#define TECO_K_CRTYPE_VT152 0x02        /* VT100 in VT52 mode               */
#define TECO_K_CRTYPE_VT100 0x04        /* VT100 in ANSI mode               */
#define TECO_K_CRTYPE_VT05  0x06        /* VT05                             */
#define TECO_K_CRTYPE_VT102 0x08        /* VT102 in ANSI mode               */
#define TECO_K_CRTYPE_VK100 0x0a        /* VK100 (GIGI)                     */
        uint8_t         crtyp2;         /* Scope type * 2                   */
        uint32_t        zmax;           /* TEXT BUFFER SIZE                 */
        uint8_t         *txstor;        /* TEXT BUFFER BIAS                 */
        uint32_t        qz;             /* Q-REG BUFFER SIZE IN USE         */
        QRGDEF          *qarray;        /* POINTER TO Q-REGISTER ARRAY      */
        QRGDEF          *qpntr;         /* COMMAND Q-REGISTER OFFSET        */
        uint32_t        indir;          /* FILOPN SET IF PROCESSING         */
                                        /*  INDIRECT COMMAND FILE           */
        TECFAB          *inpntr;        /* @INPNTR(R11) IS NON-ZERO IF      */
                                        /*  INPUT FILE ACTIVE               */
        TECFAB          *oupntr;        /* @OUPNTR(R11) IS NON-ZERO IF      */
                                        /*  OUTPUT FILE ACTIVE              */
        uint32_t        curfre;         /* CURRENT FREE SPACE IN BYTES      */
        uint32_t        qlcmd;          /*  SIZE OF LAST COMMAND            */
        sigjmp_buf      tecosp;         /* ERROR_MESSAGE unwind buffer      */
        QRGDEF          schbuf;         /* SEARCH BUFFER                    */
        QRGDEF          filbuf;         /* FILENAME BUFFER POINTER          */
        QRGDEF          tagbuf;         /* TAG BUFFER POINTER               */
        uint8_t         *errptr;        /* Error string pointer             */
        int32_t         errcod;         /* Last status code reported by     */
                                        /*  the interpreter                 */
        uintmax_t       syserr;         /* System-specific error code, set  */
                                        /*  when errcod is TECO__ERR        */
        uintmax_t       syserr2;        /* Extension for VMS (RMS STV)      */
    } TECODEF;

    typedef struct _scopedef {
        uint32_t        t_flags;        /* Terminal capability flags        */
#define TECO_M_TC_ANSI 0x1              /* 1-  ANSI terminal                */
#define TECO_M_TC_EDIT 0x2              /* 2-  Terminal has edit function   */
#define TECO_M_TC_SCD 0x4               /* 4-  Terminal can scroll down     */
#define TECO_M_TC_GRAPH 0x8             /* 8-  Terminal can do graphics     */
#define TECO_M_TC_MARK 0x10             /* 16- Terminal can do screen       */
                                        /*      marking                     */
#define TECO_M_TC_WIDTH 0x20            /* 32- Terminal can do change width */
#define TECO_M_TC_SCROLL 0x40           /* 64- Terminal has scrolling       */
                                        /*      region                      */
#define TECO_M_TC_EOS 0x80              /* 128-Terminal does erase to End   */
                                        /*      of screen                   */
#define TECO_M_TC_CURSOR 0x100          /* 256-Terminal has bold cursor     */
#define TECO_M_TC_SAVE 0x200            /* 512-Terminal can save/restore    */
                                        /*      cursor                      */
#define TECO_M_TC_FILL 0x400            /* 1024-Terminal requires fill      */
#define TECO_M_TC_GRPON 0x800           /* 2048-Terminal is in graphics     */
                                        /*       mode                       */
#define TECO_M_TC_MARKON 0x1000         /* 4096-Terminal is in "mask" mode  */
#define TECO_M_TC_INSERT 0x2000         /* 8192-Terminal is in "insert"     */
                                        /*       mode                       */
        void            *outchr;        /* -> "OUTPUT A CHARACTER"          */
        void            *outbln;        /* -> "PROCESS END OF LINE"         */
        void            *buindx;        /* -> TO LINE START IN MAP/CHECKING */
                                        /*  BUFFER                          */
        void            *curpos;        /* TEXT BUFFER POINTER OF CURSOR    */
        void            *curabs;        /* MAP POSITION OF CHARACTER UNDER  */
                                        /*  CURSOR                          */
        uint32_t        curlin;         /* LINE NUMBER OF CURSOR            */
        uint32_t        curcol;         /* COLUMN NUMBER OF CURSOR          */
        void            *mrklow;        /* START OF MARK'D REGION IN TEXT   */
                                        /*  BUFFER                          */
        void            *mrkhgh;        /* END OF MARK'D REGION IN TEXT     */
                                        /*  BUFFER                          */
        int32_t         rtmost;         /* RIGHTMOST COLUMN SEEN            */
        int32_t         ovrpos;         /* COLUMN POSITION +1 OF "OVRSYM"   */
        int32_t         bustrt;         /* CURRENT LINE NUMBER              */
        void            *linptr;        /* CHARACTER POSITION (P) OF LINE   */
                                        /*  START                           */
        int32_t         svstrt;         /* SAVED LINE NUMBER (BUSTRT)       */
        int32_t         initfl;         /* -1=>KNOWN; -2=>UNKNOWN, BUT      */
                                        /*  FIXED, 0=>UNKNOWN               */
        int32_t         topdot;         /* LAST TOP-OF-SCREEN POSITION      */
                                        /*  [SET/READ VIA 6:W]              */
        int32_t         hldflg;         /* "HOLD SCREEN" FLAG               */
                                        /*  [SET/READ VIA 5:W]              */
        int32_t         prelin;         /* LAST SET LINE TO SCREEN          */
                                        /*  [<0 =>UNKNOWN]                  */
        int32_t         precol;         /* LAST SET COLUMN TO SCREEN        */
        int32_t         mrkflg;         /* ACTIVE "MARK" IF ANY             */
                                        /*  [SET/READ VIA 4:W]              */
        int32_t         scroln;         /* SIZE OF SCROLLED REGION IF ANY   */
                                        /*  [SET/READ VIA 7:W]              */
        int16_t         seeall;         /* "SEE ALL CHARACTERS" FLAG        */
                                        /*  [SET/READ VIA 3:W               */
        int16_t         scrflg;         /* "SCROLLING ALLOWED" FLAG         */
                                        /*  [SET/READ VIA 8:W]              */
        int32_t         vtsize;         /* VERTICAL SIZE [SET/READ VIA 2:W] */
        int32_t         htsize;         /* HORIZONTAL SIZE                  */
                                        /*  [SET/READ VIA 2:W]              */
        int32_t         vtsiz1;         /* VERTICAL SIZE -1                 */
        int32_t         htsiz1;         /* HORIZONTAL SIZE -1               */
        int32_t         htsiz2;         /* HORIZONTAL SIZE -2               */
        int32_t         mapsiz;         /* -(SIZE OF THE SCREEN MAP) IN     */
                                        /*  BYTES (0 => UNALLOCATED)        */
        uint32_t        maplng;         /* Size of map [Positive]           */
        void            *mapptr;        /* POINTER TO FIRST SCREEN MAP      */
        void            *mapend;        /* Pointer to end of map            */
        void            *newmap;        /* POINTER TO SECOND SCREEN MAP     */
        void            *newptr;        /* Pointer to new screen map line   */
                                        /*  array                           */
        void            *oldptr;        /* Pointer to old screen map line   */
                                        /*  array                           */
        void            *cost;          /* Pointer to cost calculation      */
                                        /*  array                           */
/*+
 *      Pointers to terminal sequences
-*/
        uint8_t         *seqfix;        /* Fixup screen                     */
        uint8_t         *seqhom;        /* Cursor to home                   */
        uint8_t         *seqeol;        /* Erase to End of Line             */
        uint8_t         *seqeos;        /* Erase to end of screen           */
        uint8_t         *seqscu;        /* Scroll up                        */
        uint8_t         *seqscd;        /* Scroll down                      */
        uint8_t         *seqgon;        /* Graphics on                      */
        uint8_t         *seqgof;        /* Graphics off                     */
        uint8_t         *seqmon;        /* Mark on                          */
        uint8_t         *seqmof;        /* Mark off                         */
        uint8_t         *seqcon;        /* Cursor on                        */
        uint8_t         *seqcof;        /* Cursor off                       */
        uint8_t         *seqerc;        /* Erase character ?                */
        uint8_t         *seqerl;        /* Erase line                       */
        uint8_t         *scur;          /* Save cursor                      */
        uint8_t         *rcur;          /* Restore cursor                   */
/*+
 *      Graphics Mode Characters
 *
 *      EACH TABLE CONTAINS 15. BYTES CORRESPONDING TO THE GRAPHICS
 *      MODE CHARACTERS FOR:
 *
 *           1) A LINE WHICH OVERFLOWS THE SCREEN'S WIDTH
 *           2) CURSOR POSITIONED ON <LF> IMMEDIATELY AFTER <CR>
 *              ALSO, "SEE ALL" MODE SYMBOL FOR <LF>
 *           3) "SEE ALL" MODE SYMBOL FOR CONTROL CHARACTER FLAG
 *           4) "SEE ALL" MODE SYMBOL FOR <VT>
 *           5) AT END OF BUFFER ENDING WITHOUT <FF>
 *           6) AT END OF BUFFER ENDING WITH <FF>
 *              ALSO, "SEE ALL" MODE SYMBOL FOR <FF>
 *           7) "SEE ALL" MODE SYMBOL FOR <HT>
 *           8) "SEE ALL" MODE SYMBOL FOR NULL SPACING
 *           9) "SEE ALL" MODE SYMBOL FOR <CR>
 *          10) "SEE ALL" MODE SYMBOL FOR OVERPRINT <CR>
 *          11) "SEE ALL" MODE SYMBOL FOR HEX PAIR START
 *          12) "SEE ALL" MODE SYMBOL FOR COMPOSE SEQUENCE START
 *          13) "SEE ALL" MODE SYMBOL FOR HEX PAIR END
 *          14) "SEE ALL" MODE SYMBOL FOR COMPOSE SEQUENCE END
 *          15) "SEE ALL" MODE SYMBOL FOR <ESC>
 *
 *      This table *must* be initialised.  The secquences seqgon and seqgof
 *      will be used before and after outputing one of these characters
 *      if they are not NULL.
-*/
#define GRPTBL_MAX 15
        uint8_t         **graph;        /* Graphics char table              */
        uint8_t         *seqdca;        /* Direct cursor addressing         */
        uint8_t         *seqcrt;        /* Cursor right                     */
        uint8_t         *seqclf;        /* Cursor left                      */
        uint8_t         *seqcup;        /* Cursor up                        */
        uint8_t         *seqcdn;        /* Cursor down                      */
        uint8_t         *savcur;        /* Save cursor                      */
        uint8_t         *rescur;        /* Restore cursor                   */
    } SCOPEDEF;

/*
** Operating System interface block.
*/

    typedef struct _io_support {
        void    (*init0)    ();         /* early initialisation             */
        int32_t (*init)     ();         /* terminal setup                   */
        int32_t (*restore)  ();         /* restore terminal to before teco  */
        int32_t (*output)   ();         /* Do terminal output               */
        int32_t (*input)    ();         /* get terminal input               */
        int32_t (*flush)    ();         /* flush output                     */
        int32_t (*getbf)    ();         /* get input                        */
        int32_t (*putbf)    ();         /* put output                       */
	int32_t (*rewfl)    ();	        /* rewind output		    */
        int32_t (*ejflg)    ();         /* get ej flag information          */
        int32_t (*etflg)    ();         /* inform terminal of ET flag       */
        int32_t (*getcmd)   ();         /* get command line into Q-reg Z    */
        int32_t (*gexit)    ();         /* process special functions        */
        int32_t (*getfl)    ();         /* get files opened                 */
#define TECO_K_GETFL_READ      1        /*  Open file for read access       */
#define TECO_K_GETFL_WRITE     2        /*  Open file for write access      */
        int32_t (*eofl)     ();         /* test if at end-of-file           */
        int32_t (*clsfl)    ();         /* close input & output files       */
        int32_t (*en_preset)();         /* preset the "en" specification    */
        int32_t (*en_next)  ();         /* get next "en" occurance          */
        int32_t (*set_filename)();      /* set a file name, etc.            */
        int32_t (*syserr)   ();         /* return system error code         */
        int32_t (*crtset)   ();         /* configure sequences in scope     */
    } IO_SUPPORT;

#endif /* __TECODEF_LOADED */

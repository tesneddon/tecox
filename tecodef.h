/*
**++
**  MODULE DESCRIPTION:
**
**      TECO internal constant and structure definitions.
**
**  AUTHOR:             Tim E. Sneddon
**
**  Copyright (C) 2012 Tim E. Sneddon <tim@sneddon.id.au>
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
**  CREATION DATE:      01-FEB-2004
**
**  MODIFICATION HISTORY:
**
**      01-FEB-2004 V41.00  Sneddon   Initial coding.
**	08-NOV-2011 V41.01  Sneddon   Remove dispatch stuff from previous port.
**  	    	    	    	      Add TT driver structure.
**	10-NOV-2011 V41.02  Sneddon   Changes to radix constants.
**	11-NOV-2011 V41.03  Sneddon   Add np back to TECODEF.
**	15-DEC-2011 V41.04  Sneddon   Add IO_SUPPORT as a replacement and
**				      extension of TT driver structure.
**	31-JAN-2012 V41.05  Sneddon   Changed all flags in TECODEF to be
**				      uint32_t.
**	01-FEB-2012 V41.06  Sneddon   Moving external routine declarations in
**				      here as well.
**	07-MAR-2012 V41.07  Sneddon   Added x86-64/amd64.
**--
*/
#ifndef __TECODEF_LOADED
#define __TECODEF_LOADED 1

#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

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
 * Computer (m)		Operating System (n)
 * -------------------------------------------------------
 * 0	PDP-11		0	RSX-11D
 * 			1	RSX-11M
 *			2	RSX-11S
 *			3	IAS
 *			4	RSTS/E
 *			5	VAX/VMS (compatability mode)
 *			6	RSX-11M+
 *			7	RT-11
 * 1	PDP-8		0	OS/8
 * 2	DEC-10		0	TOPS-10
 * 3	DEC-20		0	TOPS-20
 * 4	VAX		0	OpenVMS VAX (native mode)
 *				OpenVMS Alpha (compatability mode)
 *				OpenVMS I64 (compatability mode)
 *			1	OpenBSD
 *			2	Ultrix-32
 * 5	Alpha		0	OpenVMS Alpha
 *			1	OpenBSD
 *			2	OSF/1
 *				Digital UNIX
 *				Tru64 UNIX
 *			3	FreeBSD
 * 6	IA-64		0	OpenVMS I64
 *			1	HP-UX
 *			2	GNU/Linux
 * 7	PA-RISC		0	HP-UX
 *			1	GNU/Linux
 * 8	x86		0	GNU/Linux
 *			1	OpenBSD
 *			2	Solaris
 *			3	FreeBSD
 *			4	NetBSD
 *			5	Plan 9
 *			6	Darwin
 * 9	SPARC		0	Solaris
 * 10	MIPS		0	Ultrix
 *			1	OSF/1
 * 11	Cray		0	UNICOS
 * 12	PowerPC		0	MacOS X
 *			1	Darwin
 * 13   x86-64		0	GNU/Linux
 */
#if (!defined(TECO_K_WORD_SIZE))
# error "TECO word size has not been defined"
#endif
#if (defined(vax) || defined(VAX) || defined(__vax) || defined(__VAX))
# if (TECO_K_WORD_SIZE != 32)
#  error "unsupported word size for VAX architecture"
# endif
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
#  endif
# elif (defined(__OpenBSD__))
#  define TECO_K_OS 1
# elif (defined(ultrix))
#  define TECO_K_OS 2
# else
#  error "unknown OS on VAX architecture"
# endif
#elif (defined(__alpha) || defined(__ALPHA) || defined(__Alpha_AXP))
# if ((TECO_K_WORD_SIZE != 32) && (TECO_K_WORD_SIZE != 64))
#  error "unsupported word size for Alpha architecture"
# endif
# define TECO_K_ARCH 5
# if (defined(vms) || defined(VMS) || defined(__vms) || defined(__VMS))
#  define TECO_K_OS 0
#  include <inttypes.h>
#  if (TECO_K_WORD_SIZE == 32)
#   undef TECO_K_ARCH
#   define TECO_K_ARCH 4
#  endif
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
# if ((TECO_K_WORD_SIZE != 32) && (TECO_K_WORD_SIZE != 64))
#  error "unsupported word size for IA-64 architecture"
# endif
# define TECO_K_ARCH 6
# if (defined(vms) || defined(VMS) || defined(__vms) || defined(__VMS))
#  define TECO_K_OS 0
#  include <inttypes.h>
#  if (TECO_K_WORD_SIZE == 32)
#   undef TECO_K_ARCH
#   define TECO_K_ARCH 4
#  endif
# elif (defined(__hpux))
#  define TECO_K_OS 1
# else
#  error "unknown OS on IA-64 architecture"
# endif
#elif (defined(__hppa) || defined(__hppa__))
# define TECO_K_ARCH 7
# if (defined(__hpux))
#  define TECO_K_OS 0
# elif ((defined(linux) || (defined(__linux) || defined(__linux__))
#  define TECO_K_OS 1
#  include <stdint.h>
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
# else
#  error "unknown OS on x86-64 architecture"
# endif
#else
# error "unknown architecture"
#endif

/*
** Define machine word size data type.
*/
#if (TECO_K_WORD_SIZE == 16)
# error "16-bit systems may be supported soon..."
#elif (TECO_K_WORD_SIZE == 32)
    typedef int32_t intXX_t;
    typedef uint32_t uintXX_t;
#elif (TECO_K_WORD_SIZE == 64)
    typedef int64_t intXX_t;
    typedef uint64_t uintXX_t;
#else
# error "word size unsupported by TECO"
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
**	Define q-register data structure
*/
    typedef struct _qrgdef {
	struct _qrgdef	*qrg_next;	/* Q-register next in PDL	    */
	intXX_t		qrg_value;	/* Q-register's 'VALUE'		    */
    	uint8_t		*qrg_ptr;	/* Q-register's Text Pointer	    */
    	uint16_t	qrg_size;	/* Q-register's size		    */
    	uint16_t	qrg_alloc;	/* Q-register's space alloc	    */
    	uint8_t		qrg_flags;	/* Q-register flags		    */
#define TECO_M_QRG_REF 0x1		/* Has been pushed		    */
#define TECO_M_QRG_LOCAL 0x2		/* It's a local q-register	    */
    	uint8_t		qrg_num;	/* Original q-reg number	    */
    	uint16_t	qrg_space;	/* Spare			    */
    } QRGDEF;

#define TECO_K_QRG_EXTENT 256		/* Q-reg storage allocation extent  */
#define TECO_K_NUMQRG ((('Z' - 'A') + 1) + (('9' - '0') + 1))
					/* NUMBER OF Q-REGS PER GROUP 	    */
					/*  (A-Z,0-9)			    */
#define TECO_K_QRG_FAKE (TECO_K_NUMQRG)	/* REGISTER FOR TEC$DO_COMMAND	    */

/*
**	CHARACTER DEFINITIONS
*/
#define TECO_C_NUL	'\0'	/* ASCII NULL				    */
#define TECO_C_SOH	'\001'	/* ASCII START OF HEADING		    */
#define TECO_C_ETX	'\003'	/* ASCII END OF TEXT (CONTROL/C)	    */
#define TECO_C_EOT	'\004'	/* ASCII END OF TRANSMISSION (CONTROL/D)    */
#define TECO_C_ENQ	'\005'	/* ASCII ENQUIRY (CONTROL/E)		    */
#define TECO_C_BEL	'\007'	/* ASCII BELL (CONTROL/G)		    */
#define TECO_C_BS	'\b'	/* ASCII BACKSPACE			    */
#define TECO_C_TAB	'\t'	/* ASCII HORIZONTAL TAB			    */
#define TECO_C_LF	'\n'	/* ASCII LINE FEED			    */
#define TECO_C_VT	'\v'	/* ASCII VERTICAL TAB			    */
#define TECO_C_FF	'\f'	/* ASCII FORM FEED			    */
#define TECO_C_CR	'\r'	/* ASCII CARRIAGE RETURN		    */
#define TECO_C_DC1 	'\021'	/* ASCII DEVICE CONTROL 1 (XON)	(CONTROL/Q) */
#define TECO_C_DC2	'\022'	/* ASCII DEVICE CONTROL 2 (CONTROL/R)	    */
#define TECO_C_NAK	'\025'	/* ASCII NEGATIVE ACK. (CONTROL/U)	    */
#define TECO_C_SYN	'\026'	/* ASCII SYNCHRONIZATION (CONTROL/V)	    */
#define TECO_C_ETB	'\027'	/* ASCII END OF TEXT BLOCK (CONTROL/W)	    */
#define TECO_C_EM	'\031'	/* ASCII END OF MEDIUM (CONTROL/Y)	    */
#define TECO_C_SUB	'\032'	/* ASCII SUBSTITUTE (CONTROL/Z)    	    */
#define TECO_C_ESC	'\033'	/* ASCII ESCAPE (ALSO CALLED ALTMODE)	    */
#define TECO_C_SPACE	' '	/* ASCII SPACE				    */
#define TECO_C_APS	'\''	/* ASCII APOSTROPHE			    */
#define TECO_C_LAB	'<'	/* ASCII LEFT ANGLE BRACKET		    */
#define TECO_C_EQU	'='	/* ASCII EQUAL SIZE			    */
#define TECO_C_RAB	'>'	/* ASCII RIGHT ANGLE BRACKET		    */
#define TECP_C_UND	'_'	/* ASCII UNDERSCORE (ALSO CALLED BACKARROW) */
#define TECO_C_GRV	'`'	/* ASCII ACCENT GRAVE			    */
#define TECO_C_LCB	'{'	/* ASCII LEFT CURLY BRACKET		    */
#define TECO_C_VBR	'|'	/* ASCII VERTICAL BAR			    */
#define TECO_C_RCB	'}'	/* ASCII RIGHT CURLY BRACKET		    */
#define TECO_C_TDE	'~'	/* ASCII FILDE				    */
#define TECO_C_DEL	'\177'	/* ASCII DELETE (ALSO CALLED RUBOUT)	    */
#define TECO_C_SS3	'\217'	/* Special shift 3			    */
#define TECO_C_CSI	'\233'	/* Control sequence introducer		    */

#define TECO__TRUE -1
#define TECO__FALSE 0

/* Flags that control the behaviour of the teco_scan() routine.
 */
#define TECO_M_SCAN_NONE   0x0
#define TECO_M_SCAN_UPPER  0x1
#define TECO_M_SCAN_LOWER  0x2
#define TECO_M_SCAN_TSTNXT 0x4
#define TECO_M_SCAN_INTERP 0x8

/*
** Flags to control the behaviour of push/pop
*/
#define TECO_K_PDL_ITR 0
#define TECO_K_PDL_MACRO 1
#define TECO_K_PDL_PAREN 2
    typedef struct _pdldef {            /* Push-down list block             */
    	struct _pdldef	*next;		/* Pointer to next block in stack   */
    	uint32_t	type;           /* Tyep of block PAREN,MACRO,ITR    */
    	uint8_t		*itrst;
    	uint32_t	itrcnt;
    	uint32_t	nopr;
    	int32_t		nacc;
    	uint8_t		*scanp;
    	uint32_t	mpdcnt;
    	QRGDEF		*qcmnd;
    	QRGDEF		*lclptr;
    } PDLDEF;

/* Flags to control the behaviour of teco_qref().
 */
#define TECO_K_QREF_NORMAL 0
#define TECO_K_QREF_EXTENDED 1

/* Flags to control the behaviour of the teco_type() and teco_print()
 * output routines.
 */
#define TECO_M_TYPE_T 0x0		/* Type without case flagging and   */
					/*  ringing bells		    */
#define TECO_M_TYPE_F 0x1		/* Type with case flagging	    */
#define TECO_M_TYPE_B 0x2		/* Type with ringing bells	    */

/* Flags to control zerod output mode.
 */
#define TECO_K_ZEROD_TERM 1		/* Write number to terminal	    */
#define TECO_K_ZEROD_TXBUF 2		/* Write number to text buffer	    */

/* Flags to control the behaviour of the teco_qskp() routine.
 */
#define TECO_K_QSKP_NONE 0		/* No quote restoration		    */
#define TECO_K_QSKP_IREST 1		/* Restore quote to ESC		    */
#define TECO_K_QSKP_SAVE 2		/* Save quote before processing and */
					/*  restore before return	    */

/* Memory zone identifiers.
 */
#define TECO_K_ZONE_GLOBAL 0
#define TECO_K_ZONE_LOCAL 1

#define TECO_K_INITIAL_ZMAX 8192

#define TECO_T_HELP_CMD "HELP"
#define TECO_K_HELP_SIZE (sizeof(TECO_T_HELP_CMD)-1)

#define TECO_K_CTRLZ_MAX 3
#define TECO_K_ESC_MAX 2

#define RADIX_DEC 0			/* Decimal radix		    */
#define RADIX_OCT 1			/* Octal radix			    */
#define RADIX_HEX 2			/* Hexidecimal radix		    */
#define RADIX_MAX 2

/*
**	Define the impure area, pointed to
**	by R11.
*/
    typedef struct _tecodef {
    	uint8_t		*scanp;		/* COMMAND LINE EXECUTION POINTER   */
    	uint32_t	mpdcnt;		/* MACRO PUSHDOWN COUNTER	    */
    	QRGDEF		*lclptr;	/* LOCAL LEVEL Q-REG ARRAY POINTER  */
    	uint8_t		*itrst;		/* ITERATION START		    */
    	uint32_t	itrcnt;		/* ITERATION COUNT		    */
    	uint32_t	nopr;		/* ARITHMETIC OPERATOR		    */
    	int32_t		nacc;		/* EXPRESSION ACCUMULATOR	    */
    	uint32_t	flags;		/* Flags word			    */
#define TECO_M_NFLG   0x1		/* NUMBER FLAG			    */
#define TECO_M_OFLG   0x2		/* OPERATOR FLAG		    */
#define TECO_M_CFLG   0x4		/* COMMA FLAG			    */
#define TECO_M_CLNF   0x8		/* COLON FLAG			    */
#define TECO_M_CLN2F  0x10		/* Two colons seen		    */
#define TECO_M_QFLG   0x20		/* QUOTED STRING FLAG		    */
#define TECO_M_TFLG   0x40		/* TRACE FLAG			    */
#define TECO_M_STOP   0x80		/* STOP INDICATOR		    */
#define TECO_M_REPFLG 0x100		/* REPLACE FLAG			    */
    	uint32_t	temp;		/* GENERAL TEMPORARY READ/WRITE     */
					/*  WORD			    */
    	uint8_t		*oscanp;	/* BACKUP FOR "SCANP"		    */
    	uint8_t		quote;		/* QUOTE CHARACTER 		    */
					/*  (NORMALLY ESCAPE)		    */
    	uint8_t		__fill_1;
    	uint16_t	pcnt;		/* PAREN PUSH COUNTER		    */
    	int32_t		m;		/* NUMBER			    */
    	int32_t		n;		/*  ARGUMENTS			    */
    	int32_t		np;		/* VALUE OF CURRENT NUMBER	    */
    	uint16_t	qlengt;		/* COMMAND LINE LENGTH		    */
    	uint16_t	cndn;		/* COUNTER FOR " NESTING	    */
    	uint8_t		*pst;		/* CHARACTER POSITION AT SEARCH	    */
					/*  START			    */
/* ------------------------------------	   END OF EACH COMMAND CLEAR AREA   */
    	QRGDEF		*qcmnd;		/* COMMAND LINE OR MACRO Q-REG	    */
					/*  NUMBER			    */
    	QRGDEF		*qnmbr;		/* CURRENT Q-REG NUMBER		    */
    	PDLDEF		*pdl;		/* MACRO PUSH-DOWN LIST		    */
    	QRGDEF		*qpdl;		/* Q-REGISTER PUSH-DOWN LIST	    */
    	uint32_t	p;		/* CURRENT TEXT POINTER (.)	    */
    	uint32_t	zz;		/* TEXT BUFFER SIZE IN USE	    */
    	uint32_t	errpos;		/* ERROR POSITION		    */
    	uint32_t	lschsz;		/* -(LENGTH) OF LAST SKIPPED	    */
    	uint32_t	evflag;		/* EDIT VERIFY FLAG		    */
	uint32_t	euflag;		/* CASE FLAGGING FLAG		    */
    	uint32_t	etype;		/* EDIT TYPEOUT FLAG		    */
#define TECO_M_ET_BIN 0x1		/*    +1., output in binary (image) */
					/*	   mode			    */
#define TECO_M_ET_CRT 0x2		/*    +2., do scope type rubout on  */
					/*	   control/u		    */
#define TECO_M_ET_LC 0x4		/*    +4., accept lower case input  */
#define TECO_M_ET_NCH 0x8		/*    +8., no echo during input for */
					/*	   ctrl/t		    */
#define TECO_M_ET_CCO 0x10		/*   +16., cancel control/o on	    */
					/*	   output		    */
#define TECO_M_ET_CKE 0x20		/*   +32., return -1 if error/no    */
					/*	   input on ctrl/t	    */
#define TECO_M_ET_DET 0x40		/*   +64., detach and detached flag */
#define TECO_M_ET_XIT 0x80		/*  +128., "no prompt yet" flag	    */
#define TECO_M_ET_TRU 0x100		/*  +256., truncate long output	    */
					/*	   lines		    */
#define TECO_M_ET_IAS 0x200		/*  +512., interactive scope	    */
					/*	   available for "watch"    */
#define TECO_M_ET_RFS 0x400		/* +1024., refresh scope available  */
#define TECO_M_TECO8 0x800		/* +2048., reserved for TECO-8	    */
#define TECO_M_ET_UNIX 0x800		/* +2048., enable UNIX mode on UNIX */
#define TECO_M_ET_8BT 0x1000		/* +4096., terminal is an 8-bit	    */
					/*	   terminal		    */
#define TECO_M_ET_GRV 0x2000		/* +1892., accept "`" as escape	    */
					/*	   during command input	    */
#define TECO_M_ET_UNU 0x4000		/*+16384., unused		    */
#define TECO_M_ET_CC 0x8000		/*+32768., allow program trap	    */
					/*	   control/c		    */
    	uint32_t	esflag;		/* EDIT SEARCH FLAG		    */
    	uint32_t	ehelp;		/* EDIT HELP LEVEL		    */
    	uint32_t	edit;		/* EDIT LEVEL FLAG		    */
#define TECO_M_ED_CTL 0x1		/*  +1., don't allow "^" as meaning */
					/*	 control character	    */
#define TECO_M_ED_TNK 0x2		/*  +2., allow yanks, etc. to	    */
					/*	 clobber text buffer	    */
#define TECO_M_ED_EXP 0x4		/*  +4., don't allow arbitrary	    */
					/*	 expansion(s)		    */
#define TECO_M_ED_TECO8 0x8		/*  +8., not used by TECO-11	    */
#define TECO_M_ED_SRH 0x10		/* +16., don't reset "dot" on	    */
					/*	 search failure		    */
#define TECO_M_ED_IMD 0x20		/* +32., allow immediate mode	    */
					/*	 commands		    */
#define TECO_M_ED_INC 0x40		/* +64., only move "dot" by one on  */
					/*	 iterative search	    */
#define TECO_M_ED_WCH 0x80		/*+128., don't allow automatic "w"  */
					/*	 command before prompt	    */
    	uint16_t	sflg;		/* SEARCH MODE FLAG		    */
    	uint16_t	eeflg;		/* Alternate escape character	    */
    	uint16_t	flags2;		/* SECONDARY FLAG WORD		    */
#define TECO_M_FFFLAG 0x1		/* FORM FEED FLAG		    */
#define TECO_M_EOFLAG 0x2		/* END-OF-FILE FLAG		    */
#define TECO_M_EXITOK 0x4		/* EXIT WITH DIRY BUFFER IS OKAY    */
#define TECO_M_EXITEND 0x8		/* EXIT AT END OF COMMAND	    */
#define TECO_M_EGINH 0x10		/* Inhibit EG exits		    */
#define TECO_M_OUTDNE 0x20		/* Output was done (screen dirty)   */
#define TECO_M_MAKCTL 0x40		/* Make next character read into a  */
					/*  control character		    */
    	uint8_t		symspc;		/* EXTRA SYMBOL SPECIAL CHARACTER   */
    	uint8_t		nmrbas;		/* RADIX (  0=>DECIMAL,		    */
					/*	    1=>OCTAL,		    */
					/*	    2=>HEXIDECIMAL)	    */
    	uint16_t	nwatch;		/* NUMBER OF LINES TO DISPLAY	    */
    	uint8_t		crtype;		/* SCOPE TYPE			    */
#define TECO_K_CRTYPE_VT52  0x00	/* VT52				    */
#define TECO_K_CRTYPE_VT61  0x01	/* VT61 [TECO-10 only]		    */
#define TECO_K_CRTYPE_VT152 0x02	/* VT100 in VT52 mode		    */
#define TECO_K_CRTYPE_VT100 0x04	/* VT100 in ANSI mode		    */
#define TECO_K_CRTYPE_VT05  0x06	/* VT05				    */
#define TECO_K_CRTYPE_VT102 0x08	/* VT102 in ANSI mode  		    */
#define TECO_K_CRTYPE_VK100 0x0a	/* VK100 (GIGI)			    */
    	uint8_t		crtyp2;		/* Scope type * 2		    */
    	uint32_t	zmax;		/* TEXT BUFFER SIZE		    */
    	uint8_t		*txstor;	/* TEXT BUFFER BIAS		    */
    	uint32_t	qz;		/* Q-REG BUFFER SIZE IN USE	    */
    	QRGDEF		*qarray;	/* POINTER TO Q-REGISTER ARRAY	    */
    	QRGDEF		*qpntr;		/* COMMAND Q-REGISTER OFFSET	    */
    	uint32_t	indir;		/* FILOPN SET IF PROCESSING	    */
					/*  INDIRECT COMMAND FILE	    */
    	void		**inpntr;	/* @INPNTR(R11) IS NON-ZERO IF      */
					/*  INPUT FILE ACTIVE		    */
    	void		**oupntr;	/* @OUPNTR(R11) IS NON-ZERO IF	    */
					/*  OUTPUT FILE ACTIVE		    */
    	uint32_t	curfre;		/* CURRENT FREE SPACE IN BYTES	    */
    	uint32_t	qlcmd;		/*  SIZE OF LAST COMMAND	    */
    	sigjmp_buf	tecosp;		/* ERROR_MESSAGE unwind buffer	    */
    	QRGDEF		schbuf;		/* SEARCH BUFFER		    */
    	QRGDEF		filbuf;		/* FILENAME BUFFER POINTER	    */
    	QRGDEF		tagbuf;		/* TAG BUFFER POINTER		    */
    	uint8_t		*errptr;	/* Error string pointer		    */
    	int32_t		errcod;		/* Last status code reported by	    */
					/*  the interpreter		    */
    	uintXX_t	syserr;		/* System-specific error code, set  */
					/*  when errcod is TECO__ERR	    */
    	uintXX_t	syserr2;	/* Extension for VMS (RMS STV)	    */
    } TECODEF;

    typedef struct _scopedef {
    	uint32_t	t_flags;	/* Terminal capability flags	    */
#define TECO_M_TC_ANSI 0x1		/* 1-  ANSI terminal		    */
#define TECO_M_TC_EDIT 0x2		/* 2-  Terminal has edit function   */
#define TECO_M_TC_SCD 0x4		/* 4-  Terminal can scroll down	    */
#define TECO_M_TC_GRAPH 0x8		/* 8-  Terminal can do graphics	    */
#define TECO_M_TC_MARK 0x10		/* 16- Terminal can do screen 	    */
					/*	marking			    */
#define TECO_M_TC_WIDTH 0x20		/* 32- Terminal can do change width */
#define TECO_M_TC_SCROLL 0x40		/* 64- Terminal has scrolling	    */
					/*	region			    */
#define TECO_M_TC_EOS 0x80		/* 128-Terminal does erase to End   */
					/*	of screen		    */
#define TECO_M_TC_CURSOR 0x100		/* 256-Terminal has bold cursor	    */
#define TECO_M_TC_SAVE 0x200		/* 512-Terminal can save/restore    */
					/*	cursor			    */
#define TECO_M_TC_FILL 0x400		/* 1024-Terminal requires fill	    */
#define TECO_M_TC_GRPON 0x800		/* 2048-Terminal is in graphics	    */
					/*	 mode			    */
#define TECO_M_TC_MARKON 0x1000		/* 4096-Terminal is in "mask" mode  */
#define TECO_M_TC_INSERT 0x2000		/* 8192-Terminal is in "insert"	    */
					/*	 mode			    */
    	void		*outchr;	/* -> "OUTPUT A CHARACTER"	    */
    	void		*outbln;	/* -> "PROCESS END OF LINE"	    */
    	void		*buindx;	/* -> TO LINE START IN MAP/CHECKING */
					/*  BUFFER			    */
    	void		*curpos;	/* TEXT BUFFER POINTER OF CURSOR    */
    	void		*curabs;	/* MAP POSITION OF CHARACTER UNDER  */
					/*  CURSOR			    */
    	uint32_t	curlin;		/* LINE NUMBER OF CURSOR	    */
    	uint32_t	curcol;		/* COLUMN NUMBER OF CURSOR	    */
    	void		*mrklow;	/* START OF MARK'D REGION IN TEXT   */
					/*  BUFFER			    */
    	void		*mrkhgh;	/* END OF MARK'D REGION IN TEXT	    */
					/*  BUFFER			    */
    	int32_t		rtmost;		/* RIGHTMOST COLUMN SEEN	    */
    	int32_t		ovrpos;		/* COLUMN POSITION +1 OF "OVRSYM"   */
    	int32_t		bustrt;		/* CURRENT LINE NUMBER		    */
    	void		*linptr;	/* CHARACTER POSITION (P) OF LINE   */
					/*  START			    */
    	int32_t		svstrt;		/* SAVED LINE NUMBER (BUSTRT)	    */
    	int32_t		initfl;		/* -1=>KNOWN; -2=>UNKNOWN, BUT	    */
					/*  FIXED, 0=>UNKNOWN		    */
    	int32_t		topdot;		/* LAST TOP-OF-SCREEN POSITION	    */
					/*  [SET/READ VIA 6:W]		    */
    	int32_t		hldflg;		/* "HOLD SCREEN" FLAG		    */
					/*  [SET/READ VIA 5:W]		    */
    	int32_t		prelin;		/* LAST SET LINE TO SCREEN	    */
					/*  [<0 =>UNKNOWN]		    */
    	int32_t		precol;		/* LAST SET COLUMN TO SCREEN	    */
    	int32_t		mrkflg;		/* ACTIVE "MARK" IF ANY		    */
					/*  [SET/READ VIA 4:W]		    */
    	int32_t		scroln;		/* SIZE OF SCROLLED REGION IF ANY   */
					/*  [SET/READ VIA 7:W]		    */
    	int16_t		seeall;		/* "SEE ALL CHARACTERS" FLAG	    */
					/*  [SET/READ VIA 3:W		    */
    	int16_t		scrflg;		/* "SCROLLING ALLOWED" FLAG	    */
					/*  [SET/READ VIA 8:W]		    */
    	int32_t		vtsize;		/* VERTICAL SIZE [SET/READ VIA 2:W] */
    	int32_t		htsize;		/* HORIZONTAL SIZE		    */
					/*  [SET/READ VIA 2:W]		    */
    	int32_t		vtsiz1;		/* VERTICAL SIZE -1		    */
    	int32_t		htsiz1;		/* HORIZONTAL SIZE -1		    */
    	int32_t		htsiz2;		/* HORIZONTAL SIZE -2		    */
    	int32_t		mapsiz;		/* -(SIZE OF THE SCREEN MAP) IN	    */
					/*  BYTES (0 => UNALLOCATED)	    */
    	uint32_t	maplng;		/* Size of map [Positive]	    */
    	void		*mapptr;	/* POINTER TO FIRST SCREEN MAP	    */
    	void		*mapend;	/* Pointer to end of map	    */
    	void		*newmap;	/* POINTER TO SECOND SCREEN MAP	    */
    	void		*newptr;	/* Pointer to new screen map line   */
					/*  array			    */
    	void		*oldptr;	/* Pointer to old screen map line   */
					/*  array			    */
    	void		*cost;		/* Pointer to cost calculation	    */
					/*  array			    */
/*+
 *	Pointers to terminal sequences
-*/
    	uint8_t		*seqfix;	/* Fixup screen			    */
    	uint8_t		*seqhom;	/* Cursor to home		    */
    	uint8_t		*seqeol;	/* Erase to End of Line		    */
    	uint8_t		*seqeos;	/* Erase to end of screen	    */
    	uint8_t		*seqscu;	/* Scroll up			    */
    	uint8_t		*seqscd;	/* Scroll down			    */
    	uint8_t		*seqgon;	/* Graphics on			    */
    	uint8_t		*seqgof;	/* Graphics off			    */
    	uint8_t		*seqmon;	/* Mark on			    */
    	uint8_t		*seqmof;	/* Mark off			    */
    	uint8_t		*seqcon;	/* Cursor on			    */
    	uint8_t		*seqcof;	/* Cursor off			    */
    	uint8_t		*seqerc;	/* Erase character ?		    */
    	uint8_t		*seqerl;	/* Erase line			    */
    	uint8_t		*scur;		/* Save cursor			    */
    	uint8_t		*rcur;		/* Restore cursor		    */
    	uint8_t		*graph;		/* Graphics char table		    */
    	uint8_t		*seqdca;	/* Direct cursor addressing	    */
    	uint8_t		*seqcrt;	/* Cursor right			    */
    	uint8_t		*seqclf;	/* Cursor left			    */
    	uint8_t		*seqcup;	/* Cursor up			    */
    	uint8_t		*seqcdn;	/* Cursor down			    */
    } SCOPEDEF;

    typedef struct _io_support {
    	int32_t (*init)	    ();		/* terminal setup		    */
    	int32_t (*restore)  ();		/* restore terminal to before teco  */
    	int32_t (*output)   ();		/* Do terminal output		    */
    	int32_t (*input)    ();		/* get terminal input		    */
    	int32_t (*ejflg)    ();		/* get ej flag information	    */
    	int32_t (*etflg)    ();		/* inform terminal of ET flag       */
    	int32_t (*getcmd)   ();		/* get command line into Q-reg Z    */
    } IO_SUPPORT;

/*
** Useful macros...
*/
#define ERROR_MESSAGE(nnn) \
do { \
    ctx.errcod = TECO__##nnn; \
    raise(SIGUSR1); \
} while (0)

#define goto_unwind() siglongjmp(ctx.tecosp, ctx.errcod)
#define set_unwind() (int32_t) sigsetjmp(ctx.tecosp, 1)

/*
** teco.c
*/

    extern int32_t teco();
    extern void qset();

/*
** tecoio.c
*/

    extern void teco_type();
    extern uint8_t listen(uint32_t);
    extern void teco_delch(void);

#define TYPET 0x0	/* Type without case flagging and ringing bells */
#define TYPEF 0x1	/* Type with case flagging */
#define TYPEB 0x2	/* Type with ringing bells */
#define type(c) teco_type(TYPET, (c))
#define typb(c) teco_type(TYPEB, (c))
#define typf(c) teco_type(TYPEF, (c))
#define print(ptr, len) \
do { \
    uint32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        type(p[i]);  \
} while (0)
#define prinb(ptr, len) \
do { \
    uint32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        typb(p[i]); \
} while (0)
#define prinf(ptr, len) \
do { \
    uint32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        typf(p[i]); \
} while (0)

#endif /* __TECODEF_LOADED */

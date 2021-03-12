/*
**++
**  MODULE DESCRIPTION:
**
**      TECO internal macros used by the intrepreter.
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
**  CREATION DATE:      12-MAR-2021
**
**  MODIFICATION HISTORY:
**
**      12-MAR-2021 V41.00  Sneddon   Initial coding.
**--
*/
#ifndef __TECO_MACROS_LOADED
#define __TECO_MACROS_LOADED 1

#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include "tecochr.h"

#define ERROR_MESSAGE(nnn) \
    ERROR_CODE(TECO__##nnn)

#define ERROR_CODE(n) \
do { \
    ctx.errcod = n; \
    raise(SIGUSR1); \
} while (0)

#define goto_unwind() siglongjmp(ctx.tecosp, ctx.errcod)
#define set_unwind() (int32_t) sigsetjmp(ctx.tecosp, 1)

#define TYPET 0x0       /* Type without case flagging and ringing bells */
#define TYPEF 0x1       /* Type with case flagging */
#define TYPEB 0x2       /* Type with ringing bells */
#define type(c) \
do { \
    teco_type(TYPET, (c)); \
    io_support.flush(0); \
} while (0)
#define typb(c) \
do { \
    teco_type(TYPEB, (c)); \
    io_support.flush(0); \
} while (0)
#define typf(c) \
do { \
    teco_type(TYPEF, (c)); \
    io_support.flush(0); \
} while (0)
#define print(ptr, len) \
do { \
    int32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < (len); i++) \
        teco_type(TYPET, p[i]);  \
    io_support.flush(0); \
} while (0)
#define prinb(ptr, len) \
do { \
    int32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        teco_type(TYPEB, p[i]); \
    io_support.flush(0); \
} while (0)
#define prinf(ptr, len) \
do { \
    int32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        teco_type(TYPEF, p[i]); \
    io_support.flush(0); \
} while (0)
#define prinz(s) \
do { \
    int32_t len = strlen((s)); \
    print(s,len); \
} while (0)

#define bzchk(n) \
do { \
    if ((n) > ctx.zz) \
        ERROR_MESSAGE(POP); \
} while (0)

/*
** Clear the colon flag and preset with TECO__TRUE
** if it is found.
*/
#define chkclo() \
do { \
    if (ctx.flags & TECO_M_CLNF) { \
        ctx.flags &= ~TECO_M_CLNF; \
        ncom(TECO__TRUE); \
    } \
} while (0)

/*
** Clear any incoming number and check for colon.
*/
#define chkcln() \
do { \
    ctx.flags &= ~TECO_M_NFLG; \
    chkclo(); \
} while (0)

#define clnxit(status) \
do { \
    chkcln(); \
    if (((status) != 0) \
        && !(ctx.flags & TECO_M_FFFLAG)) { \
        ctx.n = TECO__FALSE; \
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

#define yankok() \
do { \
    if (ctx.oupntr != 0) { \
        if (!(ctx.flags & TECO_M_ED_YNK)) \
            ERROR_MESSAGE(YCA); \
    } \
} while (0)

/**
 * Clear and size up the text buffer
 */
#define yankc() \
do { \
    ctx.p = ctx.zz = 0; \
    yankf(); \
} while (0)

#define yankf() \
do { \
    ctx.flags &= ~TECO_M_FFFLAG; \
    txadj(ctx.curfre); \
} while (0)

#define mtque(q) \
    ((((struct qelem *))(q)->next == (struct qelem *)(q)) \
    && (((struct qelem *)(q))->prev == (struct qelem *)(q)))

#endif /* __TECO_MACROS_LOADED */

#ifndef macros_h__
#define macros_h__

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

#endif /* macros_h__ */

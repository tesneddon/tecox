#ifndef __TECO_EXTRN_LOADED
#define __TECO_EXTRN_LOADED 1
#include "tecodef.h"

/*
** crtrub.c
*/
    uint32_t decode();

/*
** teco.c
*/
    int32_t teco();
    int32_t teco_bakup();
    void teco_interp();
    void qset();
    void txadj();

/*
** tecoio.c
*/
    uint8_t listen();
    void teco_delch();
    int32_t teco_getfl();
    int32_t teco_init();
    int32_t teco_putbf();
    void teco_type();
    uint8_t tlistn();
    int32_t yyy_y();
    static int32_t yyy_c() { return 0; }
    static int32_t yyy_f() { return 0; }
/*
** tecomsg.c
*/
    void teco_putmsg();

#endif /* __TECO_EXTRN_LOADED */

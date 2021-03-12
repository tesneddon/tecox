/*
** extrn.h
**
**  Prototypes for internal functions used by the TECO interpreter.
**
**  Copyright 2021, Tim E. Sneddon <tim@sneddon.id.au>
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
**  CREATION DATE:      25-SEP-2012
**
**  MODIFICATION HISTORY:
**
**      25-SEP-2012 V41.00  Sneddon   Initial coding.
*/
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

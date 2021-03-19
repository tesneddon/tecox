/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains long and short definitions of all TECO
**  messages as well as support to print them.
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
**  CREATION DATE:      31-JAN-2005
**
**  MODIFICATION HISTORY:
**
**      31-JAN-2005 V41.00  Sneddon	  Initial coding.
**      02-OCT-2014 V41.01  Sneddon	  Updated to make use of catalogues.
**--
*/
#define MODULE TECOMSG
#define VERSION "V41.01"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include <errno.h>
#include <nl_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tecodef.h"
#define TECO_MESSAGES_INTERNAL
#include "tecomsg.h"
#include "extrn.h"
#include "globals.h"
#include "macros.h"

/*
** Forward Declarations.
*/

    void teco_putmsg(void);

void teco_putmsg() {

    MSGDEF *message = &TECO_MESSAGES[ctx.errcod];
    uint16_t eh, eh4;

    if ((ctx.errcod < TECO_MESSAGES_MIN)
        || (ctx.errcod > TECO_MESSAGES_MAX)) {
        /* This shouldn't happen...use system I/O call so we
         * don;t get stuck in a loop?.
         */ 
        printf("?SYS\tMessage code %d outside acceptable range", ctx.errcod);
        return;
    }

    message = &TECO_MESSAGES[ctx.errcod-1];

    /**
     * @note
     * EH (@ref ehelp) is treated as two fields.  The first is two bits
     * (@ref eh) and the second is 1 bit (@ref eh4).  Thus, if the first
     * field is 0, then it defaults to 2.
     */
    eh = ctx.ehelp & 0x0003;
    if (eh == 0) eh = 2;
    eh4 = ctx.ehelp & 0x0004;

    /*
    ** The error identifier is always printed.
    */
    type('?');  prinz(message->msg_name);

    if (eh & 2) {
        /*
        ** Print the error description.
        */
        char *buf = 0;

        type(TECO_C_TAB);
        if (ctx.errcod == TECO__ERR) {
            /*
            ** This is a system specific error code.  Need to call the
            ** OS interface to receive the message.
            */
            char *sysid, *sysmsg;

            io_support.syserr(&sysid, &sysmsg);
            asprintf(&buf ,message->msg_text, sysid, sysmsg);

            free(sysid);
            free(sysmsg);
        } else if (ctx.errcod == TECO__FNF) {
            /*
            ** This is a file not found, so we need to include the file
            ** name in the message output.
            */
            asprintf(&buf, message->msg_text, ctx.filbuf.qrg_size,
                     ctx.filbuf.qrg_size, ctx.filbuf.qrg_ptr);
        } else if (ctx.errcod == TECO__SRH) {
            /*
            ** Search failure includes the failed string in the
            ** output.
            */
            asprintf(&buf, message->msg_text, ctx.schbuf.qrg_size,
                     ctx.schbuf.qrg_size, ctx.schbuf.qrg_ptr);
        }

        if (buf != 0) {
            prinz(buf);
            free(buf);
        } else {
            prinz(message->msg_text);
        }

//        crlfno();

        if (eh & 1) {
            crlfno();
            crlfno();

            print("     ?", 6); prinz(message->msg_name);
            print("    ", 4); prinz(message->msg_text);
            crlfno();
            crlfno();
            prinz(message->msg_help);
            crlfno();
            crlfno();
        }
    }

    if (ctx.errpos && eh4) {
        crlfno();
        print(ctx.errptr, ctx.errpos);
        type('?');
    }
} /* teco_putmsg */

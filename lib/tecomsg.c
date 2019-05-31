/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains long and short definitions of all TECO
**  messages as well as support to print them.
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
#include "tecomsg.h"
#include "globals.h"
#include "macros.h"
#define ID_MAX 3

/*
** Message descriptor.  These messages are stored in set TECO__Set.  id is
** always three characters and never zero terminated.  text is variable
** length ans always zero terminated.
*/

    struct MSGDSC {
        char id[ID_MAX];
        char text[];
    };

/*
** Forward Declarations.
*/

    void teco_putmsg(void);

void teco_putmsg() {

    static nl_catd catalog = (nl_catd) -1;

    struct MSGDSC *message;
    char *help;
    uint16_t eh, eh4;

    if (catalog == (nl_catd) -1) {
        char *name = getenv("TECOMSG");

        /*
        ** Always open up the catalog on the first go round...
        */
        catalog = catopen(name == 0 || name[0] == '\0' ? "tecomsg" : name, 0);
        if (catalog == (nl_catd) -1) {
            printf("?SYS\t%s", strerror(errno));
        }
    }

    /*
    ** Fetch the id and basic text from the catalog.
    */
    message = (struct MSGDSC *)catgets(catalog, TECO__Set, ctx.errcod, 0);
    if (message == 0) {
        printf("?SYS\tUnable to load message catalogue");
    }

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
    type('?');  print(message->id, ID_MAX);

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
            asprintf(&buf ,message->text, sysid, sysmsg);

            free(sysid);
            free(sysmsg);
        } else if (ctx.errcod == TECO__FNF) {
            /*
            ** This is a file not found, so we need to include the file
            ** name in the message output.
            */
            asprintf(&buf, message->text, ctx.filbuf.qrg_size,
                     ctx.filbuf.qrg_size, ctx.filbuf.qrg_ptr);
        } else if (ctx.errcod == TECO__SRH) {
            /*
            ** Search failure includes the failed string in the
            ** output.
            */
            asprintf(&buf, message->text, ctx.schbuf.qrg_size,
                     ctx.schbuf.qrg_size, ctx.schbuf.qrg_ptr);
        }

        if (buf != 0) {
            print(buf, strlen(buf));
            free(buf);
        } else {
            print(message->text, strlen(message->text));
        }

//        crlfno();

        if (eh & 1) {
            crlfno();
            crlfno();

            help = catgets(catalog, TECO__HELP_Set, ctx.errcod, 0);
            if (help == 0) {
                // cope with the rror...
            } else {
                print("     ?", 6); print(message->id, ID_MAX);
                print("    ", 4); print(message->text, strlen(message->text));
                crlfno();
                crlfno();
                print(help, strlen(help));
                crlfno();
                crlfno();
            }
        }
    }

    if (ctx.errpos && eh4) {
        crlfno();
        print(ctx.errptr, ctx.errpos);
        type('?');
    }
} /* teco_putmsg */

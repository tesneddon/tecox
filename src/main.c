/*
**++
**  MODULE DESCRIPTION:
**
**	This module is the main entry point for TECO.
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
**  CREATION DATE:      09-OCT-2011
**
**  MODIFICATION HISTORY:
**
**      09-OCT-2011 V41.00  Sneddon	Initial coding.
**      06-JUN-2013 V41.01  Sneddon	Fixed silly bug in setting argc/argv.
**--
*/
#define MODULE TECOMAIN
#define VERSION "V41.01"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
# include <ssdef.h>
# include <stsdef.h>
#endif
#include <stdlib.h>
#include "tecodef.h"
#include "globals.h"

/*
** Forward Declarations
*/

    int main();
    static void exit_handler();

int main(c,
	 v)
    int c;
    char *v[];
{
    uint32_t status;

#if defined(unix) || defined(__unix__)
    /*
    ** On UNIX-ish systems we use this little hack to pass the command line
    ** pointers back to io_support.getcmd.  Wish we had LIB$GET_FOREIGN for
    ** UNIX...
    */
    extern int argc;
    extern char **argv;

    argc = c;
    argv = v;
#endif

    atexit(exit_handler);

    // previous port has a whole bunch of setup in here...check where that could
    // go, should it be in teco_init?  Look at the end of old TECO.C on VAX.

    status = teco();

#ifdef vms
    if (status != TECO__SUCCESS) {
	status = SS$_ABORT | STS$M_INHIB_MSG;
    } else {
	status = SS$_NORMAL;
    }
#endif

    return status;
}

static void exit_handler(void)
{
    // do a bunch of stuff :-)

    io_support.restore();
}

/*
**++
**  MODULE DESCRIPTION:
**
**	This module is the main entry point for TECO.
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
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied
**  See the License for the specific language governing permissions and
**  limitations under the License.
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
#include "extrn.h"
#include "globals.h"

/*
** Forward Declarations
*/

    int main();
    static void exit_handler();

int main(argc, argv)
    int argc;
    char *argv[];
{
    uint32_t status;

    io_support.init0(argc, argv);

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

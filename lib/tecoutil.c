/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains miscellaneous utility routines.
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
**  CREATION DATE:      09-OCT-2011
**
**  MODIFICATION HISTORY:
**
**      09-OCT-2011 V41.00  Sneddon	Initial coding.
**--
*/
#define MODULE TECOUTIL
#define VERSION "V41.00"
#ifdef vms
#  ifdef VAX11C
#    module MODULE VERSION
#  else
#    pragma module MODULE VERSION
#  endif
#endif
#include "tecodef.h"

/*
** Forward Declarations
*/

    int teco$start(int flag);

/*
**++
**  ROUTINE:	teco$start
**
**  FUNCTIONAL DESCRIPTION:
**
**	Turn control over to user.
**
**  RETURNS:	cond_value, longword, write only, by value
**
**  PROTOTYPE:
**
**	teco$start
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  COMPLETION CODES:
**
**  SIDE EFFECTS:
**
**--
*/
#if 0
int teco$start(flag)
    int *flag;
{
    // check arg count

    if (flag && *flag)
	ctx.flags2 |= TECO_M_EXITOK;

    return clean_up_and_start();
}
#endif

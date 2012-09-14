/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains miscellaneous utility routines.
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
int teco$start(flag)
    int *flag;
{
    // check arg count

    if (flag && *flag)
	ctx.flags2 |= TECO_M_EXITOK;

    return clean_up_and_start();
}

/*
** globals.h
**
**  External reference definitions for TECO globals.  These are defined as
**  globals in teco.c.
**
**  Copyright (C) 2012, Tim E. Sneddon <tim@sneddon.id.au>
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
**  CREATION DATE:      25-SEP-2012
**
**  MODIFICATION HISTORY:
**
**      25-SEP-2012 V41.00  Sneddon   Initial coding.
*/
#ifndef globals_h__
#define globals_h__
#include "tecodef.h"

    extern TECODEF      ctx;               /* TECO's context area      */
    extern SCOPEDEF     scope;             /* TECO's "screen" area     */
    extern IO_SUPPORT   io_support;        /* TECO's OS-specific block */

#endif /* globals_h__ */

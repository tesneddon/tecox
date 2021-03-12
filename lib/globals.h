/*
** globals.h
**
**  External reference definitions for TECO globals.  These are defined as
**  globals in teco.c.
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
**  CREATION DATE:      25-SEP-2012
**
**  MODIFICATION HISTORY:
**
**      25-SEP-2012 V41.00  Sneddon   Initial coding.
*/
#ifndef __TECO_GLOBALS_LOADED
#define __TECO_GLOBALS_LOADED 1
#include "tecodef.h"

    extern TECODEF      ctx;               /* TECO's context area      */
    extern SCOPEDEF     scope;             /* TECO's "screen" area     */
    extern IO_SUPPORT   io_support;        /* TECO's OS-specific block */

    extern QRGDEF       qreg_local[];

#endif /* __TECO_GLOBALS_LOADED */

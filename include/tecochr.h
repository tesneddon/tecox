/*
**++
**  MODULE DESCRIPTION:
**
**      TECO character definitions..
**
**  AUTHOR:             Tim E. Sneddon
**
**  Copyright (C) 2019 Tim E. Sneddon <tim@sneddon.id.au>
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
**  CREATION DATE:      16-MAY-2019
**
**  MODIFICATION HISTORY:
**
**      16-MAY-2019 V41.00  Sneddon   Initial coding.
**--
*/
#ifndef __TECOCHR_LOADED
#define __TECOCHR_LOADED 1

/*
**      CHARACTER DEFINITIONS
*/
#define TECO_C_NUL      '\0'    /* ASCII NULL                               */
#define TECO_C_SOH      '\001'  /* ASCII START OF HEADING                   */
#define TECO_C_ETX      '\003'  /* ASCII END OF TEXT (CONTROL/C)            */
#define TECO_C_EOT      '\004'  /* ASCII END OF TRANSMISSION (CONTROL/D)    */
#define TECO_C_ENQ      '\005'  /* ASCII ENQUIRY (CONTROL/E)                */
#define TECO_C_BEL      '\007'  /* ASCII BELL (CONTROL/G)                   */
#define TECO_C_BS       '\b'    /* ASCII BACKSPACE                          */
#define TECO_C_TAB      '\t'    /* ASCII HORIZONTAL TAB                     */
#define TECO_C_LF       '\n'    /* ASCII LINE FEED                          */
#define TECO_C_VT       '\v'    /* ASCII VERTICAL TAB                       */
#define TECO_C_FF       '\f'    /* ASCII FORM FEED                          */
#define TECO_C_CR       '\r'    /* ASCII CARRIAGE RETURN                    */
#define TECO_C_DLE      '\020'  /* ASCII DATA LINK ESCAPE                   */
#define TECO_C_DC1      '\021'  /* ASCII DEVICE CONTROL 1 (XON) (CONTROL/Q) */
#define TECO_C_DC2      '\022'  /* ASCII DEVICE CONTROL 2 (CONTROL/R)       */
#define TECO_C_NAK      '\025'  /* ASCII NEGATIVE ACK. (CONTROL/U)          */
#define TECO_C_SYN      '\026'  /* ASCII SYNCHRONIZATION (CONTROL/V)        */
#define TECO_C_ETB      '\027'  /* ASCII END OF TEXT BLOCK (CONTROL/W)      */
#define TECO_C_EM       '\031'  /* ASCII END OF MEDIUM (CONTROL/Y)          */
#define TECO_C_SUB      '\032'  /* ASCII SUBSTITUTE (CONTROL/Z)             */
#define TECO_C_ESC      '\033'  /* ASCII ESCAPE (ALSO CALLED ALTMODE)       */
#define TECO_C_FS       '\034'  /* ASCII FIELD SEPARATOR                    */
#define TECO_C_GS       '\035'  /* ASCII GROUP SEPARATOR                    */
#define TECO_C_SPACE    ' '     /* ASCII SPACE                              */
#define TECO_C_APS      '\''    /* ASCII APOSTROPHE                         */
#define TECO_C_LAB      '<'     /* ASCII LEFT ANGLE BRACKET                 */
#define TECO_C_EQU      '='     /* ASCII EQUAL SIZE                         */
#define TECO_C_RAB      '>'     /* ASCII RIGHT ANGLE BRACKET                */
#define TECP_C_UND      '_'     /* ASCII UNDERSCORE (ALSO CALLED BACKARROW) */
#define TECO_C_GRV      '`'     /* ASCII ACCENT GRAVE                       */
#define TECO_C_LCB      '{'     /* ASCII LEFT CURLY BRACKET                 */
#define TECO_C_VBR      '|'     /* ASCII VERTICAL BAR                       */
#define TECO_C_RCB      '}'     /* ASCII RIGHT CURLY BRACKET                */
#define TECO_C_TDE      '~'     /* ASCII FILDE                              */
#define TECO_C_DEL      '\177'  /* ASCII DELETE (ALSO CALLED RUBOUT)        */
#define TECO_C_SS3      '\217'  /* Special shift 3                          */
#define TECO_C_CSI      '\233'  /* Control sequence introducer              */

#endif /* __TECOCHR_LOADED */

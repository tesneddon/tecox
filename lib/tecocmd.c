/*
**++
**  MODULE DESCRIPTION:
**
**	This module contains the command decode logics for TECO
**  (written in TECO of course).  Line continuation is handled with
**  the C line terminator '\'.  All non-printable characters are
**  encoded using octal character constants (of course :-),
**  eg. ESC = \033.
**
**  AUTHOR:             Tim E. Sneddon
**
**  Copyright (C) 2012 Tim E. Sneddon <tim@sneddon.id.au>
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
**  CREATION DATE:      29-JAN-1985
**
**  MODIFICATION HISTORY:
**
**      29-JAN-1985 V40.00  Murphy	Initial coding.
**	21-OCT-1986 V40.11  Goldstein	Integrate into VMS system build.
**	04-SEP-1989 V40.12  Murphy	Save TECO.TEC input Q-registers (0,1,2)
**					and restore to allow re-entering TECO
**					without random behaviour.
**	09-NOV-2011 V41.00  Sneddon	Adapted this version from original
**					MACRO-32 source.
**--
*/
#define MODULE TECOCMD
#define VERSION "V41.00"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include "tecodef.h"

/*
** Initial command decoder (TECO.TEC)
*/
    uint8_t tecocmd[] = "\
!TECO.TEC V40.00![00U00,0X0[10U10,0X1[20U20,0X2[30U30,0X3[40U40,0X4\
[50U50,0X5[60U60,0X6[70U70,0X7[80U80,0X8[90U90,0X9\
[90,0X9[00,0X0[20,0X210U0[0\004\030U0[00\030\
EDU0[0ED&4EDEUU0[0-1EUESU0[00ESETU0[032768#32#8#1,0ETQ1U00U1[10,\
0X1-1EJ-4\"E:QZ#Z\"E-1\030G_J::FSdcl  teco\033TECO\033\"UH\
':S \033\033J0\030''Q0+1\"E32768U0'32768\"E:EITECO\033\"S]\
1Q0#32768U1[1JODONE\033''Z\"EGZ0,0XZ'J::FS^ES\033\033\033\
::FSTECO32\033TECO\033\
ZJ-:S^ES\033\"S.-Z\"EFR\033''Z\"EODONE\033'Q0&32767U10,0X10,\
0X9J:FS^ES/NOI\033\033\"SO/NOINI\033'J:FS/NOI\033\033\"\
S!/NOINI!::FSN\033\033\"S::FSI\033\033\033'::FS^ES\033\
\033\"S0A\"AI \033''Q1#4096U1'Q1&4096\"EHX0HKEPEAEF:EGINI\033\
\"SG*J::FS$\033\033\"SHX1HK:ER^EQ1\033\"SYHX1HKECG1\174\001\
%Can't find initialization file \"\001:G1\001\"\015\012\001\
''0,0X1\1740,0X1-1EJ-5*-1EJ-1024\"EEN[]*.*\033:EN\033\"SG*JS[\033\
0KFS.\033]\033JS]\0330,.X1HK'':ER^EQ1TECO.INI\033\"SYHX9\
HKECG90,0X9''ER\033EW\033Z\"NZJ.U0G1I=\033G0Q0JZX9ZK]1]0Q0\
ET]0Q0ES]0Q0EU]0Q0ED]0Q0\030]0Q0\022]2HX0HKG9JS=\0330,.-1\
X90,.KM0+0U90,0X0[20,0X210U0[0\004\030U0[00\030EDU0[0ED&4E\
DEUU0[0-1EUESU0[00ESETU0[032768#32#8#1,0ET[1G9\031X1\031K0,\
0X9Q9U1J::FS^ES\033\033\033ZJ-:S^ES\033\"S.-Z\"EFR\033'\
'Z\"EOSCOPES\033'\174G0''J:FS^ES/NOM\033\033\"SO/NOMEMOR\
Y\033'J:FS/NOM\033\033\"S!/NOMEMORY!::FSE\033\033\"S::F\
SM\033\033\"S::FSO\033\033\"S::FSR\033\033\"S::FSY\033\
\033\033''''::FS^ES\033\033\"S0A\"AI \033''Q1#4U1'J:FS^\
ES/NOC\033\033\"SO/NOCREATE\033'J:FS/NOC\033\033\"S!/NO\
CREATE!::FSR\033\033\"S::FSE\033\033\"S::FSA\033\033\"\
S::FST\033\033\"S::FSE\033\033\033''''::FS^ES\033\033\
\"S0A\"AI \033''Q1#256U1'-1EJ-4\"NJ::STE\033\"S<0A\"AC>'::S^ES@\033\
\"S0KIMUNG \033'''J::FSMU\033\033\"S<0A\"AD>'::FS^ES\033\033\
\033-1EJ-5*-1EJ-1024\"EJ::FS\"\033\033\"S::FS^ES\033\033\033''ZU\
0J:S/\033\"SR.-Q0\"L.U0''J:S^ES\033\"S.-Q0\"L.U0''J<S,\033;.U\
1Q1-1J-:S[\033\"S:S]\033\"S.-Q1\"GF<'''Q1-1J-:S<\033\"S:S>\033\
\"S.-Q1\"GF<'''Q1-1J-:S(\033\"S:S)\033\"S.-Q1\"GF<'''Q1-1J.-1-Q0\
\"LD.U0'0;>Q0J<-.;-1A-32\"N-1A-9\"N0;''-D>0,.X0::S^ES\033\0330\
,.K:Q0\"E\001\077How can I MUNG nothing\077\015\012\001\
\003\003'EI^EQ0\033JODONE\033'J:FS^ES/GT\033\033\"SO/G\
TEDIT\033'J:FS/GT\033\033\"S!/GTEDIT!::FSE\033\033\"S::\
FSD\033\033\"S::FSI\033\033\"S::FST\033\033\033''':\
:FS^ES\033\033\"S0A\"AI \033''Q1#64U1'J:FS^ES/VT\033\033\
\"SO/VTEDIT\033'J:FS/VT\033\033\"S!/VTEDIT!::FSE\033\033\
\"S::FSD\033\033\"S::FSI\033\033\"S::FST\033\033\033\
'''::FS:H\033\033\"S::FSO\033\033\"S::FSL\033\033\"S::\
FSD\033\033\033''Q1#32U1'::FS:S\033\033\"S::FSE\033\033\
\"S::FSE\033\033\"S::FSA\033\033\"S::FSL\033\033\"S::FS\
L\033\033\033''''Q1#16U1'::FS:H\033\033\"S::FSO\033\033\
\"S::FSL\033\033\"S::FSD\033\033\033''Q1#32U1'::FS^ES\033\
\033\"S0A\"AI \033''Q1#1U1'J:FS^ES/SC\033\033\"SO/SCROLL\033\
'J:FS/SC\033\033\"S!/SCROLL!::FSR\033\033\"S::FSO\033\033\
\"S::FSL\033\033\"S::FSL\033\033\033'''::S:^ED\033\"SR\
-D\U2FR\033]0U2'::FS:S\033\033\"S::FSE\033\033\"S\
::FSE\033\033\"S::FSA\033\033\"S::FSL\033\033\"S::FSL\033\
\033\033''''Q1#16U1'::FS^ES\033\033\"S0A\"AI \033''Q1#1\
28U1'J::FSTE\033\033\"S<0A\"AD>'::FS^ES\033\033\033J:FS\
^ES/IN\033\033\"SO/INSPECT\033'J:FS/IN\033\033\"S!/INSP\
ECT!::FSS\033\033\"S::FSP\033\033\"S::FSE\033\033\"S::\
FSC\033\033\"S::FST\033\033\033''''::FS^ES\033\033\
\"S0A\"AI \033''Q1#2U1'J:FS^ES/FI\033\033\"SO/FIND\033'J:F\
S/FI\033\033\"S!/FIND!::FSN\033\033\"S::FSD\033\033\033\
'::FS^ES\033\033\"S0A\"AI \033''Q1#8U1'J<.-Z;0A-9\"EDF<'0A-3\
2\"EDF<'C-1A-\036\"\"E:S\"\033\"UZJ''>Q1&4#Z\"E:EGMEM\033\"SG*J:?\
:FS$\033\033\"SF\174'\174Z\"E:Q1\"E-1EJ-5*-1EJ-1024\"EEN[]*.*\033\
:EN\033\"SG*JS[\033R0KFS.\033]\033JS]\0330,.X1HK''':Q9?\
\"E0EJ+100\JDG1ITECF\033ZJI.TMP\033-1EJ-4\"EI<60>/MODE:#3000\033\
\174-1EJ-7\"NI;1/-CR\033''HX9HK'G9'HX0HKEPEAEF:ER^EQ0\033\
\"SYL.\"N-1A-10\"E-D''.\"N-1A-13\"E-D''ZKHX0HKECG0'ER\033EW\0330\
,0X0'J<.-Z;0A-9\"EDF<'0A-32\"EDF<'C-1A-\036\"\"E:S\"\033\"UZJ''>J\
:FS/FI\033\033\"S::FSN\033\033\"S::FSD\033\033\033'\
Q1#8192#8U1'Z\"NQ1#32768#16384U1''Q1&16384\"NQ1&2\"E\001Edi\001\174\
\001Inspec\001'\001ting file \"\001HT\001\"\015\012\001\
'Z\"EQ1&2\"EOSCOPES\033'\001\077How can I inspect nothing?\077\
\015\012\001\003\003'Q1&2\"EJ:S=\033\"S0,.-1X00,.KZU0HX2\
HK:Q2\"N:ER^EQ2\033\"UG2HX0HKONOTFND\033'':Q0\"NEW^EQ0\033'Q\
0\"NQ1#2048U1'OREMEM\033''HX0HKQ1&2\"E:EB^EQ0\033\"SQ1#2048U1O?\
REMEM\033'Q1&256\"NQ1&32768\"EONOTFND\033''\001%Can't find f\
ile \"\001:G0\001\"\015\012\001Q1&32768\"E\001%Creating ne\
w file\015\012\001OCREATE\033\174\001%Ignoring TECO'\
s memory\015\012\001Q1#8192U10,0X0OREMEM\033'':ER^EQ0\033\
\"SQ1#2048U1OSCOPES\033'!NOTFND!\001\077Can't find file \"\001\
:G0\001\"\077\015\012\001\003\003'J::FSMA\033\033\"\
S<0A\"AD>'::FS^ES\033\033\033J<.-Z;0A-9\"EDF<'0A-32\"EDF<'C-\
1A-\036\"\"E:S\"\033\"UZJ''>Z\"E?\001\077How can I MAKE nothin\
g\077\015\012\001\033\033'Z-4\"EJ::SLOVE\033\"S\001No\
t war\077\015\012\001''HX0HK!CREATE!EW^EQ0\033OREMEM\033\
'\001%Unrecognized command \"\001HTHK\001\"\015\012\001OS\
COPES\033!REMEM!Q1&8192\"NOWRITE\033'Q1&(32768#4)\"NOSCOPES\033\
.ASCII  ?':Q0"EOSCOPES?<27>?'!WRITE!:EGMEM?<27>?"SZJG*?<19>?C::FS$?<27>
.ASCII  <27>?"UZK:EGMEM ^EQ0?<27><27><124>?ZX9ZKF?<124>?'?<124>?:Q?
.ASCII  ?9"E:Q1"E-1EJ-5*-1EJ-1024"EEN[]*.*?<27>?:EN?<27>?"SZJ.U0G*Q0JS[?<27>?RQ0
.ASCII  ?KFS.?<27>?]?<27>?Q0JS]?<27>?Q0,.X1Q0,ZK'''G1?<25>?X9?<25>?K:?<21>
.ASCII  ?9TECF?<27>?0EJ+100\.-2,.:X9-3D:?<21>?9.TMP?<27>?-1EJ-4"E:?<21>
.ASCII  ?9<60>/MODE:#3000?<27><124>?-1EJ-7"N:?<21>?9;1/-CR?<27>?'''EA:?
.ASCII  ?EW^EQ9?<27>?"SG0?<25>?PW?<25>?K'EFEW?<27>?'!SCOPES!Q1&64"NET&10?
.ASCII  ?24"E?<1>?%You don't have a refresh scope?<13><10><1>?Q1#64-?
.ASCII  ?64U1''Q1&1"NET&512"E?<1>?%Your terminal is not an interactive s?
.ASCII  ?cope?<13><10><1>?Q1#1-1U1?<124>?0,3:W?<27>?Q1&16"N-1,3:W?<27>
.ASCII  ?'0,5:W?<27>?Q1&32"N-1,5:W?<27>?'''Q1&(64#1)"EOYANK?<27>?'Q1&64"?
.ASCII  ?N?<21>?9GTE?<27><124><21>?9VTE?<27>?':EG^EQ9?<27>?"S?<21>?1?
.ASCII  <27>?G*?<25>?X9?<25>?K?<124>?:?<21>?9DIT?<27>?:Q1"E-1EJ-5*-1EJ-1024"EEN[
.ASCII  ?]*.*?<27>?:EN?<27>?"SZJ.U0G*Q0JS[?<27>?RQ0,.KFS.?<27>?]?<27>?Q0?
.ASCII  ?JS]?<27>?Q0,.X1Q0,ZK'''':Q9"N:EI^EQ1^EQ9?<27>?"SQ1#2048-2048U1O?
.ASCII  ?YANK?<27>?'':EGLIB?<27>?"SG*?<25>?X1?<25>?K?<124><21>?1?<27>
.ASCII  ?':Q1"E?<21>?1LB:[1,2]?<27>?-1EJ-7"E?<21>?1SY:?<27>?'-1EJ-5*-1EJ-1024"E?
.ASCII  ?1SYS$LIBRARY:?<27>?'-1EJ-4"E?<21>?1$?<27>?''Q1&64"N?<21>?9GTEDI?
.ASCII  ?T?<27><124><21>?9VTEDIT?<27>?':Q1"N:EI^EQ1^EQ9?<27>?"SQ1#20?
.ASCII  ?48-2048U1OYANK?<27>?''?<1>?%The ?<1>?:G9?<1>? macro is not avai?
.ASCII  ?lable?<13><10><1>?!YANK!Q1&2048"NY'JQ1&8"N?<1>?Finding posi?
.ASCII  ?tion marker...?<13><10><1>?EW?<27>?G*?<25>?X0?<25>?K-1?<24>
.ASCII  ?:Q0"N:FN?<126><126>?/\?<126><126><13><10><27><27>?U?
.ASCII  ?0?<124>?:F_?<126><126>?/\?<126><126><13><10><27><27>
.ASCII  ?U0'0?<24>?Q0"U?<1><63>?Position marker not found?<63><13><10>
.ASCII  <1><3><3>?''Q1&128"NET&512"E?<1>?%Your terminal is not an ?
.ASCII  ?interactive scope?<13><10><1><124>?Q2"E2:W/4U2'0,3:W?<27>
.ASCII  ?Q1&16"N-1,3:W?<27>?'0,5:W?<27>?Q1&32"N-1,5:W?<27>?'Q2,7:W-Q2"N?<1>
.ASCII  ?%Your terminal doesn't support scrolling?<13><10><1><124>
.ASCII  ?-1EV'''!DONE!]1]0Q0ET]0Q0ES]0Q0EU]0Q0ED]0Q0?<24>?]0Q0?<18>?]2]0?
.ASCII  ?]90,0XY]9]8]7]6]5]4]3]2]1]0?
";

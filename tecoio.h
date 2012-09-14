/*
**++
**  MODULE DESCRIPTION:
**
**      TECO I/O macros and externals.
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
**  CREATION DATE:      08-NOV-2011
**
**  MODIFICATION HISTORY:
**
**      08-NOV-2004 V41.00  Sneddon   Initial coding.
**--
*/
#ifndef __TECOIO_LOADED
#define __TECOIO_LOADED 1

#define TYPET 0x0  /* Type without case flagging and ringing bells */
#define TYPEF 0x1  /* Type with case flagging */
#define TYPEB 0x2  /* Type with ringing bells */

extern void teco_type();
extern uint8_t listen(uint32_t);
extern void teco_delch(void);

#define type(c) teco_type(TYPET, (c))
#define typb(c) teco_type(TYPEB, (c))
#define typf(c) teco_type(TYPEF, (c))

#define print(ptr, len) \
do { \
    uint32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        type(p[i]);  \
} while (0)
#define prinb(ptr, len) \
do { \
    uint32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        typb(p[i]); \
} while (0)
#define prinf(ptr, len) \
do { \
    uint32_t i; \
    uint8_t *p = (uint8_t *)(ptr); \
    for (i = 0; i < len; i++) \
        typf(p[i]); \
} while (0)

/*
This is a template for the OS-specific I/O code...

	terminal output
	terminal input
	ejflg info
	gexit - process special functions
	getfl - get files opened
	inpsv - input channel switch
	outsv - output channel switch
	bakup - backwards page
	getbf - get input
	putbf - put output
	clsfl - close input & output files
	clsof - close output file
	kilfl - delete a file

What are the I/O call backs used by files?  Maybe we need our own file
processing block...


*/
#endif /* __TECOIO_LOADED */

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
**      31-JAN-2005 V41.00  Sneddon	Initial coding.
**--
*/
#define MODULE TECOMSG
#define VERSION "V41.00"
#ifdef vms
# ifdef VAX11C
#  module MODULE VERSION
# else
#  pragma module MODULE VERSION
# endif
#endif
#include <stdlib.h>
#include "tecodef.h"
#include "tecomsg.h"
#include "globals.h"

/*
** Forward Declarations.
*/

    void teco_putmsg(void);

/*
** Macro Definitions
*/

#define crlfno() print("\r\n", 2)
#define          msgdef(arg, msg, help) \
    { (uint8_t *)(arg), (uint8_t *)(msg), (uint8_t *)(help), \
      sizeof(arg), sizeof(msg), sizeof(help) }

/*
** Own Storage.
*/

    struct {
	uint8_t *id;
	uint8_t *msg;
	uint8_t *help;
	uint32_t idlen;
	uint32_t msglen;
	uint32_t helplen;
    } messages[] = {
         msgdef("NOR", "normal successful completion",    
                ""),
         msgdef("BNI", "> not in iteration",
                "There is a close angle bracket not  matched  by  an\n"
                "open  angle  bracket somewhere to its left.  (Note:\n"
                "an iteration in a macro stored in a Q-register must\n"
                "be complete within the Q-register.)"),
         msgdef("CPQ", "Can't pop Q-reg",
                "A ] command has been executed and there is  nothing\n"
                "saved on the Q-register push down list."),
         msgdef("DTB", "Delete too big",
                "An nD command  has  been  attempted  which  is  not\n"
                "contained within the current page."),
         msgdef("ERR", "%s, %s",
                "Some system service call failed.  The error\n"
                "message ID and text explain the error."),
         msgdef("FNF", "File not found \"%-*.*s\"",
                "The requested input file could not be located.   If\n"
                "this  occurred within a macro the colon modified ER\n"
                "or EB command may be necessary."),
         msgdef("IAA", "Illegal A arg",
                "The argument preceding a :A command is negative  or\n"
                "0."),
         msgdef("IEC", "Illegal E characters",
                "An invalid E command  has  been  executed.   The  E\n"
                "character must be followed by an alphabetic to form\n"
                "a legal E command (e.g., ER or EX)."),
         msgdef("IFC", "Illegal F characters",
                "An invalid F command has been executed."),
         msgdef("IIA", "Illegal insert arg",
                "A command of  the  form  \"nItext$\"  was  attempted.\n"
                "This combination of character and text insertion is\n"
                "illegal."),
         msgdef("ILL", "Illegal command",
                "An attempt has been made to execute an invalid TECO\n"
                "command."),
         msgdef("ILN", "Illegal number",
                "An 8 or 9 has been entered when the radix  of  TECO\n"
                "is set to octal."),
         msgdef("INP", "Input error",
                "The system has reported an error attempting to read\n"
                "the  current  input file.  The  text buffer  may be\n"
                "corrupt.  This operation may be retried, but if the\n"
                "error persists,  you may have to return to a backup\n"
                "file."),
         msgdef("IPA", "Illegal P arg",
                "The  argument  preceding  a  P  or  PW  command  is\n"
                "negative or 0."),
         msgdef("IQC", "Illegal \" character",
                "One of the valid \" commands did not  follow  the \"."),
         msgdef("IQN", "Illegal Q-reg name",
                "An illegal Q-register name was specified in one  of\n"
                "the Q-register commands."),
         msgdef("IRA", "Illegal radix arg",
                "The argument to a ^R radix command must be  8,  10,\n"
                "or 16."),
         msgdef("ISA", "Illegal search arg",
                "The argument preceding a search command is 0.  This\n"
                "argument must not be 0."),
         msgdef("ISS", "Illegal search string",
                "One of the search string  special  characters  (^Q,\n"
                "^R,  etc.)  would  have  modified the search string\n"
                "delimiter (usually ESCAPE)."),
         msgdef("IUC", "Illegal ^ character",
                "The character following an ^ must have ASCII  value\n"
                "between  100  and  137 inclusive or between 141 and\n"
                "172 inclusive."),
         msgdef("MEM", "Memory overflow",
                "Insufficient  memory  available  to  complete   the\n"
                "current  command.   Make  sure  the Q-register area\n"
                "does not contain much unnecessary  text.   Breaking\n"
                "up  the  text  area  into  multiple  pages might be\n"
                "useful."),
         msgdef("MRP", "Missing )",
                "There is a right parenthesis that is not matched by\n"
                "a corresponding left parenthesis."),
         msgdef("NAB", "No arg before ^_",
                "The  ^_  command  must  be  preceded  by  either  a\n"
                "specific numeric argument or a command that returns\n"
                "a numeric value."),
         msgdef("NAC", "No arg before .",
                "A command has been executed in which  a  ,  is  not\n"
                "preceded by a numeric argument."),
         msgdef("NAE", "No arg before =",
                "The =, ==, or  ===  command  must  be  preceded  by\n"
                "either  a  specific  numeric  argument or a command\n"
                "that returns a numeric value."),
         msgdef("NAP", "No arg before )",
                "A ) parenthesis has been  encountered  and  is  not\n"
                "properly preceded by a specific numeric argument or\n"
                "a command that returns a numeric value."),
         msgdef("NAQ", "No arg before \"",
                "The \" commands must be preceded by a single numeric\n"
                "following commands or skip to  the  matching  '  is\n"
                "argument  on  which  the  decision  to  execute the\n"
                "based."),
         msgdef("NAS", "No arg before ;",
                "The ;  command must be preceded by a single numeric\n"
                "argument  on  which  the  decision  to  execute the\n"
                "following commands or skip to  the  matching  >  is\n"
                "based."),
         msgdef("NAU", "No arg before U",
                "The U command must be preceded by either a specific\n"
                "numeric  argument  or  a  command  that  returns  a\n"
                "numeric value."),
         msgdef("NFI", "No file for input",
                "Before issuing an input command, such as Y,  it  is\n"
                "necessary to open an input file by use of a command\n"
                "such as ER or EB."),
         msgdef("NFO", "No file for output",
                "Before issuing an output command such as  N  search\n"
                "or  P it is necessary to open an output file by use\n"
                "of a command such as EW or EB."),
         msgdef("NYA", "Numeric arg with Y",
                "The Y command must not  be  preceded  by  either  a\n"
                "numeric  argument  or  a  command  that  returns  a\n"
                "numeric value."),
         msgdef("OFO", "Output file already open",
                "A command has been executed which tried  to  create\n"
                "an  output  file,  but  an output file currently is\n"
                "open.  It is typically appropriate to use the EC or\n"
                "EK  command as the situation calls for to close the\n"
                "output file."),
         msgdef("OUT", "Output error",
                "A command has been executed in which  a  ,  is  not\n"
                "The system has reported an  error  attempting to do\n"
                "output to the outut file. Make sure that the output\n"
                "device did not become  write locked.  Use of the EF\n"
                "command (or EK if necessary) and another  EW can be\n"
                "considered until the condition is fixed."),
         msgdef("PDO", "Push-down list overflow",
                "The  command  string  has   become   too   complex.\n"
                "Simplify it."),
         msgdef("POP", "Pointer off page",
                "A J,  C  or  R  command  has  been  executed  which\n"
                "attempted  to  move  the pointer off the page.  The\n"
                "result of executing  one  of  these  commands  must\n"
                "leave  the pointer between 0 and Z, inclusive.  The\n"
                "characters referenced by a D or m,nX  command  must\n"
                "also be within the buffer boundary."),
         msgdef("SNI", "; not in iteration",
                "A ;  command has been executed outside of  an  open\n"
                "iteration   bracket.   This  command  may  only  be\n"
                "executed within iteration brackets."),
         msgdef("SRH", "Search failure \"%s\"",
                "A search command not preceded by a  colon  modifier\n"
                "and  not within an iteration has failed to find the\n"
                "specified \"text\".  After  an  S  search  fails  the\n"
                "pointer  is  left  at  the beginning of the buffer.\n"
                "After an N or _ search fails the last page  of  the\n"
                "input  file  has  been input and, in the case of N,\n"
                "output, and the buffer is cleared.  In the case  of\n"
                "an  N  search  it is usually necessary to close the\n"
                "output file and reopen it for continued editing."),
         msgdef("STL", "String too long",
                "A search or file name string is too long.  This  is\n"
                "most  likely  the  result of a missing ESCAPE after\n"
                "the string."),
         msgdef("UTC", "Unterminated command",
                "This is a general error which is usually caused  by\n"
                "an   unterminated   insert,   search,  or  filespec\n"
                "argument,   an   unterminated   ^A   message,    an\n"
                "unterminated  tag  or comment (i.e., unterminated !\n"
                "construct), or a missing ' character which closes a\n"
                "conditional execution command."),
         msgdef("UTM", "Unterminated macro",
                "This error is the same as  the  ?UTC  error  except\n"
                "that  the unterminated command was executing from a\n"
                "Q-register (i.e., it  was  a  macro).   (Note:   An\n"
                "entire command sequence stored in a Q-register must\n"
                "be complete within the Q-register.)"),
         msgdef("XAB", "Execution aborted",
                "Execution of TECO was aborted.  This is usually due\n"
                "to the typing of CTRL/C."),
         msgdef("YCA", "Y command aborted",
                "An attempt has been made  to  execute  an  Y  or  _\n"
                "search command with an output file open, that would\n"
                "cause text in the text buffer to be erased  without\n"
                "outputting  it  to the output file.  The ED command\n"
                "controls this check."),
         msgdef("NYI", "Not yet implimented",
                "\t\t\tA  command  was  issued  which  tries  to invoke  a\n"
                "\t\t\tfeature  not available  on  this  implimentation of\n"
                "\t\t\tTECO")
    };

void teco_putmsg(void)
{
    uint16_t ehelp = ctx.ehelp;
    uint32_t errcod = abs(ctx.errcod);
    uint8_t *fnfptr;
    uint32_t msglen = messages[errcod].msglen;
    uint8_t *msgptr = messages[errcod].msg;

    /* 0EH always defaults to 2EH.
    */
    if (!ehelp)
	ehelp = 2;

    /* The error identifier is always printed.
    */
    type('?');
    print(messages[errcod].id, messages[errcod].idlen);


    if (ehelp & 2) {
	/* Print the error description.
	*/
	type(TECO_C_TAB);

	if (errcod == TECO__ERR) {
	    /* This is a system specific error code.  Need to call the
	    ** OS interface to receive the message.
	    */
	    // for the moment this does nothing.  However, will change shortly...
	} else if (errcod == TECO__FNF) {
	    /* This is a file not found, so we need to include the file
	    ** name in the message output.
	    */
	    msglen += ctx.filbuf.qrg_size;
	    fnfptr = alloca(msglen);
	    snprintf(fnfptr, msglen, msgptr, ctx.filbuf.qrg_size,
		     ctx.filbuf.qrg_size, ctx.filbuf.qrg_ptr);
	    msgptr = fnfptr;
	}

	print(msgptr, msglen);
    }

    if (ehelp & 1) {
	crlfno();
	crlfno();

	print("     ?", 6);
	print(messages[errcod].id, messages[errcod].idlen);
	print("    ", 4);
	print(messages[errcod].msg, messages[errcod].msglen);
    }

    if ((ehelp & 4) && (ctx.errpos)) {
	crlfno();

	print(ctx.errptr, ctx.errpos);
	type('?');
    }
}

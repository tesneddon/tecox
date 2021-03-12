/*
**++
**  MODULE DESCRIPTION:
**
**      This program is the genmsg utility. It takes a message file of the
**  format:
**
**      .ident <string>
**      .facility <ident>,<number>[/SYSTEM]
**
**      .severity <severity-code>
**
**      .name <ident>
**      .text <message-text>
**      .help <help-text>
**
**  From this source file, genmsg can then create the following output formats:
**
**      o. OpenVMS message file (can be compiled to a MESSAGE object and SDL
**         file that can be further processed to a header file);
**      o. C header file.  On OpenVMS systems this defines '#define' statements
**         that refer to the message symbol ('$' is replaced with '_', this allows
**         common source on VMS and UNIX systems). On UNIX systems this will generate
**         a message number;
**      o. OpenVMS HELP file for inclusion in a larger application help file.
**      o. UNIX man page.
**      o. DocBook help file.
**
**  AUTHOR:             Tim E. Sneddon
**
**  Copyright (C) 2020 Tim E. Sneddon <tim@sneddon.id.au>
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
**  CREATION DATE:      14-APR-2020
**
**  MODIFICATION HISTORY:
**
**      14-APR-2020 V41.00  Sneddon     Initial coding.
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
#endif
#ifdef linux
# define _GNU_SOURCE
#else
static char *program_invocation_short_name = 0;
#endif
#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_DIRECTIVE 10
#define MAX_IDENT 32
#define MAX_STRING 4096

/* Message severity code */
#define SEVERITY_ERROR   0x01
#define SEVERITY_FATAL   0x02
#define SEVERITY_INFO    0x04
#define SEVERITY_SUCCESS 0x08
#define SEVERITY_WARN    0x10

typedef struct _entrydef {
    struct _entrydef *next;
    int id;
    int severity;
    char *name;
    char *text;
    char *help;
} ENTRYDEF;

static char ident[MAX_IDENT+1] = "";
static char facility[MAX_IDENT+1];
static int facility_number = 0;
static char module[MAX_IDENT+1];

static char *filename = 0;
static char *outfilename = 0;

static ENTRYDEF current, **list, *tree;
static int id = 0, count = 0;
static char directive[MAX_DIRECTIVE+1];
static int severity = SEVERITY_INFO;

static int colno = 1;
static int lineno = 1;
static int number;
static char string[MAX_STRING+1];

static void usage(int brief);

static int next(FILE *fp) {
    int c = getc(fp);

    if (c == '\n') {
        lineno++;
        colno = 1;
    } else if (c != EOF) {
        colno++;
    }

    return c;
} /* next */

static void skipline(FILE *fp) {
    int c;

    while ((c = next(fp)) != EOF) {
        if (c == '\n')
            break;
    }
} /* skipline */

static void skipspace(FILE *fp) {
    int c;

    while (isblank(c = next(fp)))
        ;

    if (c != EOF)
        ungetc(c, fp);
} /* skipspace */

static void getdirect(FILE *fp) {
    int c, i;

    directive[0] = '\0';

    for (i = 0; i < MAX_DIRECTIVE; i++) {
        c = next(fp);

        if (isalpha(c)) {
            directive[i] = c;
        } else {
            ungetc(c, fp);
            break;
        }
    }

    directive[i] = '\0';
} /* getdirect */

static void getnum(FILE* fp) {
    int c;

    number = 0;
    while ((c = next(fp) != EOF)) {
        if (!isdigit(c)) {
            ungetc(c, fp);
            break;
        }
        number = (number * 10) + (c - '0');
    }
} /* getnum */

static void getident(FILE* fp) {
    int c, i;

    string[0] = '\0';

    for (i = 0; i < MAX_STRING; i++) {
        c = next(fp);

        if (isalnum(c) || (c == '_') || (c == '$')) {
            string[i] = c;
        } else {
            ungetc(c, fp);
            break;
        }
    }

    string[i] = '\0';
} /* getident */

static void getstring(FILE *fp) {
    int c, i;
    int quote;

    c = next(fp);
    if ((c != '\\') && ispunct(c)) {
        /* Got a quote, can't use \ though as that is the line continuation */

        quote = c;
        c = next(fp);
    } else {
        /* Quote is the end of the line, can use \ before a literal '\n' to continue */

        quote = '\n';
    }

    string[0] = '\0';

    for (i = 0; i < MAX_STRING; i++) {
        int slash = 0;

        if (c == '\\') {
            slash = 1;
            c = next(fp);

            switch (tolower(c)) {
            default:
                fprintf(stderr, "%s: %s:%d:%d, illegal quoted chararacter %c\n",
                        program_invocation_short_name, filename,
                        lineno, colno, c);
                skipline(fp);
                break;
            case 'n':  c = '\n'; break;
            case 't':  c = '\t'; break;
            case '\n': c = next(fp); break;
            case '\\':
                break;
            }
        }

        if (c == EOF)
            break;

        if (!slash && (c == quote))
            break;

        string[i] = c;

        c = next(fp);
    }

    string[i] = '\0';
} /* getstring */

static void store() {
    ENTRYDEF *new;

    if (current.name == 0)
        return;

    new = calloc(1, sizeof(ENTRYDEF));
    *new = current;

    new->next = tree;
    tree = new;

    memset(&current, 0, sizeof(current));
} /*store */

void emit_message(FILE *op,
                  int selection) {
    ENTRYDEF *ep = 0;
    int i;
    int prev_severity = 0;

    fprintf(op, "\t.IDENT\t\t\"%s\"\n", ident);
    fprintf(op, "\t.FACILITY\t%s,%d%s\n", facility, facility_number,
            system ? "/SYSTEM" : "");
    for (i = 0; i < count; i++) {
        ENTRYDEF *ep = list[i];

        if (ep->severity != prev_severity) {
            fprintf(op, "\t.SEVERITY\t%s\n",
                    ep->severity == SEVERITY_INFO ? "INFORMATIONAL" :
                    ep->severity == SEVERITY_ERROR ? "ERROR" :
                    ep->severity == SEVERITY_FATAL ? "FATAL" :
                    ep->severity == SEVERITY_SUCCESS ? "SUCCESS" :
                    ep->severity == SEVERITY_WARN ? "WARNING" : "");

            prev_severity = ep->severity;
        }

        fprintf(op, "\t%s%*s<%s>\n",
                ep->name, 16-strlen(ep->name), "", ep->text);
    }
} /* emit_message */

void emit_help(FILE *op,
               int selection) {
    int i;

    for (i = 0; i < count; i++) {
        ENTRYDEF *ep = list[i];

        if (ep->severity & selection) {
            fprintf(op, "2 %s\n", ep->name);
            fprintf(op, "     ?%s    %s\n", ep->name, ep->text);
            if (ep->help)
                fprintf(op, "\n%s\n", ep->help);
            fprintf(op, "\n");
        }
    }
} /* emit_help */

void emit_header(FILE *op,
                 int selection) {
    int min_id, max_id;
    ENTRYDEF *ep = 0;

    fprintf(op, "/* %s %s */\n", module, ident);
    fprintf(op, "#ifndef %s_DEFINED\n", module);
    fprintf(op, "#define %s_DEFINED 1\n", module);

    fprintf(op, "#ifdef %s_MESSAGES_INTERNAL\n", facility);
    fprintf(op, "    typedef struct _msgdef {\n");
    fprintf(op, "        int msg_index;\n");
    fprintf(op, "        char *msg_name;\n");
    fprintf(op, "        char *msg_text;\n");
    fprintf(op, "        char *msg_help;\n");
    fprintf(op, "    } MSGDEF;\n");
    fprintf(op, "    extern MSGDEF %s_MESSAGES[];\n", facility);
    fprintf(op, "#endif /* %s_MESSAGES_INTERNAL */\n", facility);

    for (ep = tree, min_id = ep->id; ep != 0; ep = ep->next) {
#ifdef VMS
        fprintf(op, "#define %s__%s %s%s_%s\n",
               facility, ep->name, facility,
               system ? "$" : "_", ep->name);
#else
        fprintf(op, "#define %s__%s %d\n", facility, ep->name, ep->id);
#endif
        max_id = ep->id;
    }

    fprintf(op, "#ifdef %s_MESSAGES_INTERNAL\n", facility);
    fprintf(op, "#define %s_MESSAGES_MIN %d\n", facility, min_id);
    fprintf(op, "#define %s_MESSAGES_MAX %d\n", facility, max_id);
    fprintf(op, "#endif /* %s_MESSAGES_INTERNAL */\n", facility);

    fprintf(op, "#endif /* %s_DEFINED */\n", module);
} /* emit_header */

void emit_msgdef(FILE *op,
                 int selection) {
    int i;

    fprintf(op, "#define %s_MESSAGES_INTERNAL\n", facility);
    fprintf(op, "#include \"tecomsg.h\"\n");
    fprintf(op, "/* Facility: %s */\n", facility);
    fprintf(op, "MSGDEF %s_MESSAGES[] = {\n", facility);

    for (i = 0; i < count; i++) {
        char *p;
        ENTRYDEF *ep = list[i];

        fprintf(op, "    { %d, \"%s\", \"", ep->id, ep->name);

        for (p = ep->text; p && *p; p++) {
            if (*p == '"')
                fputc('\\', op);
            fputc(*p, op);
        }

        fprintf(op, "\", \"");

        if (p) {
            fprintf(op, "\\\n");

            for (p = ep->help; p && *p; p++) {
                if (*p == '\n') {
                    fprintf(op, "\\\n");
                } else {
                    if (*p == '"')
                        fputc('\\', op);
                    fputc(*p, op);
                }
            }
        }

        fprintf(op, "\", },\n");
    }

    fprintf(op, "};\n");
} /* emit_msgdef */

int main(argc,
         argv)
    int argc;
    char **argv;
{
    ENTRYDEF *ep;
    int opt, vms, header, help, msgdef, selection;
    int c;
    FILE *fp = 0, *op = 0;
    char *bn = 0, *ext = 0;
    int bnlen = 0;
    int i; 
    int print_usage = 0;

    if (program_invocation_short_name == 0) {
        program_invocation_short_name = basename(argv[0]);
    }

    opterr = 0;
    header = msgdef = vms = help = 0;
    selection = 0;
    print_usage = 1;
    while ((opt = getopt(argc, argv, "chmvxo:s:")) != -1) {
        switch (opt) {
        case 'o':
            outfilename = optarg;
            break;

        case 'h':
            print_usage = 0;
            break;

        case 's':
            for (i = 0; (c = optarg[i]) != 0; i++) {
                switch (c) {
                default:
                    fprintf(stderr, "%s: unknown severity '%c'\n",
                            program_invocation_short_name, c);
                    print_usage = 0;
                    break;

                case ' ':
                    break;

                case 'i':
                case 'I':
                    selection |= SEVERITY_INFO;
                    break;

                case 'w':
                case 'W':
                    selection |= SEVERITY_WARN;
                    break;

                case 'e':
                case 'E':
                    selection |= SEVERITY_ERROR;
                    break;

                case 'f':
                case 'F':
                    selection |= SEVERITY_FATAL;
                    break;

                case 's':
                case 'S':
                    selection |= SEVERITY_WARN;
                    break;
                } 
            }
 
            break;

        case 'c':
        case 'm':
        case 'v':
        case 'x':
            switch (opt) {
            case 'c':
                header = 1;
                break;

            case 'm':
                msgdef = 1;
                break;

            case 'v':
                vms = 1;
                break;

            case 'x':
                help = 1;
                break;
            }

            print_usage = -1;
            break; 

        case ':':
            fprintf(stderr, "%si: option '-%c' expects and argument\n",
                    program_invocation_short_name, optopt);
            break;

        case '?':
            fprintf(stderr, "%s: unknown option '-%c'\n",
                    program_invocation_short_name, optopt);
            break;
        }
    }

    if (print_usage >= 0) {
        usage(print_usage);
        exit(EXIT_FAILURE);
    }

    if (selection == 0)
        selection = -1;

    if (optind < argc) {
        filename = argv[optind];
        fp = fopen(filename, "r");
        if (!fp) {
            perror(program_invocation_short_name);
            return 1;
        }
    } else {
        filename = "<stdin>";
        fp = stdin;
    }

    id = 0;
    memset(&current, 0, sizeof(current));

    while ((c = next(fp)) != EOF) {
        int curline = lineno, curpos = colno;

        switch (c) {
        case ' ':
        case '\t':
        case '\n':
            break;

        case '!':
            skipline(fp);
            break;

        case '.':
            getdirect(fp);
            if (strcasecmp("name", directive) == 0) {
                store();

                skipspace(fp);
                getident(fp);
                skipline(fp);

                if (strlen(string) != 0) {
                    current.id = ++id;
                    current.name = strdup(string);
                    current.severity = severity;
                } else {
                    fprintf(stderr, "%s: %s:%d:%d, .NAME directive missing identifier\n",
                            program_invocation_short_name, filename,
                            curline, curpos);
                }
            } else if (strcasecmp("text", directive) == 0) {
                skipspace(fp);
                getstring(fp);
                skipline(fp);

                if (strlen(string) != 0) {
                    current.text = strdup(string);
                }
            } else if (strcasecmp("help", directive) == 0) {
                skipspace(fp);
                getstring(fp);
                skipline(fp);

                if (strlen(string) != 0) {
                    current.help = strdup(string);
                }
            } else if (strcasecmp("severity", directive) == 0) {
                unsigned char c;

                skipspace(fp);
                getident(fp);
                skipline(fp);

                c = toupper(string[0]);
                switch (c) {
                default:
                    fprintf(stderr, "%s: %s:%d:%d, unknown severity keyword %s\n",
                            program_invocation_short_name, filename,
                            curline, curpos, string);
                    break;

                case 'E': severity = SEVERITY_ERROR; break;
                case 'F': severity = SEVERITY_FATAL; break;
                case 'I': severity = SEVERITY_INFO; break;
                case 'S': severity = SEVERITY_SUCCESS; break;
                case 'W': severity = SEVERITY_WARN; break;
                }
            } else if (strcasecmp("facility", directive) == 0) {
                int faclen;

                skipspace(fp);
                getident(fp);
                skipline(fp);

                faclen = strlen(string);
                if (faclen != 0) {
                    if (faclen >= MAX_IDENT) {
                        faclen = MAX_IDENT;

                        fprintf(stderr, "%s: %s:%d:%d, message facility is too long\n",
                                program_invocation_short_name, filename,
                                curline, curpos);
                    }

                    memcpy(facility, string, faclen);
                    facility[faclen] = '\0';
                }

                skipspace(fp);
                c = next(fp);
                if (c == ',') {
                    skipspace(fp);

                    getnum(fp);
                    facility_number = number;

                    // max facility number?

                    skipspace(fp);
                    c = next(fp);
                    if (c == '/') {
                        getident(fp);

                        // look for SYSTEM
                    } else {
                        ungetc(c, fp);
                    }
                } else {
                    ungetc(c, fp);
                }

                // mostly for vms
            } else if (strcasecmp("ident", directive) == 0) {
                int idlen;

                skipspace(fp);
                getstring(fp);
                skipline(fp);

                idlen = strlen(string);
                if (idlen != 0) {
                    if (idlen >= MAX_IDENT) {
                        idlen = MAX_IDENT;

                        fprintf(stderr, "%s: %s:%d:%d, module ident is too long\n",
                                program_invocation_short_name, filename,
                                curline, curpos);
                    }

                    memcpy(ident, string, idlen);
                    ident[idlen] = '\0';
                }
            } else {
                fprintf(stderr, "%s: %s:%d:%d, unknown directive %s\n",
                        program_invocation_short_name, filename,
                        curline, curpos, directive);
                skipline(fp);
            }

            break;
        }
    }

    store(); /* Store the final message */

    fclose(fp);

    /* Arrange in an ordered list */
    count = id;
    list = calloc(count, sizeof(ENTRYDEF *));
    for (ep = tree; ep != 0; ep =ep->next) {
        list[ep->id-1] = ep;
    }

    /* Now, generate the message output */

    bn = basename(filename);
    if (bn == 0) {
        fprintf(stderr, "%s: unable to determine module name\n");
        return 1;
    }

    ext = strrchr(bn, '.');
    if (ext != 0) {
        *ext = '\0';
    }

    bnlen = strlen(bn);
    if (bnlen > MAX_IDENT)
        bnlen = MAX_IDENT;

    memcpy(module, bn, bnlen);
    module[bnlen] = '\0';

    for (i = 0; i < strlen(module); i++) {
        module[i] = toupper(module[i]);
    }

    if (strlen(facility) == 0) {
        strcpy(facility, module);
    }

    if (outfilename) {
        op = fopen(outfilename, "w");
        if (!op) {
            perror(program_invocation_short_name);
            return 1;
        }
    } else {
        outfilename = "<stdout>";
        op = stdout;
    }

    if (header) {
        emit_header(op, selection);
    } else if (vms) {
        emit_message(op, selection);
    } else if (msgdef) {
        emit_msgdef(op, selection);
    } else if (help) {
        emit_help(op, selection);
    }

    fclose(op);

    return 0;
} /* main */

static void usage(int brief) {
    printf("Usage: genmsg [-c | -h | -m | -v] [ -s e|f|i|s|w ] [-o outfile] [infile]\n");

    if (!brief) {
        printf("\n");
        printf("  infile     GENMSG message source, default is stdin\n");
        printf("\n");
        printf("  -c         C Header format (message number constants)\n");
        printf("  -h         This help message\n");
        printf("  -m         UNIX gencat format\n");
        printf("  -o outfile Output file name, default is stdout\n");
        printf("  -s efisw   Indicate the message severity to output, default is all\n");
        printf("             e=error, f=fatal, i=info, s=success, w=warning\n");
        printf("  -v         OpenVMS MESSAGE Utility format\n");
        printf("  -x         OpenVMS HELP Utility format\n");
    }
}

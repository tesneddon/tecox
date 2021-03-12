/* TECOMSG TECO V41.02 */
#ifndef TECOMSG_DEFINED
#define TECOMSG_DEFINED 1
#ifdef TECO_MESSAGES_INTERNAL
    typedef struct _msgdef {
        int msg_index;
        char *msg_name;
        char *msg_text;
        char *msg_help;
    } MSGDEF;
    extern MSGDEF TECO_MESSAGES[];
#endif /* TECO_MESSAGES_INTERNAL */
#define TECO__MEM_PAG 47
#define TECO__DISK_QUOTA 46
#define TECO__SEAR_ITER 45
#define TECO__NYI 44
#define TECO__YCA 43
#define TECO__XAB 42
#define TECO__UTM 41
#define TECO__UTC 40
#define TECO__STL 39
#define TECO__SRH 38
#define TECO__SNI 37
#define TECO__POP 36
#define TECO__PDO 35
#define TECO__OUT 34
#define TECO__OFO 33
#define TECO__NYA 32
#define TECO__NFO 31
#define TECO__NFI 30
#define TECO__NAU 29
#define TECO__NAS 28
#define TECO__NAQ 27
#define TECO__NAP 26
#define TECO__NAE 25
#define TECO__NAC 24
#define TECO__NAB 23
#define TECO__MRP 22
#define TECO__MEM 21
#define TECO__IUC 20
#define TECO__ISS 19
#define TECO__ISA 18
#define TECO__IRA 17
#define TECO__IQN 16
#define TECO__IQC 15
#define TECO__IPA 14
#define TECO__INP 13
#define TECO__ILN 12
#define TECO__ILL 11
#define TECO__IIA 10
#define TECO__IFC 9
#define TECO__IEC 8
#define TECO__IAA 7
#define TECO__FNF 6
#define TECO__ERR 5
#define TECO__DTB 4
#define TECO__CPQ 3
#define TECO__BNI 2
#define TECO__NORMAL 1
#ifdef TECO_MESSAGES_INTERNAL
#define TECO_MESSAGES_MIN 47
#define TECO_MESSAGES_MAX 1
#endif /* TECO_MESSAGES_INTERNAL */
#endif /* TECOMSG_DEFINED */

#ifndef TRANS2_H_
#define TRANS2_H_
#include "oplist.h"
#include <string.h>
int dirnum = 13;
char directives_lcc[][13] = {
    "import",
    "export",
    "proc",
    "endproc",
    "code",
    "data",
    "lit",
    "bss",
    "align",
    "skip",
    "byte",
    "address"
};


enum directive_lcc {
    IMPORT,
    EXPORT,
    PROC,
    ENDPROC,
    CODE,
    DATA,
    LIT,
    BSS,
    ALIGN,
    SKIP,
    BYTE,
    ADDRESS
};

int opnum = 34;
char opnames_lcc[][10] = {
    "ADDRF",
    "ADDRG",
    "ADDRL",
    "CNST",
    "INDIR",
    "NEG",
    "ADD",
    "BAND",
    "BOR",
    "BXOR",
    "DIV",
    "LSH",
    "MOD",
    "MUL",
    "RSH",
    "SUB",
    "ASGN",
    "EQ",
    "GE",
    "GT",
    "LE",
    "LT",
    "NE",
    "ARG",
    "CALL",
    "RET",
    "JUMP",
    "LABEL",
    "BCOM",
    "CVF",
    "CVI",
    "CVP",
    "CVU",
    "DISCARD"
};



enum op_type {
    TYPE_NONE,
    TYPE_I, //signed
    TYPE_U, //unsigned
    TYPE_P, //unsigned
    TYPE_F, //unsigned bits
    TYPE_V, //wtf
    TYPE_B  //wtf
};

int typenum = 7;
char typenames[][10] = {
    "N",
    "I",
    "U",
    "P",
    "F",
    "V",
    "B"
};

enum opname_lcc {
    ADDRF,
    ADDRG,
    ADDRL,
    CNST,
    INDIR,
    NEG,
    ADD,
    BAND,
    BOR,
    BXOR,
    DIV,
    LSH,
    MOD,
    MUL,
    RSH,
    SUB,
    ASGN,
    EQ,
    GE,
    GT,
    LE,
    LT,
    NE,
    ARG,
    CALL,
    RET,
    RET2,
    JUMP,
    LABEL,
    BCOM,
    CVF,
    CVI,
    CVP,
    CVU,
    DISCARD,
    FASTCALL,
    ALLOC,
    DISCARD1,
    ALLOC1,
    STORE,
    RSTORE,
    ADDRS,
    DUP

};


enum opname_rc {
    rc_nop,
    rc_addrf,
    rc_addrl,
    rc_cnst,
    rc_addrs,
    rc_indir,
    rc_add,
    rc_bcom,
    rc_band,
    rc_bor,
    rc_bxor,
    rc_lsh,
    rc_rsh,
    rc_sub,
    rc_eq,
    rc_ge,
    rc_gt,
    rc_le,
    rc_lt,
    rc_ne,
    rc_call,
    rc_ret,
    rc_jump,
    rc_discard,
    rc_alloc,
    rc_fastcall,
    rc_store,
    rc_rstore,
    rc_dup
};

enum directive_rc {
    rc_import,
    rc_export,
    rc_proc,
    rc_endproc,
    rc_code,
    rc_data,
    rc_align,
    rc_skip,
    rc_byte,
    rc_address,
    rc_label

};

enum sign_rc {
    SIGN_SIGNED,
    SIGN_UNSIGNED
};

enum size_rc {
    SIZE_WORD,
    SIZE_DWORD
};

enum arg_rc {
    ARG_NONE,
    ARG_CHAR,
    ARG_WORD,
    ARG_LINK
};

struct LCCEntry {
    bool isDirective;
    int name;
    op_type type;
    char arg[100];
};

struct RCEntry {
    RCEntry(bool _isD, int _name, const char * _arg = NULL, sign_rc _sign = SIGN_UNSIGNED, size_rc _size = SIZE_WORD, arg_rc _argType = ARG_NONE) {
        isDirective = _isD;
        name = _name;
        sign = _sign;
        size = _size;
        argType = _argType;
        if(_arg) {
            strcpy(arg, _arg);
        }

    }

    bool isDirective;
    int name;
    sign_rc sign;
    size_rc size;
    arg_rc argType;
    char arg[100];

};

#endif

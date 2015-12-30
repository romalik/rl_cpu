#include <stdio.h>
#include "trans2.h"
#include "oplist.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sstream>

std::vector< RCEntry > output;
int nLocals = 0;
int nArgs = 0;
int cArg = 0;


void genProcEnding() {
    output.push_back(RCEntry(0,rc_ret));
}

void genProcHeader(std::string params) {
    std::stringstream ss(params);
    std::string funcName;
    nLocals = 0;
    nArgs = 0;
    cArg = 0;
    ss >> funcName >> nLocals >> nArgs;

    RCEntry dir_label = RCEntry(1, rc_label, funcName.c_str());
    output.push_back(dir_label);

    int totalReserve = nLocals + nArgs;
    if(totalReserve > 0) {
        char buf[100];
        sprintf(buf, "%d", totalReserve);
        RCEntry op_alloc = RCEntry(0, rc_alloc, buf, SIZE_WORD, ( (totalReserve > 127)?ARG_WORD:ARG_CHAR));
        output.push_back(op_alloc);
    }
}

void parseDirective(std::string line) {
    std::stringstream ss(line);

    std::string lccWord;
    ss >> lccWord;
    std::string lccArg;
    if(lccWord.size() == line.size()) {
        lccArg = "";
    } else {
        lccArg = std::string(line, lccWord.size()+1);
    }

    if(!strcmp(lccWord.c_str(), "import")) {
        output.push_back(RCEntry(1, rc_import, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "export")) {
        output.push_back(RCEntry(1, rc_export, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "proc")) {
        output.push_back(RCEntry(1, rc_proc, lccArg.c_str()));
        genProcHeader(lccArg);
    } else if(!strcmp(lccWord.c_str(), "endproc")) {
        genProcEnding();
        output.push_back(RCEntry(1, rc_endproc, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "code")) {
        output.push_back(RCEntry(1, rc_code, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "data")) {
        output.push_back(RCEntry(1, rc_data, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "lit")) {
        output.push_back(RCEntry(1, rc_data, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "bss")) {
        output.push_back(RCEntry(1, rc_data, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "align")) {
        output.push_back(RCEntry(1, rc_align, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "skip")) {
        output.push_back(RCEntry(1, rc_skip, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "byte")) {
        output.push_back(RCEntry(1, rc_byte, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "address")) {
        output.push_back(RCEntry(1, rc_address, lccArg.c_str()));
    } else {
        fprintf(stderr, "Unknown directive %s\n", line.c_str());
        exit(1);
    }

}

void fail(std::string line) {
    fprintf(stderr, "Translator: Bad line: %s\n", line.c_str());
    exit(1);
}

void genBuiltin(std::string line, int opSize) {
    char buf[100];
    sprintf(buf, "__builtin_%s", line.c_str());
    if(opSize == 0) {
        fail(line);
        //output.push_back(RCEntry(0, rc_call0, buf, SIZE_WORD, ARG_LINK));
    } else if (opSize == 1) {
        // Stack: ...|ARG1|ARG2|
        output.push_back(RCEntry(0, rc_call0, buf, SIZE_WORD, ARG_LINK));
        // Stack: ...|RETV|ARG2|
        //              ^
        //          builtin stores result here
        output.push_back(RCEntry(0, rc_discard, "1", SIZE_WORD, ARG_CHAR));
        // Stack: ...|RETV|
    } else if (opSize == 2) {
        // Stack: ...|ARG1l|ARG1h|ARG2l|ARG2h|
        output.push_back(RCEntry(0, rc_call0, buf, SIZE_WORD, ARG_LINK));
        // Stack: ...|RETVl|RETVh|ARG2l|ARG2h|
        //              ^     ^
        //          builtin stores result here
        output.push_back(RCEntry(0, rc_discard, "2", SIZE_WORD, ARG_CHAR));
        // Stack: ...|RETVl|RETVh|
    } else {
        fail(line);
    }
}

void genBuiltinCmp(std::string line, int opSize, std::string argStr) {

    char buf[100];
    sprintf(buf, "__builtin_cmp_%s", line.c_str());
    output.push_back(RCEntry(0, rc_call0, buf, SIZE_WORD, ARG_LINK));
    if(opSize == 1) {
        fail(line);
    } else if(opSize == 2) {
        output.push_back(RCEntry(0, rc_discard, "3", SIZE_WORD, ARG_CHAR));
        output.push_back(RCEntry(0, rc_cnst, "0", SIZE_WORD, ARG_CHAR));
        output.push_back(RCEntry(0, rc_nop));
        output.push_back(RCEntry(0, rc_ne, argStr.c_str(), SIZE_WORD, ARG_LINK));

    } else {
        fail(line);
    }
}

void genRet(int opSize) {
    if(opSize == 0) {
        //do nothing, just ret
    } else if(opSize == 1) {
        //bp-4
        output.push_back(RCEntry(0, rc_addrl, "-4", SIZE_WORD, ARG_CHAR));
        output.push_back(RCEntry(0, rc_rstore, "", SIZE_WORD, ARG_NONE));
    } else if(opSize == 2) {
        //bp-5
        output.push_back(RCEntry(0, rc_addrl, "-5", SIZE_WORD, ARG_CHAR));
        output.push_back(RCEntry(0, rc_rstore, "", SIZE_DWORD, ARG_NONE));
    }
    output.push_back(RCEntry(0,rc_ret));
}

void parseOp(std::string line) {
    std::stringstream ss(line);
    std::string opRaw;
    ss >> opRaw;

    char opType = 0;
    int opSize = 0;

    if(opRaw[opRaw.size() - 1] == 'V') {
        opSize = 0;
        opType = 'V';
    } else if(opRaw[opRaw.size() - 1] == 'B') {
        opSize = 0;
        opType = 'B';
    } else {
        opType = opRaw[opRaw.size() - 2];
        opSize = opRaw[opRaw.size() - 1] - '0';
    }

    std::string op;
    if(opRaw == "DISCARD") {
        op = opRaw;
        opSize = 0;
        opType = 0;
    } else if(opType == 'V') {
        op = std::string(opRaw, 0, opRaw.size() - 1);
    } else if(opType == 'B') {
        op = std::string(opRaw, 0, opRaw.size() - 1);
    } else {
        op = std::string(opRaw, 0, opRaw.size() - 2);
    }
    int flArgNumeric = 0;
    arg_rc argType = ARG_NONE;
    std::string argStr;
    ss >> argStr;

    if(isdigit(argStr[0])) {
        int val = atoi(argStr.c_str());
        if(val >= -127 && val <= 127) {
            if(argStr.find('+') == std::string::npos) {
                flArgNumeric = 1;
                argType = ARG_CHAR;
            } else {
                argType = ARG_WORD;
            }
        } else {
            if(argStr.find('+') == std::string::npos) {
                flArgNumeric = 1;
            }
            argType = ARG_WORD;
        }
    } else if(argStr != "") {
        argType = ARG_LINK;
    } else {
        argType = ARG_NONE;
    }

    //fprintf(stderr, "Str: [%s], op: [%s], opSize: %d, opType: %c, argType: %d\n", line.c_str(), op.c_str(), opSize, opType, argType);


    if(op == "ADDRF") {
//        if(!flArgNumeric) fail(line);
        if(opSize != 1) fail(line);
        output.push_back(RCEntry(0, rc_addrf, argStr.c_str(), SIZE_WORD, argType));


    } else if(op == "ADDRG") {
        if(opSize != 1) fail(line);
        output.push_back(RCEntry(0, rc_cnst, argStr.c_str(), SIZE_WORD, argType));


    } else if(op == "ADDRL") {
//        if(!flArgNumeric) fail(line);
        if(opSize != 1) fail(line);
        char buf[100];
        if(flArgNumeric) {
            int localN = atoi(argStr.c_str()) + nArgs;
            sprintf(buf, "%d", localN);
            output.push_back(RCEntry(0, rc_addrl, buf, SIZE_WORD, ((localN<=127)?ARG_CHAR:ARG_WORD)));
        } else {
            sprintf(buf, "%s+%d", argStr.c_str(), nArgs);
            output.push_back(RCEntry(0, rc_addrl, buf, SIZE_WORD, ARG_LINK));
        }

    } else if(op == "CNST") {
        unsigned long long mask = 0xffff;
        for(int i = 0; i<opSize; i++) {
            int cval = (atol(argStr.c_str()) >> (unsigned long long)i*16ULL) & mask;
            char buf[100];
            sprintf(buf, "%d", cval);
            output.push_back(RCEntry(0, rc_cnst, buf, SIZE_WORD, (cval >=0 && cval <= 127)?ARG_CHAR:ARG_WORD));
        }

    } else if(op == "INDIR") {
        if(opType != 'B') {
            if(opSize == 1) {
                output.push_back(RCEntry(0, rc_indir, "", SIZE_WORD, ARG_NONE));
            } else if(opSize == 2) {
                //fprintf(stderr, "INDIR 2\n");
                output.push_back(RCEntry(0, rc_indir, "", SIZE_DWORD, ARG_NONE));
            } else {
                //hmmm strange..
                fprintf(stderr,"Strange indir attempt. I'd better crash..\n");
                fail(line);
            }
        } else {
            //skip for structure
        }
    } else if(op == "NEG") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bxor, "0xffff", SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_add, "0x01", SIZE_WORD, ARG_CHAR));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_cnst, "0xffff", SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_cnst, "0xffff", SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_bxor, "", SIZE_DWORD, ARG_NONE));
            output.push_back(RCEntry(0, rc_cnst, "0x01", SIZE_WORD, ARG_CHAR));
            output.push_back(RCEntry(0, rc_cnst, "0x00", SIZE_WORD, ARG_CHAR));
            output.push_back(RCEntry(0, rc_add, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "BCOM") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bxor, "0xffff", SIZE_WORD, ARG_WORD));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_cnst, "0xffff", SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_cnst, "0xffff", SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_bxor, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "ADD") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_add, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_add, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "SUB") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_sub, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_sub, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "BAND") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_band, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_band, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "BOR") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bor, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_bor, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "BXOR") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bxor, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_bxor, "", SIZE_DWORD, ARG_NONE));
        } else {
            fail(line);
        }
    } else if(op == "DIV") {
        genBuiltin(line, opSize);
    } else if(op == "MOD") {
        genBuiltin(line, opSize);
    } else if(op == "MUL") {
        genBuiltin(line, opSize);
    } else if(op == "LSH") {
        if((opSize == 1) && (output.back().name == rc_cnst)) {
            int val = atoi(output.back().arg);
            output.pop_back();
            while(val > 0) {
                output.push_back(RCEntry(0, rc_lsh, "", SIZE_WORD, ARG_NONE));
                val--;
            }
        } else {
            genBuiltin(line, opSize);
        }
    } else if(op == "RSH") {
        if((opSize == 1) && (output.back().name == rc_cnst)) {
            int val = atoi(output.back().arg);
            output.pop_back();
            while(val > 0) {
                output.push_back(RCEntry(0, rc_rsh, "", SIZE_WORD, ARG_NONE));
                val--;
            }
        } else {
            genBuiltin(line, opSize);
        }
    } else if(op == "ASGN") {
        if(opType == 'B') {
            output.push_back(RCEntry(0, rc_cnst, argStr.c_str(), SIZE_WORD, argType));
            output.push_back(RCEntry(0, rc_call0, "__builtin_memcpy", SIZE_WORD, ARG_LINK));
            output.push_back(RCEntry(0, rc_discard, "3", SIZE_WORD, ARG_CHAR));
        } else {
            if(opSize == 1) {
                output.push_back(RCEntry(0, rc_store, "", SIZE_WORD, ARG_NONE));
            } else if(opSize == 2) {
                output.push_back(RCEntry(0, rc_store, "", SIZE_DWORD, ARG_NONE));
            } else {
                fail(line);
            }
        }
    } else if(op == "EQ") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_eq, argStr.c_str(), SIZE_WORD, ARG_LINK));
        } else {
            genBuiltinCmp(line,opSize,argStr);
        }
    } else if(op == "NE") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_ne, argStr.c_str(), SIZE_WORD, ARG_LINK));
        } else {
            genBuiltinCmp(line,opSize,argStr);
        }
    } else if(op == "GT") {
        if(opSize == 1) {
            if(opType == 'U' || opType == 'P') {
                output.push_back(RCEntry(0, rc_ugt, argStr.c_str(), SIZE_WORD, ARG_LINK));
            } else {
                output.push_back(RCEntry(0, rc_gt, argStr.c_str(), SIZE_WORD, ARG_LINK));
            }
        } else {
            genBuiltinCmp(line,opSize,argStr);
        }
    } else if(op == "GE") {
        if(opSize == 1) {
            if(opType == 'U' || opType == 'P') {
                output.push_back(RCEntry(0, rc_uge, argStr.c_str(), SIZE_WORD, ARG_LINK));
            } else {
                output.push_back(RCEntry(0, rc_ge, argStr.c_str(), SIZE_WORD, ARG_LINK));
            }
        } else {
            genBuiltinCmp(line,opSize,argStr);
        }
    } else if(op == "LT") {
        if(opSize == 1) {
            if(opType == 'U' || opType == 'P') {
                output.push_back(RCEntry(0, rc_ult, argStr.c_str(), SIZE_WORD, ARG_LINK));
            } else {
                output.push_back(RCEntry(0, rc_lt, argStr.c_str(), SIZE_WORD, ARG_LINK));
            }
        } else {
            genBuiltinCmp(line,opSize,argStr);
        }
    } else if(op == "LE") {
        if(opSize == 1) {
            if(opType == 'U' || opType == 'P') {
                output.push_back(RCEntry(0, rc_ule, argStr.c_str(), SIZE_WORD, ARG_LINK));
            } else {
                output.push_back(RCEntry(0, rc_le, argStr.c_str(), SIZE_WORD, ARG_LINK));
            }
        } else {
            genBuiltinCmp(line,opSize,argStr);
        }
    } else if(op == "ARG") {
        if(opSize == 1) {
            char buf[100];
            sprintf(buf, "%d", cArg);
            output.push_back(RCEntry(0, rc_addrl, buf, SIZE_WORD, (cArg <= 127)?ARG_CHAR:ARG_WORD));
            output.push_back(RCEntry(0, rc_rstore, "", SIZE_WORD, ARG_NONE));
            cArg++;
        } else if(opSize == 2) {
            char buf[100];
            sprintf(buf, "%d", cArg);
            output.push_back(RCEntry(0, rc_addrl, buf, SIZE_WORD, (cArg <= 127)?ARG_CHAR:ARG_WORD));
            output.push_back(RCEntry(0, rc_rstore, "", SIZE_DWORD, ARG_NONE));
            cArg+=2;
        } else {
            fail(line);
        }
    } else if(op == "CALL") {
        if(opSize == 0) {
            output.push_back(RCEntry(0, rc_call0, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 1) {
            output.push_back(RCEntry(0, rc_call1, "", SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_call2, "", SIZE_WORD, ARG_NONE));
        } else {
            fail(line);
        }
        cArg = 0;
    } else if(op == "RET") {
        genRet(opSize);
    } else if(op == "JUMP") {
        output.push_back(RCEntry(0, rc_jump, "", SIZE_WORD, ARG_NONE));
    } else if(op == "LABEL") {
        output.push_back(RCEntry(1, rc_label, argStr.c_str(), SIZE_WORD, ARG_WORD));
    } else if(op == "CVI" || op == "CVU" || op == "CVF" || op == "CVP") {
        int fromSz = atoi(argStr.c_str());
        int toSz = opSize;
        char buf[100];
        sprintf(buf, "%d", abs(fromSz - toSz));

        if(fromSz > toSz) {
            if(fromSz - toSz < 127) {
                output.push_back(RCEntry(0, rc_discard, buf, SIZE_WORD, ARG_CHAR));
            } else {
                output.push_back(RCEntry(0, rc_discard, buf, SIZE_WORD, ARG_WORD));
            }
        } else if(fromSz < toSz) {
            for(int i = 0; i<toSz - fromSz; i++) {
                output.push_back(RCEntry(0,rc_cnst, "0", SIZE_WORD, ARG_CHAR));
            }
        }
    } else if(op == "DISCARD") {
        if(argType == ARG_CHAR || argType == ARG_WORD) {
            if(atoi(argStr.c_str())!=0) {
                output.push_back(RCEntry(0, rc_discard, argStr.c_str(), SIZE_WORD, argType));
            }
        } else {
            fail(line);
        }
    }

}

void parseLine(std::string line) {
    if(line[0] >= 'a' && line[0] <= 'z') { //lower-case - directive!
        parseDirective(line);
    } else {
        parseOp(line);
    }
}

void optimize() {
    std::vector< RCEntry >::iterator it = output.begin();
    while(it != output.end()) {
        std::vector< RCEntry >::iterator prevIt;
        if(it != output.begin() && it->isDirective == 0 && it->argType == ARG_NONE) {
            prevIt = it - 1;
            if(it->name == rc_indir && it->size == SIZE_WORD) { //indirect locals, frame and globals
                if(prevIt->name == rc_addrf ||
                   prevIt->name == rc_addrl ||
                   prevIt->name == rc_cnst  ||
                   prevIt->name == rc_addrs) {
                    if(prevIt->needIndir == 0) {
                        prevIt->needIndir = 1;
                        it = output.erase(it);
                        continue;
                    }
                }
            } else if((it->name >= rc_add && it->name <= rc_bxor)  || (it->name >= rc_call0 && it->name <= rc_call2) || it->name == rc_jump) { //immediate arithm & call ops
                if(it->size == SIZE_WORD && prevIt->name == rc_cnst && prevIt->size == SIZE_WORD && prevIt->needIndir == 0) {
                    prevIt->name = it->name;
                    if((it->name >= rc_call0 && it->name <= rc_call2) || it->name == rc_jump) { //force word size arg for call&jump
                        prevIt->argType = ARG_LINK;
                    }

                    it = output.erase(it);
                    continue;
                }
            }
        }
        it++;

    }
}

void dumpCode() {
    for(int i = 0; i<output.size(); i++) {
        if(output[i].isDirective == 1) {
            printf(".%s %s\n", directive_rc_str[output[i].name], output[i].arg);
        } else {
            char argTypeStr[10];
            if(output[i].argType == ARG_NONE) {
                sprintf(argTypeStr, "");
            } else if(output[i].argType == ARG_CHAR) {
                sprintf(argTypeStr, "_b");
            } else {
                sprintf(argTypeStr, "_w");
            }
            printf("%s%s%s%s %s\n",
                   output[i].needIndir?"i":"",
                   opname_rc_str[output[i].name],
                   output[i].size == SIZE_DWORD?"2":"",
                   argTypeStr,
                   output[i].arg);
        }
    }
}

int getOpNum(std::vector< RCEntry > & code) {
    int res = 0;
    for(int i = 0; i<code.size(); i++) {
        if(code[i].isDirective == 0) {
            res++;
        }
    }
    return res;
}

int main(int argc, char ** argv) {
    std::string line;
    while(1) {
        std::getline(std::cin, line);
        if(line == "")
            break;
        parseLine(line);
    }

    if(argc > 1 && !strcmp(argv[1], "-o")) {
        optimize();
    }
    dumpCode();
    return 0;
}

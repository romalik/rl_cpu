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


void genProcHeader(std::string params) {
    std::stringstream ss(params);
    std::string funcName;
    nLocals = 0;
    nArgs = 0;
    ss >> funcName >> nLocals >> nArgs;

    RCEntry dir_label = RCEntry(1, rc_label, funcName.c_str());
    output.push_back(dir_label);

    //////WHY?///////
    nArgs += 5;
    /////End Why////


    int totalReserve = nLocals + nArgs;

    char buf[100];
    sprintf(buf, "%d", totalReserve);
    RCEntry op_alloc = RCEntry(0, rc_alloc, buf, SIGN_UNSIGNED, SIZE_WORD, ( (totalReserve > 127)?ARG_WORD:ARG_CHAR));
    output.push_back(op_alloc);
}

void parseDirective(std::string line) {
    std::stringstream ss(line);

    std::string lccWord;
    ss >> lccWord;
    std::string lccArg = std::string(line, lccWord.size()+1);

    if(!strcmp(lccWord.c_str(), "import")) {
        output.push_back(RCEntry(1, rc_import, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "export")) {
        output.push_back(RCEntry(1, rc_export, lccArg.c_str()));
    } else if(!strcmp(lccWord.c_str(), "proc")) {
        output.push_back(RCEntry(1, rc_proc, lccArg.c_str()));
        genProcHeader(lccArg);
    } else if(!strcmp(lccWord.c_str(), "endproc")) {
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
    printf("Bad line: %s\n", line.c_str());
    exit(1);
}

void parseOp(std::string line) {
    std::stringstream ss(line);
    std::string opRaw;
    ss >> opRaw;

    char opType = opRaw[opRaw.size() - 1];
    int opSize = opRaw[opRaw.size() - 2] - '0';

    std::string op = std::string(opRaw, 0, opRaw.size() - 2);

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


    if(op == "ADDRF") {
        if(!flArgNumeric) fail(line);
        if(opSize != 1) fail(line);
        output.push_back(RCEntry(0, rc_addrf, argStr.c_str(), SIGN_UNSIGNED, SIZE_WORD, argType));


    } else if(op == "ADDRG") {
        if(opSize != 1) fail(line);
        output.push_back(RCEntry(0, rc_cnst, argStr.c_str(), SIGN_UNSIGNED, SIZE_WORD, argType));


    } else if(op == "ADDRL") {
        if(!flArgNumeric) fail(line);
        if(opSize != 1) fail(line);
        output.push_back(RCEntry(0, rc_addrl, argStr.c_str(), SIGN_UNSIGNED, SIZE_WORD, argType));


    } else if(op == "CNST") {
        unsigned long long mask = 0xffff;
        for(int i = 0; i<opSize; i++) {
            int cval = (atol(argStr.c_str()) >> (unsigned long long)i*16ULL) & mask;
            char buf[100];
            sprintf(buf, "%d", cval);
            output.push_back(RCEntry(0, rc_cnst, buf, SIGN_UNSIGNED, SIZE_WORD, (cval >=0 && cval <= 127)?ARG_CHAR:ARG_WORD));
        }

    } else if(op == "INDIR") {
        if(opType != 'B') {
            if(opSize == 1) {
                output.push_back(RCEntry(0, rc_indir, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
            } else if(opSize == 2) {
                output.push_back(RCEntry(0, rc_indir, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
            } else {
                //hmmm strange..
                printf("Strange indir attempt. I'd better crash..\n");
                fail(line);
            }
        } else {
            //skip for structure
        }
    } else if(op == "NEG") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bxor, "0xffff", SIGN_UNSIGNED, SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_add, "0x01", SIGN_UNSIGNED, SIZE_WORD, ARG_CHAR));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_cnst, "0xffff", SIGN_UNSIGNED, SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_cnst, "0xffff", SIGN_UNSIGNED, SIZE_WORD, ARG_WORD));
            output.push_back(RCEntry(0, rc_bxor, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
            output.push_back(RCEntry(0, rc_cnst, "0x01", SIGN_UNSIGNED, SIZE_WORD, ARG_CHAR));
            output.push_back(RCEntry(0, rc_cnst, "0x00", SIGN_UNSIGNED, SIZE_WORD, ARG_CHAR));
            output.push_back(RCEntry(0, rc_add, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
        }
    } else if(op == "ADD") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_add, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_add, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
        }
    } else if(op == "SUB") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_sub, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_sub, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
        }
    } else if(op == "BAND") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_band, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_band, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
        }
    } else if(op == "BOR") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bor, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_bor, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
        }
    } else if(op == "BXOR") {
        if(opSize == 1) {
            output.push_back(RCEntry(0, rc_bxor, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
        } else if(opSize == 2) {
            output.push_back(RCEntry(0, rc_bxor, "", SIGN_UNSIGNED, SIZE_DWORD, ARG_NONE));
        }
    } else if(op == "DIV") {
        //fuck that
    } else if(op == "MOD") {
        //fuck that
    } else if(op == "MUL") {
        //fuck that
    } else if(op == "LSH") {
        if((opSize == 1) && (output.back().name == rc_cnst)) {
            int val = atoi(output.back().arg);
            output.pop_back();
            while(val > 0) {
                output.push_back(RCEntry(0, rc_lsh, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
                val--;
            }
        } else {
            //fuck that..
        }
    } else if(op == "RSH") {
        if((opSize == 1) && (output.back().name == rc_cnst)) {
            int val = atoi(output.back().arg);
            output.pop_back();
            while(val > 0) {
                output.push_back(RCEntry(0, rc_rsh, "", SIGN_UNSIGNED, SIZE_WORD, ARG_NONE));
                val--;
            }
        } else {
            //fuck that..
        }
    } else if(op == "ASGN") {

    }
}

void parseLine(std::string line) {
    if(line[0] >= 'a' && line[0] <= 'z') { //lower-case - directive!
        parseDirective(line);
    } else {
        parseOp(line);
    }
}

int main(int argc, char ** argv) {
    std::string line;
    while(1) {
        std::getline(std::cin, line);
        if(line == "")
            break;
        parseLine(line);

    }
}

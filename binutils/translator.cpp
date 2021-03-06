#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int dirnum = 13;
char directives[][13] = {
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
	"label",
	"address"
};


enum directive {
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
	LABEL_DIRECTIVE,
	ADDRESS
};

int opnum = 49;
char opnames[][10] = {
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
    "DISCARD",
    "FASTCALL",
    "ALLOC",
    "DISCARD1",
    "ALLOC1",
    "STORE",
    "RSTORE",
    "UEQ",
    "UGE",
    "UGT",
    "ULE",
    "ULT",
    "UNE",
    "ADDRS",
    "DUP"
};


enum arg_type {
	NO_ARG,
	SHORT_ARG,
	LONG_ARG,
	LINK_TIME_ARG
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

enum opname {
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
    UEQ,
    UGE,
    UGT,
    ULE,
    ULT,
    UNE,
    ADDRS,
    DUP

};

typedef struct Operation_t {
	int flInstr; // 0 - directive, 1 - instruction
	int type;
	int size;
	int name;
	int flIndir;
	int flArg; // 0 - no arg, 1 - short arg, 2 - long arg
	std::string arg;
	std::string str;
	Operation_t() : type(0), size(0), name(0), flIndir(0), flArg(0), flInstr(0) {}
	Operation_t(int _name, int _flInstr, std::string _arg = std::string(""), int _flArg = 0) {
		name = _name;
		flInstr = _flInstr;
		arg = _arg;
		flArg = _flArg;
        flIndir = 0;
        size = 0;
	}
	Operation_t(int _name, int _flInstr, int _arg, int _flArg = 0) {
		name = _name;
		flInstr = _flInstr;
		char buf[100];
		sprintf(buf, "%d", _arg);
		arg = std::string(buf);
		flArg = _flArg;
        flIndir = 0;
        size = 0;
	}

} Operation;


int parseOp(const char * str) {
	for(int i = 0; i<opnum; i++) {
		if(strlen(str) >= strlen(opnames[i])) {
			if(!memcmp(opnames[i], str, strlen(opnames[i])))
				return i;
		}
	}
	return -1;
}
int parseOpType(const char * str, int op) {
	char tp = str[strlen(opnames[op])];
	if(tp == 'I') {
        return TYPE_I;
	} else if(tp == 'U') {
		return TYPE_U;
	} else if(tp == 'P') {
		return TYPE_P;
	} else if(tp == 'F') {
		return TYPE_F;
	} else if(tp == 'V') {
		return TYPE_V;
	} else if(tp == 'B') {
		return TYPE_B;
	}
	return TYPE_NONE;
}


int parseOpSize(const char * str, int op) {
	char tp = str[strlen(opnames[op]) + 1];
	if(tp > '0' && tp < '9')
		return tp - '0';
	return 0;
}
int parseDirective(const char * str) {
	for(int i = 0; i<dirnum; i++) {
		if(strlen(str) >= strlen(directives[i])) {
			if(!memcmp(directives[i], str, strlen(directives[i])))
				return i;
		}
	}
	return -1;
}


class Sample {
public:

	std::vector<int> data;
	Sample() {}
	Sample  operator()(int i) { data.push_back(i); return *this; }
};

int opIn(Operation op, Sample sampl) {
	for(int i = 0; i<sampl.data.size(); i++) {
		if(op.name == sampl.data[i])
			return 1;
	}
	return 0;
}

int parseArg(const char * s, int off) {
	if(strlen(s) <= off)
		return 0;

    return strtol(s+off, NULL, 0);
}

int currentArg = 0;
int currentArgFrameSize = 0;
int currentLocalFrameSize = 0;


void procStructure(std::vector<Operation> & asmCode, Operation op, std::vector<Operation>::reverse_iterator prevOp) {
    //structure
    if(op.name == INDIR) {
        op.size = 1;
        asmCode.push_back(op);
    } else if(op.name == ASGN) {
        if(prevOp == asmCode.rend() || (*prevOp).name != INDIR) {
            printf("Structure assign fail");
            exit(1);
        }
        asmCode.pop_back();

        int wordsToCopy = strtol(op.arg.c_str(), NULL, 0);

        Operation loadSizeOp(CNST, 1, op.arg);

        if(wordsToCopy > 0 && wordsToCopy <= 255) {
            loadSizeOp.flArg = SHORT_ARG;
        } else {
            loadSizeOp.flArg = LONG_ARG;
        }
        asmCode.push_back(loadSizeOp);
        Operation constMemcpyAddr(CNST, 1, "memcpy_r", LINK_TIME_ARG);
        asmCode.push_back(constMemcpyAddr);
        Operation callFastcallAdaptor(CALL, 1, "fastcall3", LINK_TIME_ARG);
        asmCode.push_back(callFastcallAdaptor);

        //okay.. now we have:
        // ... arg1|arg2|arg3|funcAddr|result| <SP>
        //so, we should push SP-5
        // ... arg1|arg2|arg3|funcAddr|result|&arg1| <SP>
        //rstore
        // ... result|arg2|arg3|funcAddr| <SP>
        //discard_b 3
        // ... result| <SP>


        Operation pushTarget(ADDRS, 1, "-5", LONG_ARG);
        Operation rstore(RSTORE, 1, "", NO_ARG);
        Operation discard(DISCARD, 1, "3", SHORT_ARG);

        asmCode.push_back(pushTarget);
        asmCode.push_back(rstore);
        asmCode.push_back(discard);
    }
}

void procLabel(std::vector<Operation> & asmCode, Operation op, std::vector<Operation>::reverse_iterator prevOp) {
    op.flInstr = 0;
    op.name = LABEL_DIRECTIVE;
    asmCode.push_back(op);
}

void procArg(std::vector<Operation> & asmCode, Operation op, std::vector<Operation>::reverse_iterator prevOp) {
    if(op.size == 1) {
        op.name = ADDRL;
        op.flArg = SHORT_ARG;
        char buf[100];
        sprintf(buf, "%d", currentArg);
        op.arg = std::string(buf);
        asmCode.push_back(op);
        asmCode.push_back(Operation(RSTORE, 1));
        currentArg++;
    } else if(op.size == 2) {
        op.name = ADDRL;
        op.flArg = SHORT_ARG;
        char buf[100];
        sprintf(buf, "%d", currentArg);
        op.arg = std::string(buf);
        op.size = 1;
        asmCode.push_back(op);
        Operation rStore_op = Operation(RSTORE, 1);
        rStore_op.size = 2;
        asmCode.push_back(rStore_op);
        currentArg+=2;
    } else {
        printf("Bad arg size!\n");
        exit(1);
    }
}

void procCVT(std::vector<Operation> & asmCode, Operation op, std::vector<Operation>::reverse_iterator prevOp) {
    // if arg(from) > size(to) - discard (arg-size)
    // if arg(from) < size(to) - alloc (size-arg)
    int fromSz = atoi(op.arg.c_str());
    int toSz = op.size;
    Operation newOp;


    char buf[100];
    if(fromSz > toSz) {
        newOp.name = DISCARD;
        int nDiscard = fromSz - toSz;
        sprintf(buf,"%d",fromSz - toSz);
        newOp.arg = std::string(buf);
        newOp.flInstr = 1;
        if(nDiscard == 1) {
            newOp.name = DISCARD1;
        } else if(nDiscard >=0 || nDiscard <= 255) {
            newOp.flArg = SHORT_ARG;
        } else {
            newOp.flArg = LONG_ARG;
        }

        asmCode.push_back(newOp);
    } else if(toSz > fromSz) {
        for(int i = 0; i<toSz - fromSz; i++) {
            Operation push_zero(CNST, 1, "0", SHORT_ARG);
            asmCode.push_back(push_zero);
        }
    }
}

void procFastcallFunc(std::vector<Operation> & asmCode, Operation op, std::vector<Operation>::reverse_iterator prevOp) {
    char funcName[100];
    sprintf(funcName, "%s%s%d", opnames[op.name], typenames[op.type], op.size);

    char adaptorName[100];

    if(op.size > 1) {
        sprintf(adaptorName, "fastcall2_long");

        Operation constMemcpyAddr(CNST, 1, std::string(funcName), LINK_TIME_ARG);
        asmCode.push_back(constMemcpyAddr);
        Operation callFastcallAdaptor(CALL, 1, std::string(adaptorName), LINK_TIME_ARG);
        asmCode.push_back(callFastcallAdaptor);
        //okay.. now we have:
        // ... arg1l|arg1h|arg2l|arg2h|funcAddr|result_l|result_h| <SP>
        //so, we should push SP-6
        // ... arg1l|arg1h|arg2l|arg2h|funcAddr|result_l|result_h|&arg1h| <SP>
        //rstore
        // ... arg1l|result_h|arg2l|arg2h|funcAddr|result_l| <SP>
        //so, we should push SP-6
        // ... arg1l|result_h|arg2l|arg2h|funcAddr|result_l|&arg1_l| <SP>
        //rstore
        // ... result_l|result_h|arg2l|arg2h|funcAddr| <SP>
        //discard_b 3
        // ... result_l|result_h| <SP>
        Operation pushTargetH(ADDRS, 1, "-6", LONG_ARG);
        Operation rstoreH(RSTORE, 1, "", NO_ARG);
        Operation pushTargetL(ADDRS, 1, "-6", LONG_ARG);
        Operation rstoreL(RSTORE, 1, "", NO_ARG);
        Operation discard(DISCARD, 1, "3", SHORT_ARG);

        asmCode.push_back(pushTargetH);
        asmCode.push_back(rstoreH);
        asmCode.push_back(pushTargetL);
        asmCode.push_back(rstoreL);
        asmCode.push_back(discard);


    } else {
        sprintf(adaptorName, "fastcall2");
        Operation constMemcpyAddr(CNST, 1, std::string(funcName), LINK_TIME_ARG);
        asmCode.push_back(constMemcpyAddr);
        Operation callFastcallAdaptor(CALL, 1, std::string(adaptorName), LINK_TIME_ARG);
        asmCode.push_back(callFastcallAdaptor);

        //okay.. now we have:
        // ... arg1|arg2|funcAddr|result| <SP>
        //so, we should push SP-4
        // ... arg1|arg2|funcAddr|result|&arg1| <SP>
        //rstore
        // ... result|arg2|funcAddr| <SP>
        //discard_b 2
        // ... result| <SP>


        Operation pushTarget(ADDRS, 1, "-4", LONG_ARG);
        Operation rstore(RSTORE, 1, "", NO_ARG);
        Operation discard(DISCARD, 1, "2", SHORT_ARG);

        asmCode.push_back(pushTarget);
        asmCode.push_back(rstore);
        asmCode.push_back(discard);
    }

}


void addOp(std::vector<Operation> & asmCode, Operation op) {

    std::vector<Operation>::reverse_iterator prevOp = asmCode.rbegin();


    if(isdigit(op.arg[0])) {
        int val = atoi(op.arg.c_str());
        if(val >=0 && val <= 255 && op.arg.find('+') == std::string::npos) {
            op.flArg = SHORT_ARG;
        } else {
            op.flArg = LONG_ARG;
        }
    } else if(op.arg != "") {
        op.flArg = LINK_TIME_ARG;
    } else {
        op.flArg = NO_ARG;
    }



    if(op.type == TYPE_B) {
        procStructure(asmCode, op, prevOp);
    } else if(op.name == LABEL) {
        procLabel(asmCode, op, prevOp);
    } else if(op.name == ARG) {
        procArg(asmCode, op, prevOp);
    } else if(opIn(op, Sample()(CVI)(CVU)(CVP))) {
        procCVT(asmCode,op,prevOp);
    } else if(op.name == ASGN) {
        //check size here
        if(op.size == 1 || op.size == 2) {
            op.name = STORE;
            asmCode.push_back(op);
        } else {
            printf("Bad assign size!\n");
            exit(1);

        }
    } else if(op.name == CALL) {
        currentArg = 0;
        op.size = 1;
        if((*prevOp).name == CNST) {
            (*prevOp).name = CALL;
        } else {
            asmCode.push_back(op);
        }
    } else if(op.name == INDIR) {
        if(op.size == 1 && opIn(*prevOp, Sample()(ADDRF)(ADDRG)(ADDRL)(CNST)) && ((*prevOp).flIndir == 0)) {
            (*prevOp).flIndir = 1;
        } else {
            asmCode.push_back(op);
        }
    } else if(op.name == NEG && op.size == 1) {
        Operation op1;
        op1.name = BXOR;
        op1.flArg = LONG_ARG;
        op1.flInstr = 1;
        op1.arg = "0xffff";
        asmCode.push_back(op1);
        Operation op2;
        op2.name = ADD;
        op2.flArg = SHORT_ARG;
        op2.flInstr = 1;
        op2.arg = "1";
        asmCode.push_back(op2);
    } else if(op.name == BCOM && op.size == 1) {
        Operation op1;
        op1.name = BXOR;
        op1.flArg = LONG_ARG;
        op1.flInstr = 1;
        op1.arg = "0xffff";
        asmCode.push_back(op1);
    } else if(op.size > 1) {
        if(op.name == CNST) {
            unsigned long long mask = 0xffff;
            for(int i = 0; i<op.size; i++) {
                int cval = (atol(op.arg.c_str()) >> i*16) & mask;
                char buf[100];
                sprintf(buf, "%d", cval);
                //mask = mask << 16;
                Operation newOp;
                newOp.flInstr = 1;
                newOp.name = CNST;
                newOp.flArg = LONG_ARG;
                newOp.arg = std::string(buf);
                asmCode.push_back(newOp);
            }
            return;
        } else if(opIn(op, Sample()(ADD)(BAND)(BOR)(BXOR)(LSH)(RSH)(SUB)(EQ)(GE)(GT)(LE)(LT)(NE)(UEQ)(UGE)(UGT)(ULE)(ULT)(UNE)(RET)(STORE)(RSTORE))) {
            asmCode.push_back(op);
        } else if(opIn(op, Sample()(DIV)(MOD)(MUL)(NEG)(BCOM))) {
            procFastcallFunc(asmCode, op, prevOp);
        } else {
            printf("Bad opcode! %d %s %d\n", op.name, opnames[op.name], op.size);
            exit(1);

        }
    } else if(opIn(op, Sample()(DIV)(MOD)(MUL))) {
        procFastcallFunc(asmCode, op, prevOp);
    } else if(opIn(op, Sample()(LSH)(RSH) )) {
        asmCode.push_back(op);
    } else if(opIn(op, Sample()(ADD)(SUB)(BAND)(BOR)(BXOR)(JUMP)(CALL) )) {
        if((*prevOp).name == CNST) {
            (*prevOp).name = op.name;
        }
    } else if(opIn(op, Sample()(EQ)(GE)(GT)(LE)(LT)(NE))){
        if(op.type == TYPE_U) {
            //explicitly convert to unsigned comparison
            op.name += UEQ - EQ;
        }
        asmCode.push_back(op);

    } else if(op.name == ADDRG) {
        op.name = CNST;
        asmCode.push_back(op);
    } else if(opIn(op, Sample()(ADDRF)(CNST)(RET))) {
        asmCode.push_back(op);
    } else if(opIn(op, Sample()(DISCARD))) {
        op.name = DISCARD1;
        op.flArg = NO_ARG;
        asmCode.push_back(op);
    } else if(op.name == ADDRL) {
        int cArg = strtol(op.arg.c_str(), NULL, 0);
        char buf[100];
//        sprintf(buf,"%d",cArg + currentArgFrameSize);
        sprintf(buf,"%s+%d",op.arg.c_str(), currentArgFrameSize);
        op.arg = std::string(buf);
        asmCode.push_back(op);
    } else {
        printf("Bad opcode! %d %s %d\n", op.name, opnames[op.name], op.size);
        exit(1);
    }

}

std::string getArg(std::string str) {
	int i = 0;
	for(i = 0; i<str.size(); i++) {
		if(str[i] == ' ') {
			i++;
			break;
		}
	}
	std::string res;
	if(i != str.size()) {
		res = std::string(str, i);
	}
	return res;
}


std::string getWord(std::string str) {
	int i = 0;
	for(i = 0; i<str.size(); i++) {
		if(str[i] == ' ') {
			break;
		}
	}
	std::string res;
	if(i != str.size()) {
		res = std::string(str, 0, i);
	}
	return res;
}

int main() {
	std::vector<Operation> asmCode;

	while(1) {
		std::string line;

		std::getline(std::cin, line);
		if(line == "")
			break;
		int i = 0;
		if((i = parseDirective(line.c_str())) != -1) {
            //printf("Directive %d\n", i);
			Operation newOp;
            newOp.flInstr = 0;
            newOp.name = i;
            if(i == LIT || i == BSS || i == DATA) {
                newOp.name = DATA;
            }

			newOp.arg = getArg(line);
			asmCode.push_back(newOp);
			if(newOp.name == PROC) {
				std::string lName = getWord(newOp.arg);
				asmCode.push_back(Operation(LABEL_DIRECTIVE, 0, lName, LINK_TIME_ARG));


				std::string localsStr = getArg(newOp.arg);
				std::string argsStr = getArg(localsStr);
				currentLocalFrameSize = atoi(localsStr.c_str());
				currentArgFrameSize = atoi(argsStr.c_str());

                    currentArgFrameSize += 5; //reserve for 2 long args and fn ptr

                if(currentArgFrameSize == 0) {
                  //do nothing
                } else if(currentArgFrameSize >0 && currentArgFrameSize <= 255) {
					asmCode.push_back(Operation(ALLOC, 1, currentArgFrameSize, SHORT_ARG));
				} else {
					asmCode.push_back(Operation(ALLOC, 1, currentArgFrameSize, LONG_ARG));
				}

                if(currentLocalFrameSize == 0) {
                    //do nothing
                } else if(currentLocalFrameSize >0 && currentLocalFrameSize <= 255) {
					asmCode.push_back(Operation(ALLOC, 1, currentLocalFrameSize, SHORT_ARG));
				} else {
					asmCode.push_back(Operation(ALLOC, 1, currentLocalFrameSize, LONG_ARG));
				}



			}


		} else if((i = parseOp(line.c_str())) != -1) {
			int type = parseOpType(line.c_str(), i);
			int size = parseOpSize(line.c_str(), i);

			Operation newOp;
			newOp.flInstr = 1;
			newOp.type = type;
			newOp.size = size;
			newOp.name = i;
			newOp.arg = getArg(line);
            //printf("%s : Opcode: %d type %d size %d arg %s\n", line.c_str(), i, type, size, newOp.arg.c_str());
			addOp(asmCode, newOp);

		} else {
			printf("Unknown text [%s]!\n", line.c_str());
            exit(1);
		}
	}

    /*
	for(int i = 0; i<asmCode.size(); i++) {
		printf("%s i%d s%d t%d arg%d %s\n", opnames[asmCode[i].name], asmCode[i].flIndir, asmCode[i].size, asmCode[i].type, asmCode[i].flArg, asmCode[i].arg.c_str() );
	}
    */

	for(int i = 0; i<asmCode.size(); i++) {
		char tmp[100];
		
		if(asmCode[i].flInstr == 0) {
			char *s = tmp;
			char *p = directives[asmCode[i].name];
			while(1) {
				*s = tolower(*p);
				if(!*p) break;
				s++;
				p++;
			}
			printf(".%s %s\n", tmp, asmCode[i].arg.c_str());
		} else {

			char *s = tmp;
			char *p = opnames[asmCode[i].name];
			while(1) {
				*s = tolower(*p);
				if(!*p) break;
				s++;
				p++;
			}
            printf("%s%s%s", ((asmCode[i].flIndir)?"i":""), tmp, (asmCode[i].size == 2)?"2":"");
			int argType = asmCode[i].flArg;
			if(argType == NO_ARG) {
				printf("\n");
			} else if(argType == SHORT_ARG) {
				printf("_b %s\n", asmCode[i].arg.c_str());
			} else if(argType == LONG_ARG) {
				printf("_w %s\n", asmCode[i].arg.c_str());
			} else if(argType == LINK_TIME_ARG) {
                printf("_w %s\n", asmCode[i].arg.c_str());
			} 
		}
	}

	return 0;
}

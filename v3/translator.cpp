#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int dirnum = 4;
char directives[][10] = {
	"export",
	"code",
	"proc",
	"endproc"
};

int opnum = 34;
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
		"FASTCALL"

};


enum arg_type {
	NO_ARG,
	SHORT_ARG,
	LONG_ARG,
	LINK_TIME_ARG
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
		FASTCALL
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
		return 1;
	} else if(tp == 'U') {
		return 2;
	} else if(tp == 'P') {
		return 3;
	} else if(tp == 'F') {
		return 4;
	} else if(tp == 'V') {
		return 5;
	} else if(tp == 'B') {
		return 6;
	}
	return 0;
}


int parseOpSize(const char * str, int op) {
	char tp = str[strlen(opnames[op+1])];
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

	return atoi(s+off);
}

void addOp(std::vector<Operation> & asmCode, Operation op) {
	if(op.size > 1) {
		Operation newOp;
		newOp.name = FASTCALL;
		newOp.arg = std::string(opnames[op.name]);
		asmCode.push_back(newOp);
		return;
	}

	std::vector<Operation>::reverse_iterator prevOp = asmCode.rbegin();
	
	//parse arg

	if(isdigit(op.arg[0])) {
		int val = atoi(op.arg.c_str());
		if(val >=0 && val <= 255) {
			op.flArg = SHORT_ARG;
		} else {
			op.flArg = LONG_ARG;
		}
	} else if(op.arg != "") {
		op.flArg = LINK_TIME_ARG;
	} else {
		op.flArg = NO_ARG;
	}


	if(op.name == INDIR) {
		if(opIn(*prevOp, Sample()(ADDRF)(ADDRG)(ADDRL)(CNST))) {
			(*prevOp).flIndir = 1;
			return;
		}
	}

	if(opIn(op, Sample()(ADD)(SUB)(BAND)(BOR)(BXOR) )) {
		if((*prevOp).name == CNST) {
			(*prevOp).name = op.name;
			return;
		}
	}

	if(op.name == NEG) {
		Operation op1;
		op1.name = BXOR;
		op1.flArg = LONG_ARG;
		op1.arg = "0xffff";
		asmCode.push_back(op1);
		Operation op2;
		op2.name = ADD;
		op2.flArg = SHORT_ARG;
		op2.arg = "1";
		asmCode.push_back(op2);
		return;
	}

	


	if(op.name == ADDRG) {
		op.name = CNST;
	}

	asmCode.push_back(op);



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

int main() {

	std::vector<Operation> asmCode;

	while(1) {
		std::string line;

		std::getline(std::cin, line);
		if(line == "")
			break;
		int i = 0;
		if((i = parseDirective(line.c_str())) != -1) {
			printf("Directive %d\n", i);
			Operation newOp;
			newOp.flInstr = 0;	
			newOp.name = i;
			newOp.arg = getArg(line);
			asmCode.push_back(newOp);


		} else if((i = parseOp(line.c_str())) != -1) {
			int type = parseOpType(line.c_str(), i);
			int size = parseOpSize(line.c_str(), i);
			printf("Opcode: %d type %d size %d\n", i, type, size);

			Operation newOp;
			newOp.flInstr = 1;
			newOp.type = type;
			newOp.size = size;
			newOp.name = i;
			newOp.arg = getArg(line);
			addOp(asmCode, newOp);

		} else {
			printf("Unknown text [%s]!\n", line.c_str());
		}
	}

	for(int i = 0; i<asmCode.size(); i++) {
		printf("%s i%d s%d t%d arg%d %s\n", opnames[asmCode[i].name], asmCode[i].flIndir, asmCode[i].size, asmCode[i].type, asmCode[i].flArg, asmCode[i].arg.c_str() );
	}

	printf("\n---code===\n");

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
			printf("%s%s", ((asmCode[i].flIndir)?"i":""), tmp);
			int argType = asmCode[i].flArg;
			if(argType == NO_ARG) {
				printf("\n");
			} else if(argType == SHORT_ARG) {
				printf("_b %s\n", asmCode[i].arg.c_str());
			} else if(argType == LONG_ARG) {
				printf("_w %s\n", asmCode[i].arg.c_str());
			} else if(argType == LINK_TIME_ARG) {
				printf("_w link %s\n", asmCode[i].arg.c_str());
			} 
		}
	}

	return 0;
}

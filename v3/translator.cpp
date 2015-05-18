#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <string.h>

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
		"DISCARD"

};

typedef struct Operation_t {
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
		DISCARD
	};

	opname name;
	std::string argument;


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
int parseDirective(const char * str) {
	for(int i = 0; i<dirnum; i++) {
		if(strlen(str) >= strlen(directives[i])) {
			if(!memcmp(directives[i], str, strlen(directives[i])))
				return i;
		}
	}
	return -1;
}

int main() {

	std::vector<Operation> program;

	while(1) {
		std::string line;

		std::getline(std::cin, line);
		if(line == "")
			break;
		int i = 0;
		if((i = parseDirective(line.c_str())) != -1) {
			printf("Directive %d\n", i);
		} else if((i = parseOp(line.c_str())) != -1) {
			int type = parseOpType(line.c_str(), i);
			printf("Opcode: %d type %d\n", i, type);
		} else {
			printf("Unknown text [%s]!\n", line.c_str());
		}
	}

	return 0;
}
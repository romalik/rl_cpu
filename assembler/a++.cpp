#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <stack>

enum TOKEN_TYPE {
    UNKNOWN = 0,
    NUMERIC,
    OPERATOR,
    VARIABLE,
    FUNCTION
};

struct Token {
    TOKEN_TYPE type;
    std::string content;
    int precedence;
    int nArg;
};

struct localVariableEntry {
    std::string name;
    int offset;
};

/*
struct globalSymbolEntry {
    std::string name;

};
*/



int parseExpression(char * inStr, int cOffset) {
//    char * inStr = "$dest_var = $var1 + 3 - 4";

    std::vector<std::string> tokens;

    char *s = inStr;
    while(1) {
        char token[1000];
        char *t = token;
        while(isspace(*s)) s++;

        if(!*s)
            break;

        while(!isspace(*s) && *s) {
            *t = *s;
            t++; s++;
            *t = 0;
        }
        tokens.push_back(std::string(token));
    }

    for(int i = 0; i<tokens.size(); i++) {
        //printf("token %d: %s\n", i, tokens[i].c_str());
    }


    std::vector<Token> output;
    std::stack<Token> opStack;

    for(int i = 0; i<tokens.size(); i++) {

        //printf("Top on opStack %s\n", opStack.empty()?"EMPTY":opStack.top().content.c_str());
        //printf("Output: ");
        if(output.size()) {
            for(int j = 0; j<output.size(); j++) {
                //printf("%s ", output[j].content.c_str());
            }
        }
        //printf("\n");


        Token cToken;
        cToken.content = tokens[i];
        //printf("token: %s\n", cToken.content.c_str());
        if(tokens[i].c_str()[0] == '$') {
            cToken.type = VARIABLE;
            //printf("type: Variable\n");
            output.push_back(cToken);
        } else if(tokens[i].c_str()[0] == ',') {
            //printf("type: comma\n");
            while(opStack.top().type != FUNCTION) {
                if(!opStack.size()) {
                    //printf("function comma/parenthesis mismatch!\n");
                    exit(0);
                }
                output.push_back(opStack.top());
                opStack.pop();
            }
            opStack.top().nArg++;



        } else if(tokens[i].c_str()[0] == '(') {
            //printf("type: Left parenthesis\n");

            cToken.type = UNKNOWN;
            if(opStack.top().type != FUNCTION)
                opStack.push(cToken);
        } else if(tokens[i].c_str()[0] == ')') {
            //printf("type: right parenthesis\n");

            if(!opStack.size()) {
                //printf("parenthesis mismatch!\n");
                exit(0);
            }
            int zeroArgsInFunction = 1;
            while(1) {
                if(!opStack.size()) {
                    //printf("parenthesis mismatch!\n");
                    exit(0);
                }
                if(opStack.top().content == "(" || opStack.top().type == FUNCTION) {
                    break;
                }
                zeroArgsInFunction = 0;
                //printf("push %d %d %s\n", output.size(), opStack.size(), opStack.top().content.c_str());
                output.push_back(opStack.top());
                opStack.pop();
                //printf("wat\n");
            }
            if(opStack.top().type != FUNCTION)
                opStack.pop(); //remove parenthesis

            //printf("try check function on top of stack\n");
            if(opStack.size()) {
                if(opStack.top().type == FUNCTION) {
                    if(zeroArgsInFunction && opStack.top().nArg == 0) {
                        opStack.top().nArg = 0; //void arg
                    } else {
                        opStack.top().nArg += 1; //at least one arg
                    }
                    output.push_back(opStack.top());
                    opStack.pop();
                }
            }
        } else if((tokens[i].c_str()[0] >='0') && (tokens[i].c_str()[0] <='9')) {
            cToken.type = NUMERIC;
            //printf("type: Numeric\n");

            output.push_back(cToken);
        } else if(strchr("+-=", tokens[i].c_str()[0])) {
            cToken.type = OPERATOR;
            //printf("type: Operator\n");

            if(tokens[i].c_str()[0] == '+') cToken.precedence = 4;
            if(tokens[i].c_str()[0] == '-') cToken.precedence = 4;
            if(tokens[i].c_str()[0] == '=') cToken.precedence = 13;

            /* precedence check needed */
            if(opStack.size()) {
                while(opStack.top().type == OPERATOR && opStack.top().precedence < cToken.precedence) {
                    //printf("push to Output %s type %d\n", opStack.top().content.c_str(), opStack.top().type);
                    output.push_back(opStack.top());
                    opStack.pop();
                    if(!opStack.size())
                        break;
                }
            }
            opStack.push(cToken);
        } else {
            //printf("type: Function\n");
            cToken.type = FUNCTION;
            cToken.nArg = 0;
            opStack.push(cToken);
        }
    }
    while(opStack.size()) {
        if(opStack.top().content == "(") {
            //printf("parenthesis mismatch!\n");
            exit(0);
        }
        output.push_back(opStack.top());
        opStack.pop();
    }


    //printf("output:\n");
    for(int i = 0; i<output.size(); i++) {
        //printf("[type: %d (nArg: %d)] %s\n", output[i].type, (output[i].type==FUNCTION)?output[i].nArg:-1, output[i].content.c_str());
    }


    printf("\n\t\t<--- expr [cOffset = %d]\n", cOffset);

    for(int i = 0; i<output.size(); i++) {
        if(output[i].type == NUMERIC || output[i].type == VARIABLE) {
            printf("add %s 0 bp-%d\n", output[i].content.c_str(), cOffset);
            cOffset += 1;
        } else if(output[i].type == OPERATOR) {

            /* optimize this shit - no need to fuck around with stack for these operations */

            if(output[i].content == "+") {
                printf("add *bp-%d *bp-%d bp-%d\n", cOffset-1, cOffset-2, cOffset-2);
                cOffset -= 1;
            } else if(output[i].content == "-") {
                printf("sub *bp-%d *bp-%d bp-%d\n", cOffset-1, cOffset-2, cOffset-2);
                cOffset -= 1;
            } else if(output[i].content == "=") {
                printf("add *bp-%d 0 bp-%d\n", cOffset-1, cOffset-2, cOffset-2);
                cOffset -= 1;
            }
        } else if(output[i].type == FUNCTION) {
            printf("sub **SP %d *SP\n", cOffset);
            printf("lpc 0 0 *SP\t#return address\n");
            printf("add **SP hzhz *SP\t#return address\n");
            printf("add *SP 1 SP\n");
            printf("add bp 0 *SP\tsave base pointer\n");
            printf("add *SP 1 SP\n");
            printf("jmp 0 0 %s\n", output[i].content.c_str());


        }
    }



    return 0;
}

char * parseScope(char * scope, std::vector<localVariableEntry> locals, int cOffset) {

    printf("Upper-level variables: \n");
    for(int i = 0; i<locals.size(); i++) {
        printf("%s ---> %d\n", locals[i].name.c_str(), locals[i].offset);
    }
    char * s = scope;
    while(1) {
        char expr[10000];
        char * e = expr;
        while(isspace(*s) && *s) s++;
        char * startS = s;
        while(*s != ';' && *s) { *e = *s; e++; s++; }
        *e = 0;

        printf("EXPR: %s\n", expr);


        /** WARNING! Possible segfault here! strlen(expr) must be >= strlen("local") **/
        if(!memcmp(expr, "local", strlen("local"))) { //will register local variable on stack here
            char *n = expr + strlen("local");
            while(isspace(*n) && *n)
                n++;

            if(!*n) {
                printf("Something terribly wrong with LOCAL declaration!\n");
                exit(1);
            }

            char newName[1024];
            char * nn = newName;
            while(!isspace(*n) && *n && *n != ';') {
                *nn = *n;
                n++; nn++;
            }
            *nn = 0;
            localVariableEntry newLocal;
            newLocal.name = std::string(newName);
            newLocal.offset = cOffset;
            locals.push_back(newLocal);

            printf("Reg LOCAL %s with bp-offset %d\n", newLocal.name.c_str(), newLocal.offset);

            cOffset++;
        } else if (expr[0] == '{') {
            printf("Entering scope\n");
            s = parseScope(startS + 1, locals, cOffset);
        } else {
            parseExpression(expr, cOffset);
        }

        if(!*s)
            return s;
        if(*s == '}') {
            s++;
            return s;
        }
        s++;
    }
}

int main() {
    char * inStr = " local blahblah ; local blahblah2 ;  { &inner1 = $inner2 + $inner3 ; } $a = $b + $c + func1 ( $d ) ;  $a2 = $b2 - $c2 - func2 ( $d2 ) ;";

    std::vector<localVariableEntry> locals;

    parseScope(inStr, locals, 0);

    return 0;
}

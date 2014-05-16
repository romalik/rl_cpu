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
};

int main() {
    char * inStr = "$dest_var = func1 ( 3 + ( $var1 + 2 ) , $var2 ) + func2 ( $var3 + 4 , 12 , 13 )";

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
        printf("token %d: %s\n", i, tokens[i].c_str());
    }


    std::vector<Token> output;
    std::stack<Token> opStack;

    for(int i = 0; i<tokens.size(); i++) {

        printf("Top on opStack %s\n", opStack.empty()?"EMPTY":opStack.top().content.c_str());
        printf("Output: ");
        if(output.size()) {
            for(int j = 0; j<output.size(); j++) {
                printf("%s ", output[j].content.c_str());
            }
        }
        printf("\n");


        Token cToken;
        cToken.content = tokens[i];
        printf("token: %s\n", cToken.content.c_str());
        if(tokens[i].c_str()[0] == '$') {
            cToken.type = VARIABLE;
            printf("type: Variable\n");
            output.push_back(cToken);
        } else if(tokens[i].c_str()[0] == ',') {
            printf("type: comma\n");
            while(opStack.top().content != "(") {
                if(!opStack.size()) {
                    printf("parenthesis mismatch!\n");
                    exit(0);
                }
                output.push_back(opStack.top());
                opStack.pop();
            }

        } else if(tokens[i].c_str()[0] == '(') {
            printf("type: Left parenthesis\n");

            cToken.type = UNKNOWN;
            opStack.push(cToken);
        } else if(tokens[i].c_str()[0] == ')') {
            printf("type: right parenthesis\n");

            if(!opStack.size()) {
                printf("parenthesis mismatch!\n");
                exit(0);
            }
            while(1) {
                if(!opStack.size()) {
                    printf("parenthesis mismatch!\n");
                    exit(0);
                }
                if(opStack.top().content == "(") {
                    break;
                }
                printf("push %d %d %s\n", output.size(), opStack.size(), opStack.top().content.c_str());
                output.push_back(opStack.top());
                opStack.pop();
                printf("wat\n");
            }

            opStack.pop(); //remove parenthesis

            printf("try check function on top of stack\n");
            if(opStack.size()) {
                if(opStack.top().type == FUNCTION) {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
            }
        } else if((tokens[i].c_str()[0] >='0') && (tokens[i].c_str()[0] <='9')) {
            cToken.type = NUMERIC;
            printf("type: Numeric\n");

            output.push_back(cToken);
        } else if(strchr("+-=", tokens[i].c_str()[0])) {
            cToken.type = OPERATOR;
            printf("type: Operator\n");

            if(tokens[i].c_str()[0] == '+') cToken.precedence = 4;
            if(tokens[i].c_str()[0] == '-') cToken.precedence = 4;
            if(tokens[i].c_str()[0] == '=') cToken.precedence = 13;

            /* precedence check needed */
            if(opStack.size()) {
                while(opStack.top().type == OPERATOR && opStack.top().precedence < cToken.precedence) {
                    printf("push to Output %s type %d\n", opStack.top().content.c_str(), opStack.top().type);
                    output.push_back(opStack.top());
                    opStack.pop();
                    if(!opStack.size())
                        break;
                }
            }
            opStack.push(cToken);
        } else {
            printf("type: Function\n");
            cToken.type = FUNCTION;
            opStack.push(cToken);
        }
    }
    while(opStack.size()) {
        if(opStack.top().content == "(") {
            printf("parenthesis mismatch!\n");
            exit(0);
        }
        output.push_back(opStack.top());
        opStack.pop();
    }


    printf("output:\n");
    for(int i = 0; i<output.size(); i++) {
        printf("[type: %d] %s\n", output[i].type, output[i].content.c_str());
    }
    return 0;
}

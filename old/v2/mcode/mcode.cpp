#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <map>
#include <math.h>
#include <stdint.h>

class MCompiler {
    std::ifstream file;
	//width of control word
    int outputWidth;
	//width of input counter value
    int seqWidth;
	//width of input command and flags
    int commandWidth;
	//width of one memory chip
    int splitWidth;

	//one condition
    typedef std::pair<int,int> Condition;
	//one-level set of conditions
    typedef std::vector<Condition> ConditionVector;
	//whole tree of conditions
    typedef std::vector<ConditionVector> ConditionCascade;
    ConditionCascade currentConditions;

	//definition vector
    typedef std::map<std::string, std::pair<int,int> > DefVector;
    DefVector definitions;
    enum {
        NO_STATE,
        CONF_STATE,
        DEFINE_STATE,
        CONDITION_STATE
    };
    int currentState;
public:
	//one microinstruction (contril word)
    typedef std::vector<int> Instruction;
	//default control word
    Instruction defaultCode;
	//set of microinstructions (control words) for one input command and flags
    typedef std::vector<Instruction> Routine;

	//whole code. This vector size is 2 ^ (width of input command and flags).
    std::vector<Routine> code;
    MCompiler(std::string path) {
        file.open(path.c_str());
		//set defaults
        outputWidth = -1;
        seqWidth = -1;
        commandWidth = -1;
        splitWidth = 0;
        currentState = NO_STATE;
    }

    ConditionVector parseCondition(std::vector<std::string> words) {
        ConditionVector res;
        for(int i = 1; i<words.size(); i+=2) {
            if(words[i].empty())
                break;
            DefVector::iterator bitPos = definitions.find(words[i]);
            if(bitPos == definitions.end()) {
                printf("definition %s not found!\n", words[i].c_str());
                break;
            }
            res.push_back(Condition(bitPos->second.first, atoi(words[i+1].c_str())));
        }
        return res;
    }


	//check if cmd fits current conditions tree
    bool checkCondition(int cmd) {
        bool res = true;
        for(int i = 0; i<currentConditions.size(); i++) {
            for(int j = 0; j<currentConditions[i].size(); j++) {
                Condition c = currentConditions[i][j];
                bool bit = cmd & (1 << c.first);
                res = res && (bit == c.second);
            }
        }
        return res;
    }

    void compile() {
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream lineStream(line);
            std::vector<std::string> wordsVector;

            while (lineStream)
            {
                std::string tmp;
                lineStream >> tmp;
                wordsVector.push_back(tmp);
            }

            if(wordsVector[0][0] == '#') {
                //comment
            } else if(wordsVector[0] == "conf") {
                printf("Config section\n");
                currentState = CONF_STATE;
            } else if(wordsVector[0] == "define") {
                printf("Define section\n");
                currentState = DEFINE_STATE;
            } else if(wordsVector[0] == "section") {
                printf("Condition section\n");
                currentState = CONDITION_STATE;
                ConditionVector condVector = parseCondition(wordsVector);
                currentConditions.push_back(condVector);
                printf("Condition:\n");
                for(int i = 0; i<condVector.size(); i++) {
                    printf("\t%d == %d\n", condVector[i].first, condVector[i].second);
                }
            } else if(wordsVector[0] == "end") {
                printf("End current section\n");
                if(!currentConditions.empty())
                    currentConditions.pop_back();

                if(currentState == CONF_STATE) {
                    printf("Config:\tout: %d\n\tcount: %d\n\tcommand: %d\n\n", outputWidth, seqWidth, commandWidth);
                }

                if(currentState == DEFINE_STATE) {
                    defaultCode.resize(outputWidth, 0);
                    printf("Definitions:\n");
                    for(DefVector::const_iterator it = definitions.begin(); it != definitions.end(); ++it) {
                        int pin = it->second.first;
                        int val = it->second.second;
                        printf("\t%s -> %d default %d\n", it->first.c_str(), pin, val);
                        if(val >= 0) {
                            if(pin < outputWidth) {
                                defaultCode[pin] = val;
                            } else {
                                printf("Error in definitions: out of range for %s (val: %d, max: %d)\n", it->first.c_str(), pin, outputWidth);
                                break;
                            }
                        }

                    }

                    printf("Default code generated:\n->\t");
                    for(int i = 0; i<defaultCode.size(); i++) {
                        printf("%d ", defaultCode[i]);
                    }
                    printf("\n");

                    int routines = pow(2,commandWidth);
                    printf("Generating %d routines\n", routines);
                    code.resize(routines);

                }

                if(currentConditions.empty())
                    currentState = NO_STATE;

            } else if(!line.empty()) {
                if(currentState == CONF_STATE) {
                    if(wordsVector.size() < 2) {
                        printf("Error in config section! [%s]\n", line.c_str());
                        break;
                    }
                    if(wordsVector[0] == "out") {
                        outputWidth = atoi(wordsVector[1].c_str());
                    }
                    if(wordsVector[0] == "count") {
                        seqWidth = atoi(wordsVector[1].c_str());
                    }
                    if(wordsVector[0] == "command") {
                        commandWidth = atoi(wordsVector[1].c_str());
                    }
                    if(wordsVector[0] == "split") {
                        splitWidth = atoi(wordsVector[1].c_str());
                    }
                }


                if(currentState == DEFINE_STATE) {
                    if(wordsVector.size() < 3) {
                        printf("Error in define section! [%s]\n", line.c_str());
                        break;
                    }
                    definitions[wordsVector[0]].first = atoi(wordsVector[1].c_str());
                    definitions[wordsVector[0]].second = atoi(wordsVector[2].c_str());
                }

                if(currentState == CONDITION_STATE) {
                    Instruction inst = defaultCode;
                    for(int i = 0; i<wordsVector.size(); i+=2) {
                        if(wordsVector[i].empty())
                            break;
                        DefVector::iterator bitPos = definitions.find(wordsVector[i]);
                        if(bitPos == definitions.end()) {
                            printf("definition %s not found!\n", wordsVector[i].c_str());
                            break;
                        }
                        inst[bitPos->second.first] = atoi(wordsVector[i+1].c_str());
                    }

                    printf("Instruction generated [ %s ]:\n -> ", line.c_str());
                    for(int i = 0; i<inst.size(); i++) {
                        printf("%d ", inst[i]);
                    }
                    printf("\n");
                    int cnt = 0;
                    for(int i = 0; i<code.size(); i++) {
                        if(checkCondition(i)) {
                            code[i].push_back(inst);
                            cnt++;
                        }
                    }
                    printf("Instruction added to %d routines\n", cnt);

                }

            }
        }

        int maxLen = 0;
        for(int i = 0; i<code.size(); i++) {
            if(code[i].size() > maxLen)
                maxLen = code[i].size();
        }

        printf("Longest sequence: %d\n", maxLen);

        //now fill all routines to max length with default values
        for(int i = 0; i<code.size(); i++) {
            while(code[i].size() < pow(2, seqWidth)) {
                code[i].push_back(defaultCode);
            }
        }


    }

    void write(char * fname) {
        std::vector<std::ofstream *> outputFileVector;
        std::ofstream outputFile;

        outputFile.open(fname);

        if(splitWidth > 0 && outputWidth > splitWidth) {

            for(int i = 0; i*splitWidth<outputWidth; i++) {
                char buf[1000];
                sprintf(buf, "%s.%d", fname, i*splitWidth);
                outputFileVector.push_back(new std::ofstream());
                outputFileVector[i]->open(buf);
            }
        }

        for(int i = 0; i<code.size(); i++) {
            for(int j = 0; j<code[i].size(); j++) {
                Instruction inst = code[i][j];
                uint64_t a = 0;
                for(int i = 0; i<inst.size(); i++) {
                    outputFile << inst[i] << " ";
                    a |= (inst[i] << i);
                }

                outputFile << std::endl;
                //outputFile << std::hex << a << std::endl;
                for(int f = 0; f<outputFileVector.size(); f++) {
                    uint64_t mask = (1 << (f+1)*splitWidth) - 1;
                    uint64_t toWrite = (a >> f*splitWidth) & mask;
                    (*outputFileVector[f]) << std::hex << toWrite << std::endl;
                }
            }
        }
        outputFile.close();
        for(int i = 0; i<outputFileVector.size(); i++) {
            outputFileVector[i]->close();
        }

    }


};

int main(int argc, char ** argv) {
    if(argc < 2)
        return -1;

    MCompiler compiler(argv[1]);
    compiler.compile();
    if(argc == 3) {
        compiler.write(argv[2]);
    }
    return 0;
}

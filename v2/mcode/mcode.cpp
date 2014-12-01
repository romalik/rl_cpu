#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <map>
#include <math.h>

class MCompiler {
    std::ifstream file;
    int outputWidth;
    int seqWidth;
    int commandWidth;
    int splitWidth;
    typedef std::pair<int,int> Condition;
    typedef std::vector<Condition> ConditionVector;
    typedef std::vector<ConditionVector> ConditionCascade;
    ConditionCascade currentConditions;

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
    typedef std::vector<int> Instruction;
    Instruction defaultCode;
    typedef std::vector<Instruction> Routine;
    std::vector<Routine> code;
    MCompiler(std::string path) {
        file.open(path.c_str());
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

        //now fill all routines to max length with default values
        for(int i = 0; i<code.size(); i++) {
            while(code[i].size() < pow(2, seqWidth)) {
                code[i].push_back(defaultCode);
            }
        }


    }

    void writeBin(char * fname) {
        std::ofstream outputFile;



        outputFile.open(fname);

        for(int i = 0; i<code.size(); i++) {
            for(int j = 0; j<code[i].size(); j++) {
                Instruction inst = code[i][j];
                for(int i = 0; i<inst.size(); i++) {
                    outputFile << inst[i] << " ";
                }
                outputFile << std::endl;
            }
        }
        outputFile.close();
    }

};

int main(int argc, char ** argv) {
    if(argc < 2)
        return -1;

    MCompiler compiler(argv[1]);
    compiler.compile();
    if(argc == 3) {
        compiler.writeBin(argv[2]);
    }
    return 0;
}

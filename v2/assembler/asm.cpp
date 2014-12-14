#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>

class Assembler {
    std::ifstream file;
    std::vector<std::string> text;
    std::vector<std::vector<std::string> > formText;
    std::vector<std::vector<std::string> > codeText;

    int textSegStart;
    int dataSegStart;



public:

    Assembler(std::string path) {
        textSegStart = 0;
        dataSegStart = 0;

        file.open(path.c_str());
        std::string line;
        while(std::getline(file, line)) {
            text.push_pack(line);
        }
    }

    void removeJunkLines() {
        for(int i = 0; i<text.size(); i++) {
            std::stringstream lineStream(code[i]);
            std::vector<std::string> wordsVector;
            std::vector<std::string> cLine;

            while (lineStream)
            {
                std::string tmp;
                lineStream >> tmp;
                wordsVector.push_back(tmp);
            }

            for(int j = 0; j<wordsVector; j++) {
                if(wordsVector[j][0] == '#') {
                    break;
                } else if(wordsVector[j] != "") {
                    cLine.push_back(wordsVector[j]);
                }
            }

            if(!cLine.empty()) {
                formText.push_back(cLine);
            }
        }
    }

    void firstPass() {

        int cTextSegAddr = 0;
        int cDataSegAddr = 0;

        std::string mode = "text";

        for(int i = 0; i<formText.size(); i++) {
            if(formText[i][0] == ".set") {
                if(formText[i][1] == "data") {
                    dataSegStart = atoi(formText[i][2]);
                }
                if(formText[i][1] == "text") {
                    textSegStart = atoi(formText[i][2]);
                }
            }

            if(formText[i][0] == ".text") {
                mode = "text";
            }
            if(formText[i][0] == ".data") {
                mode = "data";
            }

            if(formText[i][0][ formText[i][0].size()-1 ] == ':') {
                addLabel(formText[i][0], )
            }
        }
    }

    void write(char * fname) {
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

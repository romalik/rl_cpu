#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stdlib.h>


#define DEFAULT_TEXT_START 0x00
#define DEFAULT_DATA_START 0x4000

class Assembler {
public:
    std::ifstream file;
    std::vector<std::string> text;
    std::vector<std::vector<std::string> > formText;
    std::vector<std::vector<std::string> > codeText;
    std::vector<std::vector<std::string> > dataText;
    std::vector<std::vector<std::string> > roDataText;

    std::map<std::string, int> textLabels;
    std::map<std::string, int> dataLabels;
    std::map<std::string, int> roDataLabels;


    std::vector<int> dataImage;
    std::vector<int> textImage;
    std::vector<int> roDataImage;

    int textSegStart;
    int dataSegStart;
    int roDataSegStart;

    std::map<std::string, int> baseOpcodes;


public:

    Assembler(std::string path) {
        textSegStart = -1;
        dataSegStart = -1;
        roDataSegStart = -1;
        fillOpcodes();
        file.open(path.c_str());
        std::string line;
        while(std::getline(file, line)) {
            text.push_back(line);
        }

    }

    /*
000 +		0
001 -		>
010 and		==
011 or		>=
100 xor		<
101 shift	!=
110         <=
111         1
    */
    void fillOpcodes() {
        baseOpcodes["add"] = 0x00;
        baseOpcodes["sub"] = 0x01;
        baseOpcodes["and"] = 0x02;
        baseOpcodes["or"] = 0x03;
        baseOpcodes["xor"] = 0x04;
        baseOpcodes["shift"] = 0x05;

        baseOpcodes["jg"] = 0x09;
        baseOpcodes["je"] = 0x0a;
        baseOpcodes["jge"] = 0x0b;
        baseOpcodes["jl"] = 0x0c;
        baseOpcodes["jne"] = 0x0d;
        baseOpcodes["jle"] = 0x0e;
        baseOpcodes["jmp"] = 0x0f;
        baseOpcodes["push"] = 0x1080;
        baseOpcodes["pop"] = 0x1100;
        baseOpcodes["call"] = 0x1180;
        baseOpcodes["ret"] = 0x1200;



    }


    void removeJunkLines() {
        for(int i = 0; i<text.size(); i++) {
            std::stringstream lineStream(text[i]);
            std::vector<std::string> wordsVector;
            std::vector<std::string> cLine;

            while (lineStream)
            {
                std::string tmp;
                lineStream >> tmp;
                wordsVector.push_back(tmp);
            }

            for(int j = 0; j<wordsVector.size(); j++) {
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


        std::string mode = "text";

        for(int i = 0; i<formText.size(); i++) {
            if(formText[i][0] == ".set") {
                if(formText[i][1] == "data") {
                    dataSegStart = strtol(formText[i][2].c_str(), 0, 0);
                }
                if(formText[i][1] == "text") {
                    textSegStart = strtol(formText[i][2].c_str(), 0, 0);
                }
                if(formText[i][1] == "rodata") {
                    roDataSegStart = strtol(formText[i][2].c_str(), 0, 0);
                }
            } else if(formText[i][0] == ".text") {
                mode = "text";
            } else if(formText[i][0] == ".data") {
                mode = "data";
            } else if(formText[i][0] == ".rodata") {
                mode = "rodata";
            } else {

                if(formText[i][0][ formText[i][0].size()-1 ] == ':') {
                    if(mode == "text") {
                        addLabel(formText[i][0], codeText.size()*4, textLabels);
                        formText[i].erase(formText[i].begin());
                    }
                }
                if(!formText[i].empty()) {
                    if(!formText[i][0].empty()) {
                        if(mode == "text") {
                            codeText.push_back(formText[i]);
                        } else if(mode == "data") {
                            dataText.push_back(formText[i]);
                        } else if(mode == "rodata"){
                            roDataText.push_back(formText[i]);
                        }
                    }
                }
            }
        }
    }

    void genDataImage() {
        int currentDataAddr = 0;
        for(int i = 0; i<dataText.size(); i++) {
            if(dataText[i][0][ dataText[i][0].size() - 1 ] == ':') {
                addLabel(dataText[i][0], currentDataAddr, dataLabels);
                dataText[i].erase(dataText[i].begin());
            }

            if(!dataText[i].empty()) {
                if(!dataText[i][0].empty()) {
                    if(dataText[i][0][0] == '.') {
                        int defval = 0;
                        if(dataText[i].size() > 1) {
                            if(!dataText[i][1].empty()) {
                                defval = atoi(dataText[i][1].c_str());
                            }
                        }
                        for(int j = 0; j < atoi(dataText[i][0].c_str()+1); j++) {
                            dataImage.push_back(defval);
                            currentDataAddr++;
                        }
                    }
                }
            }
        }
    }

    void genRoDataImage() {
        int currentRoDataAddr = 0;
        for(int i = 0; i<roDataText.size(); i++) {
            if(roDataText[i][0][ roDataText[i][0].size() - 1 ] == ':') {
                addLabel(roDataText[i][0], currentRoDataAddr, roDataLabels);
                roDataText[i].erase(roDataText[i].begin());
            }

            if(!roDataText[i].empty()) {
                if(!roDataText[i][0].empty()) {
                    if(roDataText[i][0][0] == '.') {
                        int defval = 0;
                        if(roDataText[i].size() > 1) {
                            if(!roDataText[i][1].empty()) {
                                defval = atoi(roDataText[i][1].c_str());
                            }
                        }
                        for(int j = 0; j < atoi(roDataText[i][0].c_str()+1); j++) {
                            roDataImage.push_back(defval);
                            currentRoDataAddr++;
                        }
                    }
                }
            }
        }
    }

/*
special:
op0:
A not modified
pc->A
sp->A

op2:
ALU->sp
*/
    int getDerefAndValue(std::string str, int & val) {
        int deref = 0;
        while(str[0] == '*') {
            str.erase(str.begin());
            deref++;
        }
        if(deref > 4) {
            printf("Too many dereferences!\n");
            exit(1);
        }
        if(str == "PC") {
            val = 0;
            deref = 0x5; //101b
            return deref;
        }
        if(str == "SP") {
            val = 0;
            deref = 0x6; //110b
            return deref;
        }
        if(str.substr(0,3) == "BP+") {
            int offset = strtol(str.substr(3).c_str(),0,0);
            if(offset > 0x2000) {
                printf("Too big offset for BP\n");

                exit(1);
            }
            val = 0x2000 + offset;
            deref++;
            if(deref > 4) {
                printf("Too many dereferences for BP!\n");
                exit(1);
            }
        } else if(isdigit(str[0])) {
            val = strtol(str.c_str(), 0, 0);
            printf("numeric %s -> %d\n", str.c_str(), val);
        } else {
            std::map<std::string, int>::iterator it = textLabels.find(str);
            if(it == textLabels.end()) {
                it = dataLabels.find(str);
            }
            if(it == dataLabels.end()) {
                it = roDataLabels.find(str);
            }

            if(it == textLabels.end() || it == dataLabels.end() || it == roDataLabels.end()) {
                printf("Label %s not found!\n", str.c_str());
                exit(1);
            }
            val = it->second;
            printf("label \"%s\" -> %d\n", str.c_str(), val);
        }
        return deref;
    }

    std::vector<int> genInstruction(std::vector<std::string> line) {
        std::vector<int> instr(4);
        std::map<std::string, int>::iterator it = baseOpcodes.find(line[0]);
        if(it == baseOpcodes.end()) {
            printf("Unknown opcode %s!\n", line[0].c_str());
            exit(1);
        }

        instr[0] = baseOpcodes[line[0]];
        int deref1 = getDerefAndValue(line[1], instr[1]);
        int deref2 = getDerefAndValue(line[2], instr[2]);
        int deref3 = getDerefAndValue(line[3], instr[3]);

        instr[0] |= ((deref1 & 0x7) << 4);
        instr[0] |= ((deref2 & 0x3) << 7);
        instr[0] |= ((deref3 & 0x7) << 9);

        printf("GEN %d %d %d %d\n", instr[0], instr[1], instr[2], instr[3]);

        return instr;
    }


    void genTextImage() {
        for(int i = 0; i<codeText.size(); i++) {
            std::vector<int> instr = genInstruction(codeText[i]);
            textImage.insert(textImage.end(), instr.begin(), instr.end());
        }
    }

    void addLabel(std::string name, int addr, std::map<std::string, int> & target) {
        while(name[name.size() - 1] == ':') {
            name.erase(name.begin() + name.size() - 1);
        }
        target[name] = addr;
    }

    void write(char * fname) {
    }

    void applyOffsets() {
        if(textSegStart == -1)
            textSegStart = DEFAULT_TEXT_START;

        if(dataSegStart == -1)
            dataSegStart = DEFAULT_DATA_START;

        if(roDataSegStart == -1) {
            roDataSegStart = textSegStart + codeText.size()*4;
        }

        std::map<std::string, int>::iterator it;

        for(it = textLabels.begin(); it != textLabels.end(); it++) {
            it->second += textSegStart;
        }
        for(it = dataLabels.begin(); it != dataLabels.end(); it++) {
            it->second += dataSegStart;
        }
        for(it = roDataLabels.begin(); it != roDataLabels.end(); it++) {
            it->second += roDataSegStart;
        }
    }

};

int main(int argc, char ** argv) {
    if(argc < 2)
        return -1;

    Assembler assembler(argv[1]);
    assembler.removeJunkLines();
    assembler.firstPass();
    assembler.genDataImage();
    assembler.genRoDataImage();
    assembler.applyOffsets();
    //here we have all link info, apply offsets now

    assembler.genTextImage();

    printf("Text:\n");
    for(int i = 0; i<assembler.codeText.size(); i++) {
        printf(">\t");
        for(int j = 0; j<assembler.codeText[i].size(); j++) {
            printf("%s ", assembler.codeText[i][j].c_str());
        }
        printf("\n");
    }

    printf("Data:\n");
    for(int i = 0; i<assembler.dataText.size(); i++) {
        printf(">\t");
        for(int j = 0; j<assembler.dataText[i].size(); j++) {
            printf("%s ", assembler.dataText[i][j].c_str());
        }
        printf("\n");
    }

    printf("Labels:\n");
    for(std::map<std::string, int>::iterator it = assembler.textLabels.begin(); it != assembler.textLabels.end(); it++) {
        printf(">\t%s: 0x%04X\n", it->first.c_str(), it->second);
    }

    printf("Data Labels:\n");
    for(std::map<std::string, int>::iterator it = assembler.dataLabels.begin(); it != assembler.dataLabels.end(); it++) {
        printf(">\t%s: 0x%04X\n", it->first.c_str(), it->second);
    }

    printf("Text image:\n");
    for(int i = 0; i<assembler.textImage.size(); i++) {
        printf("0x%04X ", assembler.textImage[i]);
        if(i%4 == 3) printf("\n");
    }


    printf("Data image:\n");
    for(int i = 0; i<assembler.dataImage.size(); i++) {
        printf("0x%04X ", assembler.dataImage[i]);
    }
    printf("\n");

    std::ofstream romFile;
    romFile.open("rom");
    romFile << "v2.0 raw" << std::endl;
    romFile << std::hex;
    for(int i = 0; i<assembler.textImage.size(); i++) {
        romFile << assembler.textImage[i] << std::endl;
    }
    for(int i = 0; i<assembler.roDataImage.size(); i++) {
        romFile << assembler.roDataImage[i] << std::endl;
    }
    romFile.close();
    return 0;
}

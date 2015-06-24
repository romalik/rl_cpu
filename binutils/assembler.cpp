#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "oplist.h"
#include <string>
#include <string.h>
#include <map>
#include <sstream>


typedef struct LabelEntry_t {
    LabelEntry_t() {uid = -1; position = -1; needExport = 0; needImport = 0; section = 0;}
    int section;
    int uid;
    int position;
    int needExport;
    int needImport;
} LabelEntry;



typedef struct Section_t {
    char labelMap[0xffff];

    std::vector<uint16_t> code;
} Section;

class Assembly {
public:
    Assembly() {
        cSection = 0;
        labelUid = 0;
        memset(sections[0].labelMap, 0, 0xffff);
        memset(sections[1].labelMap, 0, 0xffff);
    }
    //code & data
    Section sections[2];
    int cSection;
    int labelUid;

    int generateLabelUid() {
        int res = labelUid;
        labelUid++;
        return res;
    }

    std::map<std::string, LabelEntry> labels;



    void regLabel(std::string & name) {
        if(labels[name].uid == -1) {
            //new Label
            labels[name].uid = generateLabelUid();
        }
        labels[name].position = sections[cSection].code.size();
        labels[name].section = cSection;
    }

    int getLabelUid(std::string & name) {
        if(labels[name].uid == -1) {
            //new Label
            labels[name].uid = generateLabelUid();
        }
        return labels[name].uid;
    }

    int markLabelPosition() {
        sections[cSection].labelMap[sections[cSection].code.size()] = 1;
    }

    void importLabel(std::string & label) {
        labels[label].needImport = 1;
    }

    void exportLabel(std::string & label) {
        labels[label].needExport = 1;
    }

    void output(int val) {
        sections[cSection].code.push_back(val);
    }


    void convert16to8(std::vector<uint16_t> & code, char * dst) {
        char * p = dst;
        for(int i = 0; i<code.size(); i++) {
            *p = ((code[i] & 0xff00) >> 8);
            p++;
            *p = (code[i] & 0xff);;
            p++;
        }

    }

    int convertLabelsTo8(char * dst) {
        char *p = dst;
        for(std::map<std::string, LabelEntry>::iterator it = labels.begin(); it!= labels.end(); it++) {
            std::string labelName = it->first;
            LabelEntry entry = it->second;

            if(entry.position < 0)
                entry.needImport = 1;


            int skip = sprintf(p, "%s", labelName.c_str());
            p+=skip;
            p++; // skip null-terminator
            uint16_t uid = (uint16_t)entry.uid;
            char uidH = ((uid & 0xff00) >> 8);
            char uidL = (uid & 0xff);
            uint16_t pos = (uint16_t)entry.position;
            char posH = ((pos & 0xff00) >> 8);
            char posL = (pos & 0xff);
            char sect = (char)entry.section;
            char im = (char)entry.needImport;
            char ex = (char)entry.needExport;
            *p = uidH;
            p++;
            *p = uidL;
            p++;

            *p = posH;
            p++;
            *p = posL;
            p++;

            *p = sect;
            p++;
            *p = im;
            p++;
            *p = ex;
            p++;
        }
        return p - dst;

    }

    void writeToFile(std::string filename) {
        std::ofstream file;
        file.open(filename.c_str(), std::ios::out | std::ios::binary);
        char header[] = "ROBJ";
        char textSizeHigh = ((sections[0].code.size() & 0xff00) >> 8);
        char textSizeLow = (sections[0].code.size() & 0xff);
        char dataSizeHigh = ((sections[1].code.size() & 0xff00) >> 8);
        char dataSizeLow = (sections[1].code.size() & 0xff);
        char textFull[0xffff * 2];
        char dataFull[0xffff * 2];
        char labelFull[0xffff];
        memset(textFull, 0, 0xffff*2);
        memset(dataFull, 0, 0xffff*2);
        memset(labelFull, 0, 0xffff);
        convert16to8(sections[0].code, textFull);
        convert16to8(sections[1].code, dataFull);
        int labelFullLen = convertLabelsTo8(labelFull);
        char labelFullLenH = ((labelFullLen & 0xff00) >> 8);
        char labelFullLenL = (labelFullLen & 0xff);
        file.write(header, 4);
        file.write(&textSizeHigh, 1);
        file.write(&textSizeLow, 1);
        file.write(&dataSizeHigh, 1);
        file.write(&dataSizeLow, 1);
        file.write(&labelFullLenH, 1);
        file.write(&labelFullLenL, 1);
        file.write(labelFull, labelFullLen);
        file.write(sections[0].labelMap, 0xffff);
        file.write(textFull, 0xffff*2);
        file.write(sections[1].labelMap, 0xffff);
        file.write(dataFull, 0xffff*2);
        file.close();


        int textSize = ((textSizeHigh << 8) | textSizeLow);
        int dataSize = ((dataSizeHigh << 8) | dataSizeLow);
        printf("%s:\n  textSize: %d words\n  dataSize: %d words\n  labelLength: %d\n", filename.c_str(), textSize, dataSize, labelFullLen);


    }

};

int getOpIndex(std::string & op) {
    int cIdx = 0;
    while(1) {
        if(std::string(oplist[cIdx]) == "") {
            return -1;
        }
        if(std::string(oplist[cIdx]) == op) {
            return cIdx;
        }
        cIdx++;
    }
}

int evaluateExpr(const char *s) {
    int cSub = 0; // 0 for +, 1 for -
    int shRes = 0;
    int cOp = 0;
    const char * p = s;
    while(1) {

        char * next;
        cOp = strtol(p, &next, 0);
        p = next;

        if(cSub == 0) { //current +
            shRes = shRes + cOp;
        } else {
            shRes = shRes - cOp;
        }
        if(!*p) {
            break;
        } else if(*p == '-') {
            cSub = 1;
            cOp = 0;
        } else if(*p == '+') {
            cSub = 0;
            cOp = 0;
        } else {
            printf("Bad shift for label! %s\n", s);
            exit(1);
        }
        p++;
    }
    return shRes;
}

int main(int argc, char ** argv) {
    std::ifstream file;

    if(argc < 2) {
        printf("No input files\n");
        exit(1);
    }
    file.open(argv[1], std::ios_base::in);

    Assembly assembly;

    std::string line;
    while(std::getline(file, line)) {
        if(file.eof()) break;
        std::stringstream ss(line);
        std::string word;
        ss >> word;
        if(word == "" || word[0] == ';') {
            //empty line or comment
        } else if(word[0] == '.') {
            if(word == ".label") {
                ss >> word;
                assembly.regLabel(word);
            } else if(word == ".import") {
                ss >> word;
                assembly.importLabel(word);
            } else if(word == ".export") {
                ss >> word;
                assembly.exportLabel(word);
            } else if(word == ".byte") {
                int num;
                ss >> num;
                int val;
                ss >> val;
                for(int i = 0; i<num; i++) {
                    assembly.output(val);
                }
            } else if(word == ".skip") {
                int num;
                ss >> num;
                for(int i = 0; i<num; i++) {
                    assembly.output(0);
                }
            } else if(word == ".code") {
                assembly.cSection = 0;
            } else if(word == ".lit") {
                assembly.cSection = 1;
            } else if(word == ".data") {
                assembly.cSection = 1;
            } else if(word == ".bss") {
                assembly.cSection = 1;
            } else if(word == ".endproc") {
                assembly.output(ret);
            }
        } else {
            //printf("Opcode: [%s]\n", word.c_str());
            int opcode = getOpIndex(word);
            if(opcode == -1) {
                printf("Bad operation %s\n", word.c_str());
                exit(1);
            }

            int argType = 0;

            if(!word.compare(word.size()-2,2,"_b")) {
                //short arg
                argType = 1;
            } else if(!word.compare(word.size()-2,2,"_w")) {
                //long arg
                argType = 2;
            }
            //printf("Arg type: %d\n", argType);
            if(argType == 0) {
                assembly.output(opcode);
            } else {
                std::string arg;
                ss >> arg;
                //printf("Arg: [%s]\n", arg.c_str());
                if(isdigit(arg[0]) || arg[0] == '-') {
                    int argNum = evaluateExpr(arg.c_str());
                    if(argType == 1) {
                        opcode = opcode | (argNum << 8);
                        assembly.output(opcode);
                    } else if(argType == 2) {
                        assembly.output(opcode);
                        assembly.output(argNum);
                    }
                } else {
                    //here we have a label
                    if(argType == 1) {
                        printf("Trying to put label as a short argument [%s]!\n", arg.c_str());
                        exit(1);
                    }

                    assembly.output(opcode);
                    int labelUid = assembly.getLabelUid(arg);
                    assembly.markLabelPosition();
                    assembly.output(labelUid);
                }
            }
        }


    }
/*
    printf("Labels:\n");
    for(std::map<std::string, LabelEntry>::iterator it = assembly.labels.begin(); it!= assembly.labels.end(); it++) {
        std::string labelName = it->first;
        LabelEntry entry = it->second;

        printf("%s uid: %d pos: %d import: %d export: %d, section: %d\n", labelName.c_str(), entry.uid, entry.position, entry.needImport, entry.needExport, entry.section);
    }
    printf("\n");
    */

/*
    for(int sect = 0; sect < 2; sect++) {
        printf("Section %d:\n", sect);
        for(int i = 0; i<assembly.sections[sect].code.size(); i++) {
            if(i%4 == 0)
                printf("\n%04X:\t", i);
            int hasLabel = 0;
            hasLabel = assembly.sections[sect].labelMap[i];
            printf("%s%04X%s\t", hasLabel?"_":"", assembly.sections[sect].code[i], hasLabel?"_":"");
        }
        printf("\n\n");
    }
*/

    if(argc > 2) {
        assembly.writeToFile(argv[2]);
    }

}

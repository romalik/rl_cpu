#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <map>
#include <stdlib.h>
#include <stdint.h>

#include "oplist.h"




typedef struct LabelEntry_t {
    LabelEntry_t() {uid = -1; position = -1; needExport = 0; needImport = 0; section = 0; shift = 0;}
    int section;
    int uid;
    int position;
    int needExport;
    int needImport;
    int shift;
} LabelEntry;



typedef struct Section_t {
    char labelMap[0xffff];
    int offset;
    std::vector<uint16_t> code;
    std::string filename;
} Section;

class Linker {
    void convert8to16(char * src, std::vector<uint16_t> & code, int len) {
        char * p = src;
        for(int i = 0; i<len; i++) {
            uint16_t val = 0;
            val |= ((uint8_t)(*p)<<8);
            p++;
            val |= (uint8_t)(*p);
            p++;
            code.push_back(val);
        }
    }

    int convert8ToLabels(char * src, std::map<std::string, LabelEntry> & labels, int len) {
        char *p = src;
        while(p - src < len) {
            std::string labelName;
            LabelEntry entry;

            labelName = std::string(p); //read name to null-terminator
            p += labelName.size() + 1; //skip null-terminator;
            char uidH = *p; p++;
            char uidL = *p; p++;

            char posH = *p; p++;
            char posL = *p; p++;
            char sect = *p; p++;
            char im = *p; p++;
            char ex = *p; p++;

            int labelShift = 0;
            size_t found = std::min(labelName.find("+"),labelName.find("-"));
            if (found!=std::string::npos) {
                std::string shiftStr = std::string(labelName, found);
                const char *p = shiftStr.c_str();
                int cSub = 0; // 0 for +, 1 for -
                int shRes = 0;
                int cOp = 0;
                while(1) {
                    if(isdigit(*p)) {
                        cOp = cOp*10 + (*p-'0');
                    } else {
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
                            printf("Bad shift for label! %s\n", shiftStr.c_str());
                            exit(1);
                        }
                    }
                    p++;
                }

                labelName = std::string(labelName, 0, found);
                printf("Found complex label! Name: [%s] shift: [%s] eval: [%d]\n", labelName.c_str(), shiftStr.c_str(), shRes);
                labelShift = shRes;
            }

            entry.uid = (((uint8_t)uidH << 8) | (uint8_t)uidL);
            entry.position = (((uint8_t)posH << 8) | (uint8_t)posL);
            entry.section = (uint8_t)sect;
            entry.needImport = (uint8_t)im;
            entry.needExport = (uint8_t)ex;
            entry.shift = labelShift;
            labels[labelName] = entry;
        }

    }


public:
    typedef std::map<std::string, LabelEntry> LabelMap;
    std::vector<std::vector<Section> > sections;
    std::vector<LabelMap> labelsPerObj;

    Linker() {
        mode = 0;
        globalOffset = 0;
        dataOffset = 0;
    }
    int mode;
    int globalOffset;
    int dataOffset;

    void computeOffsets() {
        if(mode == 0) { //small mode
            int cOffset = 0;
            for(int i = 0; i<sections.size(); i++) { //concatenate all text sections
                sections[i][0].offset = cOffset;
                cOffset += sections[i][0].code.size();
            }

            for(int i = 0; i<sections.size(); i++) { //concatenate all data sections
                sections[i][1].offset = cOffset;
                cOffset += sections[i][1].code.size();
            }

            for(int i = 0; i<labelsPerObj.size(); i++) {
                for(LabelMap::iterator it = labelsPerObj[i].begin(); it != labelsPerObj[i].end(); it++) {
                    if(it->second.needImport != 1) {
                        it->second.position += globalOffset + sections[i][it->second.section].offset;
                    }
                }
            }

        }
    }

    int findGlobalLabel(std::string name, LabelEntry & entry) {
        for(int i = 0; i<labelsPerObj.size(); i++) {
            LabelMap::iterator it = labelsPerObj[i].find(name);
            if(it != labelsPerObj[i].end()) {
                if(it->second.needImport == 0) {
                    entry = it->second;
                    return 1;
                }
            }
        }
        return 0;
    }

    void resolveNames() {
        for(int i = 0; i<labelsPerObj.size(); i++) {
            for(LabelMap::iterator it = labelsPerObj[i].begin(); it != labelsPerObj[i].end(); it++) {
                if(it->second.needImport == 1) {
                    LabelEntry entry;
                    if(findGlobalLabel(it->first, entry) == 0) {
                        printf("Unresolved symbol %s in file %s\n", it->first.c_str(), sections[i][0].filename.c_str());

                        exit(1);
                    } else {
						it->second.position = entry.position;
						it->second.section = entry.section;
						it->second.needImport = 0;
					}
                }
            }
        }
    }

    void loadFile(std::string filename) {
        //printf("Loading %s.. ", filename.c_str());
        std::ifstream file;
        file.open(filename.c_str(), std::ios::in | std::ios::binary);

        Section textSection;
        Section dataSection;

        textSection.filename = filename;
        dataSection.filename = filename;



        char header[4];
        char textSizeHigh;
        char textSizeLow;
        char dataSizeHigh;
        char dataSizeLow;
        char textFull[0xffff * 2];
        char dataFull[0xffff * 2];
        char labelFull[0xffff];
        char labelFullLenH;
        char labelFullLenL;

        file.read(header, 4);

        if(memcmp(header, "ROBJ", 4)) {
            printf("Bad file %s. Skipping.\n", filename.c_str());
            file.close();
            return;
        }

        file.read(&textSizeHigh, 1);
        file.read(&textSizeLow, 1);
        file.read(&dataSizeHigh, 1);
        file.read(&dataSizeLow, 1);
        file.read(&labelFullLenH, 1);
        file.read(&labelFullLenL, 1);
        int labelFullLen = (((uint8_t)labelFullLenH << 8) | (uint8_t)labelFullLenL);
        file.read(labelFull, labelFullLen);
        file.read(textSection.labelMap, 0xffff);
        file.read(textFull, 0xffff*2);
        file.read(dataSection.labelMap, 0xffff);
        file.read(dataFull, 0xffff*2);
        file.close();

        int textSize = (((uint8_t)textSizeHigh << 8) | (uint8_t)textSizeLow);
        int dataSize = (((uint8_t)dataSizeHigh << 8) | (uint8_t)dataSizeLow);

        convert8to16(textFull, textSection.code, textSize);
        convert8to16(dataFull, dataSection.code, dataSize);

        LabelMap cLabels;
        convert8ToLabels(labelFull, cLabels, labelFullLen);


        std::vector<Section> tVec;
        tVec.push_back(textSection);
        tVec.push_back(dataSection);

        sections.push_back(tVec);
        labelsPerObj.push_back(cLabels);
/*
        printf("OK\n");
        printf("Object props:\n  textSize: %d words\n  dataSize: %d words\n  labelLength: %d\n", textSize, dataSize, labelFullLen);

        printf("Labels:\n");
        for(std::map<std::string, LabelEntry>::iterator it = cLabels.begin(); it!= cLabels.end(); it++) {
            std::string labelName = it->first;
            LabelEntry entry = it->second;

            printf("%s uid: %d pos: %d import: %d export: %d, section: %d\n", labelName.c_str(), entry.uid, entry.position, entry.needImport, entry.needExport, entry.section);
        }
*/
    }

    void dumpLabels() {

        printf("Labels:\n");

        for(int i = 0; i<labelsPerObj.size(); i++) {
            for(std::map<std::string, LabelEntry>::iterator it = labelsPerObj[i].begin(); it!= labelsPerObj[i].end(); it++) {
                std::string labelName = it->first;
                LabelEntry entry = it->second;

                printf("%s obj: %d uid: %d pos: %d import: %d export: %d, section: %d\n", labelName.c_str(), i, entry.uid, entry.position, entry.needImport, entry.needExport, entry.section);
            }
        }
    }

    int findLabelByUid(uint16_t uid, LabelMap & labelMap, LabelEntry & entry) {
        for(LabelMap::iterator it = labelMap.begin(); it != labelMap.end(); it++) {
            if(it->second.uid == uid) {
                entry = it->second;
                return 1;
            }
        }
        return 0;
    }

    void link() {
        for(int i = 0; i<sections.size(); i++) {
            for(int j = 0; j<0xffff; j++) {
                if(sections[i][0].labelMap[j]) {
                    LabelEntry entry;
                    uint16_t labelUid = sections[i][0].code[j];
                    if(!findLabelByUid(labelUid, labelsPerObj[i], entry)) {
                        printf("Label with uid %d not found!\n", labelUid);
                        exit(1);
                    } else {
                    //printf("Obj %d uid %d pos %d\n", i, labelUid, entry.position);
						sections[i][0].code[j] = entry.position;
					}
                }
            }
        }
    }

    void writeBin(std::string filename) {
        std::ofstream file;
        file.open(filename.c_str(), std::ios::out | std::ios::binary);

        char image[0xffff*2];
        memset(image, 0, 0xffff*2);
        char *p = image;
        if(mode == 0) { //small mode
            for(int sect = 0; sect < 2; sect++) {//write code first
                for(int i = 0; i<sections.size(); i++) {
                    for(int j = 0; j<sections[i][sect].code.size(); j++) {
                        uint16_t val = sections[i][sect].code[j];
                        char h = ((val & 0xff00) >> 8);
                        char l = (val & 0xff);
                        *p = h; p++;
                        *p = l; p++;
                    }
                }
            }
            printf("%s: %lu words\n", filename.c_str(), p - image);
        }

        file.write(image, 0xffff*2);
        file.close();
    }

};


int main(int argc, char ** argv) {
    std::vector<std::string> inFiles;
    std::string outFile;
    int textOffset = 0;
    int dataOffset = 0;
    int mode = 0;

    for(int i = 1; i<argc; i++) {
        if(!strcmp(argv[i], "-o")) {
            i++;
            if(i >= argc) {
                printf("bad command line\n");
                exit(1);
            }
            outFile = std::string(argv[i]);
        } else if(!strcmp(argv[i], "-text")) {
            i++;
            if(i >= argc) {
                printf("bad command line\n");
                exit(1);
            }
            textOffset = strtol(argv[i], NULL, 0);
        } else if(!strcmp(argv[i], "-data")) {
            i++;
            if(i >= argc) {
                printf("bad command line\n");
                exit(1);
            }
            dataOffset = strtol(argv[i], NULL, 0);
        } else if(!strcmp(argv[i], "-mode")) {
            i++;
            if(i >= argc) {
                printf("bad command line\n");
                exit(1);
            }
            mode = atoi(argv[i]);
        } else {
            inFiles.push_back(std::string(argv[i]));
        }
    }
/*
    printf("Linking files:\n");
    for(int i = 0; i<inFiles.size(); i++) {
        printf(" <- %s\n", inFiles[i].c_str());
    }
    printf("To file\n -> %s\n", outFile.c_str());
    printf("Offsets:\n  Text: %d\n  Data: %d\nMode: %s\n", textOffset, dataOffset, mode?"large":"small");
*/
    Linker linker;
    linker.mode = mode;
    linker.globalOffset = textOffset;
    linker.dataOffset = dataOffset;

    for(int i = 0; i<inFiles.size(); i++) {
        linker.loadFile(inFiles[i]);
    }



    //linker.dumpLabels();
    //printf("Compute offsets..\n");
    linker.computeOffsets();
    //printf("Resolve symbols..\n");
    linker.resolveNames();

    //linker.dumpLabels();
    //printf("Linking..\n");
    linker.link();
//    printf("Writing to %s..\n", outFile.c_str());
    linker.writeBin(outFile);
//    printf("Done.\n");
    return 0;

}

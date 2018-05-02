#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <algorithm>

#include "oplist.h"

size_t code_block_size = 8;
size_t text_space = 0x10000 * code_block_size;
size_t data_space = 0x10000;
enum {
    O_MODE_ONESEG = 0,
    O_MODE_TWOSEG = 1,
    O_MODE_RAWBIN = 2
};


        char image[0xffffff];

typedef struct LabelEntry_t {
    LabelEntry_t() {uid = -1; position = -1; needExport = 0; needImport = 0; section = 0; shift = 0;}
    int section;
    int uid;
    int position;
    int needExport;
    int needImport;
    int shift;

	std::string filename;

    std::string name;
} LabelEntry;



typedef struct Section_t {
    std::vector<char> labelMap;
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

                //labelName = std::string(labelName, 0, found);
                //printf("Found complex label! Name: [%s] shift: [%s] eval: [%d]\n", labelName.c_str(), shiftStr.c_str(), shRes);
                labelShift = shRes;
            }

            entry.uid = (((uint8_t)uidH << 8) | (uint8_t)uidL);
            entry.position = (((uint8_t)posH << 8) | (uint8_t)posL);
            entry.section = (uint8_t)sect;
            entry.needImport = (uint8_t)im;
            entry.needExport = (uint8_t)ex;
            entry.shift = labelShift;
            entry.name = labelName;
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
    

    void strip() {
        //printf("Strip: %d objs before\n", labelsPerObj.size());
        std::vector<int> requiredObjs;
        requiredObjs.push_back(whoProvides("__progbeg"));

//        printf("added main from %d\n", requiredObjs[0]);


        for(int i = 0; i<requiredObjs.size(); i++) {
            int r = requiredObjs[i];
//            printf("Checking obj %d [%s]\n", r, sections[r][0].filename.c_str());
            for(const auto & ll : labelsPerObj[r]) {
                if(ll.second.needImport) {
//                    printf("Searching label %s\n", ll.first.c_str());
                    int who = whoProvides(ll.first);
//                    printf("Found as %d\n", who);
                    if(who < 0) {
                        //printf("Warning! Label %s was imported but not found!\n", ll.first.c_str());
                    } else {
                        if(std::find(requiredObjs.begin(), requiredObjs.end(), who) == requiredObjs.end()) {
//                            printf("%s needs import - found in %d [%s]\n", ll.first.c_str(), who, sections[who][0].filename.c_str());
                            requiredObjs.push_back(who);
                            //printf("%s requested by %s\n", sections[who][0].filename.c_str(), sections[r][0].filename.c_str());

                        } else {
//                            printf("File already imported\n");
                        }
                    }
                }
            }
        }

        std::vector<std::vector<Section> > newSects;
        std::vector<LabelMap> newLabelsPerObj;

        std::sort(requiredObjs.begin(), requiredObjs.end());

        for(const auto i : requiredObjs) {
            newSects.push_back(sections[i]);
            newLabelsPerObj.push_back(labelsPerObj[i]);
        }
        sections = newSects;
        labelsPerObj = newLabelsPerObj;
        //printf("Strip: %d objs after\n", labelsPerObj.size());

    }

    int whoProvides(const std::string & name) {
        for(int i = 0; i<labelsPerObj.size(); i++) {
            if(labelsPerObj[i].count(name)) {
                if(labelsPerObj[i][name].needExport) {
                    return i;
                }
            }
        }
        return -1;
    }


    void computeOffsets() {
        int cOffset = 0;
        for(int i = 0; i<sections.size(); i++) { //concatenate all text sections
            sections[i][0].offset = cOffset;
            cOffset += sections[i][0].code.size();
        }
        if(mode == O_MODE_TWOSEG) cOffset = 0; //reset offset for two-seg mode

        for(int i = 0; i<sections.size(); i++) { //concatenate all data sections
            sections[i][1].offset = cOffset;
            cOffset += sections[i][1].code.size();
        }

        for(int i = 0; i<labelsPerObj.size(); i++) {
            for(LabelMap::iterator it = labelsPerObj[i].begin(); it != labelsPerObj[i].end(); it++) {
                if(it->second.needImport != 1) {
		  if(it->second.section == 0) {
                    it->second.position += (globalOffset + sections[i][it->second.section].offset)/code_block_size;
 		  } else {
                    it->second.position += globalOffset + sections[i][it->second.section].offset + dataOffset;
		  }
                }
            }
        }

    }

    int findGlobalLabel(std::string _name, LabelEntry & entry) {

        std::string name;
        name = _name;
        size_t plusPos = name.find('+');
        size_t minusPos = name.find('-');
        size_t cropPos = 0;
        if(plusPos == std::string::npos && minusPos == std::string::npos) {
            cropPos = 0;
        } else if(plusPos != std::string::npos && minusPos == std::string::npos) {
            cropPos = plusPos;
        } else if(plusPos == std::string::npos && minusPos != std::string::npos) {
            cropPos = minusPos;
        } else if(plusPos != std::string::npos && minusPos != std::string::npos) {
            cropPos = std::min(plusPos, minusPos);
        }

        if(cropPos) {
            name = std::string(_name,0,cropPos);
        }



        for(int i = 0; i<labelsPerObj.size(); i++) {
            LabelMap::iterator it = labelsPerObj[i].find(name);
            if(it != labelsPerObj[i].end()) {
                if(it->second.needImport == 0) {
                    entry = it->second;
                    return 1;
                }
            }
        }
        printf("Fail for name %s crop %s\n", _name.c_str(), name.c_str());
        return 0;
    }

    void resolveNames() {
        for(int i = 0; i<labelsPerObj.size(); i++) {
            for(LabelMap::iterator it = labelsPerObj[i].begin(); it != labelsPerObj[i].end(); it++) {
                if(it->second.needImport == 1) {
                    LabelEntry entry;
                    if(findGlobalLabel(it->first, entry) == 0) {
                        //printf("Unresolved symbol %s in file %s\n", it->first.c_str(), sections[i][0].filename.c_str());
                        //dumpLabels();
                        //exit(1);
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

	textSection.labelMap.resize(text_space, 0);
	dataSection.labelMap.resize(data_space, 0);


        char header[4];
        char textSizeSuperHigh;
        char textSizeHigh;
        char textSizeLow;
        char dataSizeHigh;
        char dataSizeLow;
        char textFull[text_space*2];
        char dataFull[data_space*2];
        char labelFull[0xffff];
        char labelFullLenH;
        char labelFullLenL;

        file.read(header, 4);

        if(memcmp(header, "ROBJ", 4)) {
            printf("Bad file %s. Skipping.\n", filename.c_str());
            file.close();
            return;
        }

        file.read(&textSizeSuperHigh, 1);
        file.read(&textSizeHigh, 1);
        file.read(&textSizeLow, 1);
        file.read(&dataSizeHigh, 1);
        file.read(&dataSizeLow, 1);
        file.read(&labelFullLenH, 1);
        file.read(&labelFullLenL, 1);
        int labelFullLen = (((uint8_t)labelFullLenH << 8) | (uint8_t)labelFullLenL);
        file.read(labelFull, labelFullLen);
        file.read(&textSection.labelMap[0], text_space);
        file.read(textFull, text_space*2);
        file.read(&dataSection.labelMap[0], data_space);
        file.read(dataFull, data_space*2);
        file.close();

        int textSize = (((uint8_t)textSizeSuperHigh << 16) |((uint8_t)textSizeHigh << 8) | (uint8_t)textSizeLow);
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
                //printf("%s:\ttext: %d\tdata: %d\n", filename.c_str(), textSize, dataSize);
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
            for(int cSect = 0; cSect<2; cSect++) {
                for(int j = 0; j < (cSect?data_space:text_space); j++) {
                    if(sections[i][cSect].labelMap[j]) {
                        LabelEntry entry;
                        uint16_t labelUid = sections[i][cSect].code[j];
                        if(!findLabelByUid(labelUid, labelsPerObj[i], entry)) {
                            printf("Label with uid %d not found!\n", labelUid);
                            exit(1);
                        } else {
                            //printf("Obj %d uid %d pos %d\n", i, labelUid, entry.position);
                            if(entry.needImport) {
                                printf("Unresolved symbol %s in file %s\n", entry.name.c_str(), sections[i][cSect].filename.c_str());
                                exit(1);

                            } else {
				if(!entry.section && entry.shift) {
					printf("Shift for code entry %s %d file %s\n", entry.name.c_str(), entry.shift, sections[i][0].filename.c_str());
	//				exit(1);
				}
                                sections[i][cSect].code[j] = entry.position + entry.shift;
                            }
                        }
                    }
                }
            }
        }
    }

    void writeBin(std::string & filename, int fmt) {
        std::ofstream file;
        file.open(filename.c_str(), std::ios::out | std::ios::binary);

        memset(image, 0, 0xffffff);
        char *p = image;
        char *d = image;
        size_t binarySize = 0;
        if(mode != O_MODE_RAWBIN) {
            // magic
            *p = 0;   p++;
            *p = 'R'; p++;
            *p = 0;   p++;
            *p = 'E'; p++;
            *p = 0;   p++;
            *p = 'X'; p++;
            *p = 0;   p++;
            *p = 'E'; p++;

            //mode
            *p = 0;   p++;
            *p = mode; p++;

            for(int sect = 0; sect < 2; sect++) {//write sizes
                unsigned int cSize = 0;
                for(int i = 0; i<sections.size(); i++) {
                    cSize += sections[i][sect].code.size();
                }
		*p = 0;			        p++;
                *p = ((cSize & 0xff0000) >> 16);p++;
                *p = ((cSize & 0xff00) >> 8);   p++;
                *p = (cSize & 0xff);            p++;

            }
        }
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
            printf("%s [%s]: %lu words\n", filename.c_str(), (sect?"data":"text"), (p - d)/2);
            d = p;

        }
        printf("%s: %lu words\n", filename.c_str(), (p - image)/2);
        binarySize = p-image;


        file.write(image, binarySize);
        file.close();
    }

};


int main(int argc, char ** argv) {
    std::vector<std::string> inFiles;
    std::string outFile;
    int textOffset = 0;
    int dataOffset = 0;
    int mode = 0;
    int strip = 0;
    /*
    for(int i = 0; i<argc; i++) {
      printf("Arg %d : %s\n", i, argv[i]);
    }
*/
    for(int i = 1; i<argc; i++) {
        if(!strcmp(argv[i], "-o")) {
            i++;
            if(i >= argc) {
                printf("bad command line\n");
                exit(1);
            }
            outFile = std::string(argv[i]);
        } else if(!strcmp(argv[i], "-strip")) {
            strip = 1;
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


    //  printf("Text offset 0x%04x\nData offset 0x%04x\n", textOffset, dataOffset);

    for(int i = 0; i<inFiles.size(); i++) {
        linker.loadFile(inFiles[i]);
    }

    if(strip) linker.strip();

    //linker.dumpLabels();
    //printf("Compute offsets..\n");
    linker.computeOffsets();
    //printf("Resolve symbols..\n");
    linker.resolveNames();

    //printf("Linking..\n");
    linker.link();
       // printf("Writing to %s..\n", outFile.c_str());
    //linker.dumpLabels();
    linker.writeBin(outFile,0);
    //    printf("Done.\n");

    //    linker.dumpLabels();
    return 0;

}

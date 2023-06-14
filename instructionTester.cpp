#include "instructionTester.h"

using namespace json11;
Json testROMS[0xFF];
std::vector<int> currentREGfails(8);
std::map<std::string, int> totalADDRfails;
std::map<std::string, int> totalADDRtests;

bool testIgnoreStatus = false;
bool testIgnoreCycles = false;
bool testIgnoreRegisters = false;
bool testIgnoreMemory = false;
bool testNoBCD = false;
int testAmount = -1;
int testOpcode = -1;

int doTest(int ROMindex, int testIndex) {
    cycles = 0;
    PC = testROMS[ROMindex][testIndex]["initial"]["pc"].int_value();
    S = testROMS[ROMindex][testIndex]["initial"]["s"].int_value();
    A = testROMS[ROMindex][testIndex]["initial"]["a"].int_value();
    X = testROMS[ROMindex][testIndex]["initial"]["x"].int_value();
    Y = testROMS[ROMindex][testIndex]["initial"]["y"].int_value();
    P = testROMS[ROMindex][testIndex]["initial"]["p"].int_value();
    if (testNoBCD && (mnemonics[ROMindex].substr(6, 3) == "ADC"|| mnemonics[ROMindex].substr(6, 3) == "SBC") && (P & 0b00001000) != 0) {
        return 2;
    }
    uint16_t oldPC = PC;
    uint16_t oldS = S;
    uint16_t oldA = A;
    uint16_t oldP = P;



    for (int i = 0; i < testROMS[ROMindex][testIndex]["initial"]["ram"].array_items().size(); i++) {
        uint16_t TESTaddr = testROMS[ROMindex][testIndex]["initial"]["ram"].array_items()[i][0].int_value();
        uint8_t TESTval = testROMS[ROMindex][testIndex]["initial"]["ram"].array_items()[i][1].int_value();
        SETMEM8(TESTaddr, TESTval);
    }
    if (tick6507() == -1) {
        return -1;
    }
    bool passed = true;
    if (Pin16To13(PC) != Pin16To13(testROMS[ROMindex][testIndex]["final"]["pc"].int_value()) && Pin16To13(PC-1) != Pin16To13(testROMS[ROMindex][testIndex]["final"]["pc"].int_value())) {
        currentREGfails[0]++;
        int testS = S;

        passed = false;
    }
    if (S != testROMS[ROMindex][testIndex]["final"]["s"].int_value() && !testIgnoreRegisters) {
        currentREGfails[1]++;
        passed = false;
    }
    if (A != testROMS[ROMindex][testIndex]["final"]["a"].int_value() && !testIgnoreRegisters) {
        currentREGfails[2]++;
        passed = false;
    }
    if (X != testROMS[ROMindex][testIndex]["final"]["x"].int_value() && !testIgnoreRegisters) {
        currentREGfails[3]++;
        passed = false;
    }
    if (Y != testROMS[ROMindex][testIndex]["final"]["y"].int_value() && !testIgnoreRegisters) {
        currentREGfails[4]++;
        passed = false;
    }
    if (P != testROMS[ROMindex][testIndex]["final"]["p"].int_value() && !testIgnoreStatus) {
        currentREGfails[5]++;
        passed = false;
    }
    if (cycles != testROMS[ROMindex][testIndex]["cycles"].array_items().size() && !testIgnoreCycles) { //testignore cycles var completely broken
        currentREGfails[6]++;
        passed = false;
    }
    if (!testIgnoreMemory) {
        for (int i = 0; i < testROMS[ROMindex][testIndex]["final"]["ram"].array_items().size(); i++) {
            uint16_t TESTaddr = testROMS[ROMindex][testIndex]["final"]["ram"].array_items()[i][0].int_value();
            uint8_t TESTval = testROMS[ROMindex][testIndex]["final"]["ram"].array_items()[i][1].int_value();
            if (GETMEM8(TESTaddr) != TESTval) {
                std::cout << (int)TESTaddr << "should be : (" << (int)testROMS[ROMindex][testIndex]["initial"]["ram"].array_items()[i][1].int_value() << "->"<< (int)TESTval << " but is: " << (int)GETMEM8(TESTaddr) << std::endl;
                currentREGfails[7]++;
                passed = false;
                break;
            }
        }
    }

    totalADDRtests[mnemonics[ROMindex].substr(0, 4)]++;

    if (passed) {
        return passed;
    }

    totalADDRfails[mnemonics[ROMindex].substr(0, 4)]++;

    return passed;

}

void loadTestROM(int ROMindex) {
    std::ifstream t(appPath + "tests/" + getHex((uint8_t)ROMindex) + ".json");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string ROMtxt = buffer.str();
    std::string err;
    if (ROMindex > 0) {
        testROMS[ROMindex - 1] = NULL;
    }
    testROMS[ROMindex] = Json::parse(ROMtxt, err);
}

int testInstructions() {    // If anyone reads this, I know this function is terrible okay, i do not care
    CPUTEST = true;
    
    totalADDRfails["IMP "] = 0;
    totalADDRfails["IMM "] = 0;
    totalADDRfails["ABS "] = 0;
    totalADDRfails["ZRO "] = 0;
    totalADDRfails["ZRX "] = 0;
    totalADDRfails["ZRY "] = 0;
    totalADDRfails["INX "] = 0;
    totalADDRfails["INY "] = 0;
    totalADDRfails["IND "] = 0;
    totalADDRfails["INDX"] = 0;
    totalADDRfails["INDY"] = 0;
    totalADDRfails["REL "] = 0;

    totalADDRtests["IMP "] = 0;
    totalADDRtests["IMM "] = 0;
    totalADDRtests["ABS "] = 0;
    totalADDRtests["ZRO "] = 0;
    totalADDRtests["ZRX "] = 0;
    totalADDRtests["ZRY "] = 0;
    totalADDRtests["INX "] = 0;
    totalADDRtests["INY "] = 0;
    totalADDRtests["IND "] = 0;
    totalADDRtests["INDX"] = 0;
    totalADDRtests["INDY"] = 0;
    totalADDRtests["REL "] = 0;


    
    
    init6507();
    testAmount = testAmount == -1 ? 10000 : testAmount;
    int testOpcodeLimU = testOpcode == -1 ? 0xFF : testOpcode;
    int testOpcodeLimL = testOpcode == -1 ? 0 : testOpcode;
    int totalPassed = 0;
    int totalLegal = testOpcodeLimU-testOpcodeLimL+1;
    int totalTests = 0;
    if (testOpcodeLimU == testOpcodeLimL) {
        totalLegal--;
    }

    for (int i = testOpcodeLimL; i <= testOpcodeLimU; i++) {
        std::fill(currentREGfails.begin(), currentREGfails.end(), 0);   // Reset reg fails
        
        std::cout << "testing: " << getHex((uint8_t)i) << ", " << std::setfill(' ') << std::setw(sizeof(char) * 9) << mnemonics[i] << "...         ";
        if (LUT[i] == nullptr) {
            totalLegal--;
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, 8);
            std::cout << "illegal opcode" << std::endl;
            SetConsoleTextAttribute(hConsole, 15);
            continue;
        }
        loadTestROM(i);
        int passedAmount = 0;
        int currentTestAmount = 0;

        int result;
        for (int j = 0; j < testAmount; j++) {
            result = doTest(i, j);
            if (result == 1) {
                passedAmount++;
                currentTestAmount++;
            }
            else if (result == 2) {
                // not tested (nonBCD mode)
            }
            else if (result == 0) { // Failed
                currentTestAmount++;
            }
            else if (result == -1) {
                totalLegal--;
                passedAmount = -1;
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 8);
                std::cout << "illegal opcode" << std::endl;
                SetConsoleTextAttribute(hConsole, 15);
                break;
            }
        }

        if (passedAmount != -1) {
            totalPassed += passedAmount;
            totalTests += currentTestAmount;
            if (passedAmount != currentTestAmount) {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 12);
                std::cout << "failed" << "(" << passedAmount << "/" << currentTestAmount << ")                    ";
                for (int k = 0; k < currentREGfails.size(); k++){
                    if (currentREGfails[k] == 0) {
                        continue;
                    }
                    switch (k) {
                        case 0:
                            std::cout << "PC mismatch(";
                            break;
                        case 1:
                            std::cout << "S mismatch(";
                            break;
                        case 2:
                            std::cout << "A mismatch(";
                            break;
                        case 3:
                            std::cout << "X mismatch(";
                            break;
                        case 4:
                            std::cout << "Y mismatch(";
                            break;
                        case 5:
                            std::cout << "P mismatch(";
                            break;
                        case 6:
                            std::cout << "cycle mismatch(";
                            break;
                        case 7:
                            std::cout << "MEM mismatch(";
                            break;
                    }
                    std::cout << currentREGfails[k] << "/" << currentTestAmount << ")  ";
                }
                SetConsoleTextAttribute(hConsole, 15);
            }
            else {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 10);
                std::cout << "passed" << "(" << passedAmount << "/" << currentTestAmount << ") ";
                SetConsoleTextAttribute(hConsole, 15);

            }
            std::cout << std::endl;
        }
    }

    if (testOpcode != -1) {
        return 0;
    }
    // Generate report
    std::cout << "-----------------------------" << std::endl;
    std::cout << "passed: (" << totalPassed << "/" << totalTests << ")" << " of legal opcode tests" << std::endl;
    // Addressing mode report
    std::cout << "Addressing Modes: " << std::endl;


    for (auto const& it : totalADDRfails)
    {
        std::cout << it.first << ": (" << totalADDRtests[it.first]-it.second << "/" << totalADDRtests[it.first] << ")" << std::endl;
    }
    return 0;
}


#include "debug.h"


std::vector<opCall> callStack;
sf::RenderWindow* window;
ImFont* font1;
int maxCalls = 12;
bool cycleButton = false;
bool stepLineButton = false;
bool stepFrameButton = false;

std::map<std::uint8_t, std::string> mnemonics{
    { 0x69, "IMM   ADC" },
    { 0x65, "ZRO   ADC" },
    { 0x75, "ZRX   ADC" },
    { 0x6D, "ABS   ADC" },
    { 0x7D, "INX   ADC" },
    { 0x79, "INY   ADC" },
    { 0x61, "INDX  ADC" },
    { 0x71, "INDY  ADC" },
    { 0x29, "IMM   AND" },
    { 0x25, "ZRO   AND" },
    { 0x35, "ZRX   AND" },
    { 0x2D, "ABS   AND" },
    { 0x3D, "INX   AND" },
    { 0x39, "INY   AND" },
    { 0x21, "INDX  AND" },
    { 0x31, "INDY  AND" },
    { 0x0A, "IMP   ASL" },
    { 0x06, "ZRO   ASL" },
    { 0x16, "ZRX   ASL" },
    { 0x0E, "ABS   ASL" },
    { 0x1E, "INX   ASL" },
    { 0x90, "REL   BCC" },
    { 0xB0, "REL   BCS" },
    { 0xF0, "REL   BEQ" },
    { 0x24, "ZRO   BIT" },
    { 0x2C, "ABS   BIT" },
    { 0x30, "REL   BMI" },
    { 0xD0, "REL   BNE" },
    { 0x10, "REL   BPL" },
    { 0x00, "IMP   BRK" },
    { 0x50, "REL   BVC" },
    { 0x70, "REL   BVS" },
    { 0x18, "IMP   CLC" },
    { 0xD8, "IMP   CLD" },
    { 0x58, "IMP   CLI" },
    { 0xB8, "IMP   CLV" },
    { 0xC9, "IMM   CMP" },
    { 0xC5, "ZRO   CMP" },
    { 0xD5, "ZRX   CMP" },
    { 0xCD, "ABS   CMP" },
    { 0xDD, "INX   CMP" },
    { 0xD9, "INY   CMP" },
    { 0xC1, "INDX  CMP" },
    { 0xD1, "INDY  CMP" },
    { 0xE0, "IMM   CPX" },
    { 0xE4, "ZRO   CPX" },
    { 0xEC, "ABS   CPX" },
    { 0xC0, "IMM   CPY" },
    { 0xC4, "ZRO   CPY" },
    { 0xCC, "ABS   CPY" },
    { 0xC6, "ZRO   DEC" },
    { 0xD6, "ZRX   DEC" },
    { 0xCE, "ABS   DEC" },
    { 0xDE, "INX   DEC" },
    { 0xCA, "IMP   DEX" },
    { 0x88, "IMP   DEY" },
    { 0x49, "IMM   EOR" },
    { 0x45, "ZRO   EOR" },
    { 0x55, "ZRX   EOR" },
    { 0x4D, "ABS   EOR" },
    { 0x5D, "INX   EOR" },
    { 0x59, "INY   EOR" },
    { 0x41, "INDX  EOR" },
    { 0x51, "INDY  EOR" },
    { 0xE6, "ZRO   INC" },
    { 0xF6, "ZRX   INC" },
    { 0xEE, "ABS   INC" },
    { 0xFE, "INX   INC" },
    { 0xE8, "IMP   INX" },
    { 0xC8, "IMP   INY" },
    { 0x4C, "ABS   JMP" },
    { 0x6C, "IND   JMP" },
    { 0x20, "ABS   JSR" },
    { 0xA9, "IMM   LDA" },
    { 0xA5, "ZRO   LDA" },
    { 0xB5, "ZRX   LDA" },
    { 0xAD, "ABS   LDA" },
    { 0xBD, "INX   LDA" },
    { 0xB9, "INY   LDA" },
    { 0xA1, "INDX  LDA" },
    { 0xB1, "INDY  LDA" },
    { 0xA2, "IMM   LDX" },
    { 0xA6, "ZRO   LDX" },
    { 0xB6, "ZRY   LDX" },
    { 0xAE, "ABS   LDX" },
    { 0xBE, "INY   LDX" },
    { 0xA0, "IMM   LDY" },
    { 0xA4, "ZRO   LDY" },
    { 0xB4, "ZRX   LDY" },
    { 0xAC, "ABS   LDY" },
    { 0xBC, "INX   LDY" },
    { 0x4A, "IMP   LSR" },
    { 0x46, "ZRO   LSR" },
    { 0x56, "ZRX   LSR" },
    { 0x4E, "ABS   LSR" },
    { 0x5E, "INX   LSR" },
    { 0xEA, "IMP   NOP" },
    { 0x09, "IMM   ORA" },
    { 0x05, "ZRO   ORA" },
    { 0x15, "ZRX   ORA" },
    { 0x0D, "ABS   ORA" },
    { 0x1D, "INX   ORA" },
    { 0x19, "INY   ORA" },
    { 0x01, "INDX  ORA" },
    { 0x11, "INDY  ORA" },
    { 0x48, "IMP   PHA" },
    { 0x08, "IMP   PHP" },
    { 0x68, "IMP   PLA" },
    { 0x28, "IMP   PLP" },
    { 0x2A, "IMP   ROL" },
    { 0x26, "ZRO   ROL" },
    { 0x36, "ZRX   ROL" },
    { 0x2E, "ABS   ROL" },
    { 0x3E, "INX   ROL" },
    { 0x6A, "IMP   ROR" },
    { 0x66, "ZRO   ROR" },
    { 0x76, "ZRX   ROR" },
    { 0x6E, "ABS   ROR" },
    { 0x7E, "INX   ROR" },
    { 0x40, "IMP   RTI" },
    { 0x60, "IMP   RTS" },
    { 0xE9, "IMM   SBC" },
    { 0xE5, "ZRO   SBC" },
    { 0xF5, "ZRX   SBC" },
    { 0xED, "ABS   SBC" },
    { 0xFD, "INX   SBC" },
    { 0xF9, "INY   SBC" },
    { 0xE1, "INDX  SBC" },
    { 0xF1, "INDY  SBC" },
    { 0x38, "IMP   SEC" },
    { 0xF8, "IMP   SED" },
    { 0x78, "IMP   SEI" },
    { 0x85, "ZRO   STA" },
    { 0x95, "ZRX   STA" },
    { 0x8D, "ABS   STA" },
    { 0x9D, "INX   STA" },
    { 0x99, "INY   STA" },
    { 0x81, "INDX  STA" },
    { 0x91, "INDY  STA" },
    { 0x86, "ZRO   STX" },
    { 0x96, "ZRY   STX" },
    { 0x8E, "ABS   STX" },
    { 0x84, "ZRO   STY" },
    { 0x94, "ZRX   STY" },
    { 0x8C, "ABS   STY" },
    { 0xAA, "IMP   TAX" },
    { 0xA8, "IMP   TAY" },
    { 0xBA, "IMP   TSX" },
    { 0x8A, "IMP   TXA" },
    { 0x9A, "IMP   TXS" },
    { 0x98, "IMP   TYA" }
};


void debugLog() {
    sf::Clock deltaClock;
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }

        sf::Time deltaTime = deltaClock.restart();
        ImGui::SFML::Update(*window, deltaTime);
        auto size = window->getSize();

        auto windowPos = ImVec2(.0f, 20.0f);
        ImGui::SetNextWindowPos(windowPos);
        auto contentRegion = ImGui::GetWindowContentRegionMax();
        contentRegion.y -= windowPos.y + 10;
        ImGui::SetWindowSize(size);
        ImGui::SetNextWindowContentSize(contentRegion);
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        ImGui::Begin("Hello", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::Columns(3, "locations");

        ImGui::SetColumnWidth(0, size.x/3);
        ImGui::SetColumnWidth(1, size.x/3);
        ImGui::SetColumnWidth(1, size.x/3);
        logRegisters();
        ImGui::NextColumn();
        logCallStack();
        ImGui::NextColumn();
        logStack();
        ImGui::Columns();
        logTIA();
        logPIA();
        initButtons();
        ImGui::End();

        window->clear(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
        ImGui::SFML::Render(*window);
        window->display();
    }

    ImGui::SFML::Shutdown();

}

void logTIA() {
    auto size = window->getSize();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("");
    ImGui::Text("");
    ImGui::Text("TIA:");
    ImGui::SetWindowFontScale(1.5);

    std::string regStr = "ClrClk:  " + std::to_string((int)(getColorClock()));
    ImGui::Text(regStr.c_str());

    //  COLUP0
    regStr = "    COLUP0:  0x" + getHex(MEM[0x06]);
    ImGui::Text(regStr.c_str());

    //col
    ImGui::SameLine();
    uint8_t* currentSFMLcol = getNTSCcolor(MEM[0x06]);
    ImVec4 currentColor((float)currentSFMLcol[0], (float)currentSFMLcol[1], (float)currentSFMLcol[2], 255);
    free(currentSFMLcol);
    ImGui::ColorButton("", currentColor, 0);
    //  COLUP1
    regStr = "    COLUP1:  0x" + getHex(MEM[0x07]);
    ImGui::Text(regStr.c_str());

    //col
    ImGui::SameLine();
    currentSFMLcol = getNTSCcolor(MEM[0x07]);
    currentColor = { (float)currentSFMLcol[0], (float)currentSFMLcol[1], (float)currentSFMLcol[2], 255 };
    free(currentSFMLcol);
    ImGui::ColorButton("", currentColor, 0);
    //  COLUPF
    regStr = "    COLUPF:  0x" + getHex(MEM[0x08]);
    ImGui::Text(regStr.c_str());

    //col
    ImGui::SameLine();
    currentSFMLcol = getNTSCcolor(MEM[0x08]);
    currentColor = { (float)currentSFMLcol[0], (float)currentSFMLcol[1], (float)currentSFMLcol[2], 255 };
    free(currentSFMLcol);
    ImGui::ColorButton("", currentColor, 0);

    //  COPUBK
    regStr = "    COLUBK:  0x" + getHex(MEM[0x09]);
    ImGui::Text(regStr.c_str());

    //col
    ImGui::SameLine();
    currentSFMLcol = getNTSCcolor(MEM[0x09]);
    currentColor = { (float)currentSFMLcol[0], (float)currentSFMLcol[1], (float)currentSFMLcol[2], 255 };
    free(currentSFMLcol);
    ImGui::ColorButton("", currentColor, 0);

    // Positions
    ImGui::SetWindowFontScale(1.5);
    regStr = "";
    ImGui::Text(regStr.c_str());
    regStr = "POS (p0): " + std::to_string((int)(PMBPOS[0])) + "   HM: " + getHex(MEM[0x20]);
    ImGui::Text(regStr.c_str());
    regStr = "POS (p1): " + std::to_string((int)(PMBPOS[1])) + "   HM: " + getHex(MEM[0x21]);
    ImGui::Text(regStr.c_str());
    regStr = "POS (m0): " + std::to_string((int)(PMBPOS[2])) + "   HM: " + getHex(MEM[0x22]);
    ImGui::Text(regStr.c_str());
    regStr = "POS (m1): " + std::to_string((int)(PMBPOS[3])) + "   HM: " + getHex(MEM[0x23]);
    ImGui::Text(regStr.c_str());
    regStr = "POS (BL): " + std::to_string((int)(PMBPOS[4])) + "   HM: " + getHex(MEM[0x24]);
    ImGui::Text(regStr.c_str());

    // Playfield

    regStr = "Playfield";
    ImGui::Text(regStr.c_str());

    uint8_t Reversed0E = 0;
    for (int i = 0; i < 8; i++) {
        reverseByte(MEM[0x0E]);
    }
    uint32_t playField = (((uint32_t)MEM[0x0D] >> 4) | ((uint32_t)Reversed0E << 4) | ((uint32_t)MEM[0x0F] << 12));
    std::string binary = std::bitset<32>(playField).to_string();
    ImGui::SetWindowFontScale(0.5f);
    for (uint8_t i = 0; i < 20; i++) {
        bool pfBit = binary[32-i] == '1';
        ImGui::Checkbox("", &pfBit);
        ImGui::SameLine();
    }

    ImGui::SetWindowFontScale(1.5);
    regStr = " ";
    ImGui::Text(regStr.c_str());

    regStr = "GRP0          " + getBin(MEM[0x1B]);
    ImGui::Text(regStr.c_str());
    regStr = "GRP0 delayed  " + getBin(GRPdelayed[0]);
    ImGui::Text(regStr.c_str());
    regStr = "NUSIZ0 " + std::to_string((int)MEM[0x04] & 0b00000111);
    ImGui::Text(regStr.c_str());

    regStr = "GRP1          " + getBin(MEM[0x1C]);
    ImGui::Text(regStr.c_str());
    regStr = "GRP1 delayed  " + getBin(GRPdelayed[1]);
    ImGui::Text(regStr.c_str());
    regStr = "NUSIZ1 " + std::to_string((int)MEM[0x05] & 0b00000111);
    ImGui::Text(regStr.c_str());
}

void logPIA() {
    auto size = window->getSize();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("");
    ImGui::Text("");
    ImGui::Text("PIA:");
    ImGui::SetWindowFontScale(1.5);

    //  COLUP0
    std::string regStr = "    INTIM:  0x" + getHex(MEM[0x284]);
    ImGui::Text(regStr.c_str());
    regStr = "    INTIMclocks:  0x" + getHex(INTIMclocks);
    ImGui::Text(regStr.c_str());
    regStr = "    Interval:  0x" + getHex(currentInterval);
    ImGui::Text(regStr.c_str());
}


void logRegisters() {
    auto size = window->getSize();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("Registers:");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Text("");
    std::string regStr = "    A:  0x" + getHex(A);
    ImGui::Text(regStr.c_str());
    regStr = "    X:  0x" + getHex(X);
    ImGui::Text(regStr.c_str());
    regStr = "    Y:  0x" + getHex(Y);
    ImGui::Text(regStr.c_str());
    regStr = "    S:  0x" + getHex(S);
    ImGui::Text(regStr.c_str());
    regStr = "    PC: 0x" + getHex(PC);
    ImGui::Text(regStr.c_str());
    regStr = "    P:  " + getBin(P);
    ImGui::Text(regStr.c_str());

    regStr = "    cycles: " + std::to_string((int)cycles);
    ImGui::Text(regStr.c_str());
}

void logCallStack() {
    auto size = window->getSize();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("Call Stack:");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Text("");
    ImGui::Text("     ADR  OPR  ARG");
    ImGui::Text("     ---  ---  ---");
    for (int i = callStack.size()-1; i >= (int)callStack.size()-maxCalls && i >= 0; i--) {
        std::string callStr;
        callStr += "    " + getHex(callStack[i].opAddr) + "   ";
        callStr += getHex(callStack[i].opCode) + "   ";
        callStr += getHex(callStack[i].opArg) + "\n";
        ImGui::Text(callStr.c_str());
    }
}

void logStack() {
    auto size = window->getSize();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("RAM:");
    ImGui::SetWindowFontScale(1.15);
    ImGui::Text("");
    ImGui::Text("     8  9  A  B  C  D  E  F");
    ImGui::Text("    ________________________");

    std::string RAMelem = "";
    for (int i = 0x0080; i <= 0x0080+((0x00FF-0x0080)/8); i++) {
        RAMelem = getHex(i)[7];
        RAMelem = "  " + RAMelem + "| ";
        for (int j = 0; j < 8; j++) {
            RAMelem += getHex(GETMEM8(i + 16 * j)) + " ";
        }
        ImGui::Text(RAMelem.c_str());
    }

}

void initDebug() {
    auto mode = sf::VideoMode::getDesktopMode();
    window = new sf::RenderWindow(mode, "ATARI DEBUG");
    sf::Vector2u windowSize = sf::Vector2u(800, 1400);
    window->setSize(windowSize);
    window->setView(sf::View(sf::FloatRect(0.f, 0.f, windowSize.x, windowSize.y)));
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);
    window->resetGLStates();
    ImGui::SFML::Init(*window);

    ImGuiStyle* style = &ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    font1->Scale = 0.5;
    ImGui::SFML::UpdateFontTexture();


    return;
}

void initButtons() {
    ImGui::SetWindowFontScale(2);
    ImGui::Text("   ");
    if (ImGui::Button("+ cycle  ")) {
        cycleButton = true;
    }
    if (ImGui::Button("+ ScanLn")) {
        stepLineButton = true;
    }
    if (ImGui::Button("+ Frame ")) {
        stepFrameButton = true;
    }
}

template< typename T >
std::string getHex(T nothex) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(T)*2) << (int)nothex;
    return ss.str();
}

std::string getBin(int notbin) {
    std::string binary = std::bitset<8>(notbin).to_string(); //to binary
    return binary;
}

uint16_t Pin16To13(uint16_t pin16) {
    return pin16&0x1fff;
}

void endDebug() {

}
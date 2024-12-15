#include <cstdarg>
#include <pthread.h>
#ifndef CHIP_8_H
  typedef unsigned char BYTE;
  typedef unsigned short WORD; // 16bits
  #define CHIP_8_H
  #include <stack>
  #include <vector>
  #include <string>
  #include "display/Display.hpp"
  class Chip8 {
    private:
      std::vector<BYTE> memory;
      Display displayHandler;
      WORD pc;
      WORD iReg;
      std::stack<WORD> stack;
      BYTE delayTimer;
      BYTE soundTimer;
      std::vector<BYTE> generalRegisters;
      
      void readRoom(const std::string& path);
      WORD fetchInstruction();
      void executeInstruction(const WORD& instruction);
      void clearScreen();
      void jmp(const WORD& i);
      void set(const WORD& i);
      void add(const WORD& i);
      void setIndex(const WORD& i);
      void display(const WORD& i);
      void call(const WORD& i);
      void ret();
      void skipXEqNN(const WORD& i);
      void skipXNeqNN(const WORD& i);
      void skipXEqY(const WORD& i);
      void skipXNeqY(const WORD& i);
      void iRegIncPcByTwo();
      void printMemory();
      void setFontCharacter(const WORD& i);
      void setXY(const WORD& i);
      void binaryOr(const WORD& i);
      void binaryAnd(const WORD& i);
      void logicalXor(const WORD& i);
      void addWithPossibleOverflow(const WORD& i);
      void subtract(const WORD& i, const bool inverse);
      void shift(const WORD& i, const bool isRight);
      
      void addToIndex(const WORD& i);    
      void binaryCodedDecimalConversion(const WORD& i);
      void storeMemory(const WORD& i);
      void loadMemory(const WORD& i);

    public:
      Chip8();
      void start(const std::string& roomPath);
  };

#endif

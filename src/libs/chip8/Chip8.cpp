#include "Chip8.hpp"
#include <bits/chrono.h>
#include <cstring>
#include <ios>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <unistd.h>

Chip8::Chip8() : memory(4096, 0), 
  generalRegisters(16, 0), 
  getKeyCycle(false), 
  delayTimer(0),
  soundTimer(0) {
  BYTE font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // Chip8::memory.insert(Chip8::memory.begin() + 0x050, &font[0], &font[(unsigned) sizeof(font) / sizeof(BYTE)]);
  memcpy(&Chip8::memory[0x050], &font, sizeof(BYTE) * 80);
  Chip8::pc = 0x200;
  Chip8::iReg = 0x0;
}

void Chip8::readRoom(const std::string& path) {
  std::cout << "Reading file: " << path << std::endl;

  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    std::ifstream::pos_type pos = file.tellg();

    std::vector<char> result(pos);

    file.seekg(0, std::ios::beg);
    file.read(&result[0], pos);
    
    int x = 0;
    for (auto &c : result) {
      Chip8::memory[0x200 + x] = c;
      x++;
    }

    std::cout << "Copied bytes: " << x << std::endl;
    
    file.close();
  }
  else {
    std::cout << "Error reading file" << std::endl;
    std::exit(1);
  }

}

void Chip8::iRegIncPcByTwo() {
  Chip8::pc += 2;
  return;
}

WORD Chip8::fetchInstruction() {
  WORD i = ( (Chip8::memory[Chip8::pc] << 8) | Chip8::memory[Chip8::pc + 0x1]);
  Chip8::iRegIncPcByTwo();
  return i;
}

void Chip8::executeInstruction(const WORD& instruction) {
  WORD nibble = (instruction >> 12) & 0xF;
  WORD otherBits = (instruction & 0xFFF);
  
  std::cout << "Executing instruction: " << std::hex << instruction << std::endl;
  std::cout << "Nibble: " <<  std::hex << (int)nibble << std::endl;
  // std::cin.get();

  switch (nibble) {
    case 0x0: {
      switch (otherBits) {
        case 0x0E0: {
          Chip8::clearScreen();
          break;
        }

        case 0x0EE: {
          Chip8::ret();
          break;
        }
      }

      break;
    }
    
     case 0x1: {
      Chip8::jmp(otherBits);

      break;
    }
    
    case 0x2: {
      Chip8::call(otherBits);

      break;
    }
    
    case 0x3: {
      Chip8::skipXEqNN(otherBits);

      break;
    }

    case 0x4: {
      Chip8::skipXNeqNN(otherBits);

      break;
    }

    case 0x5: {
      Chip8::skipXEqY(otherBits);

      break;
    }

    case 0x6: {
      Chip8::set(otherBits);

      break;
    }

    case 0x7: {
      Chip8::add(otherBits);

      break;
    }

    case 0x8 : { 
      BYTE instructionNibble = otherBits & 0xF;
      
      switch (instructionNibble) {
        case 0x0 : {
          Chip8::setXY(otherBits);

          break;
        }

        case 0x1 : {
          Chip8::binaryOr(otherBits);

          break;
        }

        case 0x2 : {
          Chip8::binaryAnd(otherBits);

          break;
        }

        case 0x3 : {
          Chip8::logicalXor(otherBits);

          break;
        }

        case 0x4 : {
          Chip8::addWithPossibleOverflow(otherBits);

          break;
        }

        case 0x5 : { 
          Chip8::subtract(otherBits, false);

          break;
        }
        
        case 0x6 : {
          Chip8::shift(otherBits, true);

          break;
        }

        case 0x7: {
          Chip8::subtract(otherBits, true);
          
          break;
        }

        case 0xE : {
          Chip8::shift(otherBits, false);

          break;
        }
      }

      break;
    }

    case 0x9: {
      Chip8::skipXNeqY(otherBits);
      
      break;
    }

    case 0xA: {
      Chip8::setIndex(otherBits);

      break;
    }

    case 0xB: {
      jmpWithOffset(otherBits);
      break;
    }

    case 0xC: {

      break;
    }

    case 0xD: {
      Chip8::display(otherBits);

      break;
    }

    case 0xE: {
      BYTE xRegister = (otherBits >> 8) & 0xF;
      BYTE selector = otherBits & 0xFF;
      switch (selector) {
        case 0x9E : {
          Chip8::skipIfKey(xRegister, false);
          break;
        }

        case 0xA1: {
          Chip8::skipIfKey(xRegister, true);
          break;
        }
      }

      break;
    }

    case 0xF: {
      BYTE xRegister = (otherBits >> 8) & 0xF;
      BYTE selector = otherBits & 0xFF;

      switch (selector) {
        case 0x0A: {
          Chip8::getKey(xRegister);
          break;
        }

        case 0x29: {
          Chip8::setFontCharacter(xRegister);
          break;
        }
        
        case 0x1E : {
          Chip8::addToIndex(xRegister);
          break;
        }

        case 0x33 : {
          Chip8::binaryCodedDecimalConversion(xRegister);
          break;
        }

        case 0x55 : {
          Chip8::storeMemory(xRegister);
          break;
        }

        case 0x65 : {
          Chip8::loadMemory(xRegister);
          break;
        }

        case 0x07 : {
          Chip8::setVXDelayTimer(xRegister);
          break;
        }

        case 0x15 : {
          Chip8::setDelayTimer(xRegister);
          break;
        }

        case 0x18 : {
          Chip8::setSoundTimer(xRegister);
          break;
        }
      }

      break;
    }
  }
  
  return;
}


void Chip8::jmp(const WORD& i) {
  Chip8::pc = i;
  return;
}

void Chip8::call(const WORD& i) {
  Chip8::stack.push(Chip8::pc);
  Chip8::pc = i;
  return;
}

void Chip8::ret() {
  Chip8::pc = Chip8::stack.top();
  Chip8::stack.pop();
  return;
}

void Chip8::skipXEqNN(const WORD& i) {
  BYTE x = (i >> 8) & 0xF;
  BYTE nn = i & 0x0FF;

  if (Chip8::generalRegisters[x] == nn) Chip8::iRegIncPcByTwo();

  return;
}

void Chip8::skipXNeqNN(const WORD& i) {
  BYTE x = (i >> 8) & 0xF;
  BYTE nn = i & 0x0FF;

  if (Chip8::generalRegisters[x] != nn) Chip8::iRegIncPcByTwo();

  return;
}

void Chip8::skipXEqY(const WORD& i) {
  BYTE x = (i >> 8) & 0xF;
  BYTE y = (i >> 4) & 0xF;

  if (Chip8::generalRegisters[x] == Chip8::generalRegisters[y]) 
    Chip8::iRegIncPcByTwo();

  return;
}

void Chip8::skipXNeqY(const WORD& i) {
  BYTE x = (i >> 8) & 0xF;
  BYTE y = (i >> 4) & 0xF;

  if (Chip8::generalRegisters[x] != Chip8::generalRegisters[y]) 
    Chip8::iRegIncPcByTwo();

  return;
}

void Chip8::set(const WORD& i) {
  std::cout << "set(" << std::hex << (int)i << ")" << std::endl;

  BYTE x = (i >> 8) & 0xF;
  BYTE nn = i & 0x0FF;

  Chip8::generalRegisters[x] = nn;
  
  std::cout << "Reg: " << std::hex << (int)x << ": "
    << std::hex << (int)nn << std::endl;

  return;
}

void Chip8::add(const WORD& i) {
  std::cout << "add(" << std::hex 
    << (int)i << ")" << std::endl;

  BYTE x = (i >> 8) & 0xF;
  BYTE nn = i & 0x0FF;

  Chip8::generalRegisters[x] += nn;
  
  std::cout << "Reg: " << std::hex << (int)x 
    << ": " << std::hex 
    << (int)generalRegisters[x] << std::endl;

  return;
}

void Chip8::clearScreen() {
  Chip8::displayHandler.clearScreen();

  return;
}

void Chip8::display(const WORD& i) {
  BYTE x = ( (i >> 8) & 0xF);
  BYTE y = ( (i >> 4) & 0xF);
  BYTE n = i & 0xF;
  
  std::cout << 
    "draw function: (" << 
    std::hex << (int)x << ", " << 
    std::hex << (int)y << ", " <<
    std::hex << (int)n << ", " << 
    std::hex << (int)Chip8::iReg << ")" << 
    std::endl;

  Chip8::displayHandler.draw(
    Chip8::generalRegisters[x] & 0x3F,
    Chip8::generalRegisters[y] & 0x1F,
    n,
    Chip8::iReg,
    Chip8::memory,
    Chip8::generalRegisters[0xF]
  );
}

void Chip8::start(const std::string& roomPath) {
  Chip8::readRoom(roomPath);
  Chip8::printMemory();
  std::chrono::microseconds ms = std::chrono::microseconds(1000);

  while (true) {
    std::cout << "PC value: " << std::hex << Chip8::pc << std::endl;
    if (!getKeyCycle) keyboard.getKey();
    WORD instruction = Chip8::fetchInstruction();
    Chip8::executeInstruction(instruction);
    Chip8::updateTimers();
    std::this_thread::sleep_for(ms);
  }
}

void Chip8::setIndex(const WORD& i) {
  std::cout << "setIndex(" << std::hex 
    << (int)i << ")" << std::endl;

  Chip8::iReg = i;
  
  std::cout << "iReg: " 
    << std::hex << (int)i 
    << std::endl;

  return;
}

void Chip8::printMemory() {
  for (int i = 0; i < Chip8::memory.size(); i++) {
    std::cout << std::hex << i << ": " << std::hex << (int)Chip8::memory[i] << std::endl;
  }

  return;
}

void Chip8::setFontCharacter(const WORD& x) {
  std::cout << "setFontCharacter(" << std::hex <<
    (int)x << ")" << std::endl;

  BYTE character = Chip8::generalRegisters[x];

  Chip8::iReg = 0x050 + (character * 5);

  return;
}

void Chip8::setXY(const WORD& i) {
  std::cout << "setXY(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
  std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;

  Chip8::generalRegisters[x] = Chip8::generalRegisters[y];
  
  std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;

  return;
}

void Chip8::binaryOr(const WORD& i) {
  std::cout << "binaryOr(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;


  Chip8::generalRegisters[x] |= Chip8::generalRegisters[y];
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  Chip8::generalRegisters[0xF] = 0x0;

  return;
}

void Chip8::binaryAnd(const WORD& i) {
 std::cout << "binaryAnd(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;

  Chip8::generalRegisters[x] &= Chip8::generalRegisters[y];
    
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  Chip8::generalRegisters[0xF] = 0x0;

  return;
}

void Chip8::logicalXor(const WORD& i) {
 std::cout << "logicalXor(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;


  Chip8::generalRegisters[x] ^= Chip8::generalRegisters[y];
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  Chip8::generalRegisters[0xF] = 0x0;

  return;
}

void Chip8::addWithPossibleOverflow(const WORD& i) {
 std::cout << "addWithPossibleOverflow(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
  std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;


  WORD sumResult = Chip8::generalRegisters[x] + Chip8::generalRegisters[y];
    
  Chip8::generalRegisters[x] = sumResult & 0xFF;
  
  if (sumResult > 0xFF) Chip8::generalRegisters[0xF] = 0x1;
  else Chip8::generalRegisters[0xF] = 0x0;

   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;

  return;
}

void Chip8::subtract(const WORD& i, const bool inverse) {
 std::cout << "subtract(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
  BYTE minuend = 0x0;
  BYTE subtrahend = 0x0;
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;

  if (!inverse) {
    minuend = Chip8::generalRegisters[x];
    subtrahend = Chip8::generalRegisters[y];
  }
  else {
    minuend = Chip8::generalRegisters[y];
    subtrahend = Chip8::generalRegisters[x];
  }
   
  Chip8::generalRegisters[x] = ( (minuend - subtrahend) % 256 + 256) % 256;
  
  if (minuend >= subtrahend) Chip8::generalRegisters[0xF] = 0x1;
  else Chip8::generalRegisters[0xF] = 0x0;

   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;

  return;
}

void Chip8::shift(const WORD& i, const bool isRight) {
 std::cout << "shift(" << 
    std::hex << (int)i << ")" <<
    std::endl;

  BYTE xy = (i >> 4) & 0xFF;
  BYTE x = (xy >> 4) & 0xF;
  BYTE y = xy & 0xF;
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;
  
  std::cout << "Reg : " << std::hex <<
    (int)y << ": " << std::hex <<
    (int)Chip8::generalRegisters[y]
    << std::endl;

  Chip8::generalRegisters[x] = Chip8::generalRegisters[y];
  
  BYTE out = 0x0;
  if (isRight) {
    out = Chip8::generalRegisters[x] & 0x1;

    Chip8::generalRegisters[x] >>= 1;
  }
  else {
    out = (Chip8::generalRegisters[x] >> 7) & 0x1;

    Chip8::generalRegisters[x] <<= 1;
  }
  Chip8::generalRegisters[0xF] = out;
  
   std::cout << "Reg : " << std::hex <<
    (int)x << ": " << std::hex <<
    (int)Chip8::generalRegisters[x]
    << std::endl;

  return;
}

void Chip8::addToIndex(const WORD& i) {
  Chip8::iReg += Chip8::generalRegisters[i];
  return;
}

void Chip8::binaryCodedDecimalConversion(const WORD& i) {
  BYTE num = Chip8::generalRegisters[i];

  Chip8::memory[Chip8::iReg] = num / 100; 
  Chip8::memory[Chip8::iReg + 1] = (num / 10) % 10;
  Chip8::memory[Chip8::iReg + 2] = num % 10;

  return;
}

void Chip8::storeMemory(const WORD& i) {
  for (WORD x = 0x0; x <= i; x++) {
    Chip8::memory[Chip8::iReg + x] = Chip8::generalRegisters[x];
  }

  Chip8::iReg += i + 1;

  return;
}

void Chip8::loadMemory(const WORD& i) {
  for (WORD x = 0x0; x <= i; x++) {
    Chip8::generalRegisters[x] = Chip8::memory[Chip8::iReg + x];
  }
  
  Chip8::iReg += i + 1;

  return;
}

void Chip8::getKey(const WORD& i) {
  std::cout << "getKey(" << std::hex <<
  (int)i << ")" << std::endl;

  BYTE key = Chip8::keyboard.getKey();

  if (key < 0xF) { 
    Chip8::generalRegisters[i] = key;
    Chip8::getKeyCycle = false;
  }
  else {
    Chip8::pc -= 2;
    Chip8::getKeyCycle = true;
  }
}

void Chip8::skipIfKey(const WORD& i, const bool inverse) {
  std::cout << "skipIfKey(" << std::hex <<
    (int)i << ", " << inverse << ")" << 
    std::endl;

  bool isPressed = Chip8::keyboard.isPressed(Chip8::generalRegisters[i]);

  if (!inverse) {
    if (isPressed) {
      Chip8::iRegIncPcByTwo();
    }
  }
  else {
    if (!isPressed) {
      Chip8::iRegIncPcByTwo();
    }
  }
}

void Chip8::updateTimers() {
  if (Chip8::delayTimer > 0) {
    Chip8::delayTimer--;
  }
  if (Chip8::soundTimer > 0) {
    Chip8::soundTimer--;
    // Play song
  }
  else {
    // Stop song
  }
}

void Chip8::setVXDelayTimer(const WORD& x) {
  Chip8::generalRegisters[x] = Chip8::delayTimer;
}

void Chip8::setDelayTimer(const WORD& x) {
  Chip8::delayTimer = Chip8::generalRegisters[x];
}

void Chip8::setSoundTimer(const WORD& x) {
  Chip8::soundTimer = Chip8::generalRegisters[x];
}

void Chip8::jmpWithOffset(const WORD& i) {
  // WORD highestNibble = (i >> 12) & 0xF;

  Chip8::pc = i + Chip8::generalRegisters[0];
}

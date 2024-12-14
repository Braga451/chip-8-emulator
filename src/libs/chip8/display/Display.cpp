#include "Display.hpp"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <cstdint>
#include <iostream>
#include <ostream>

Display::Display() : logicScreen(WINDOW_HEIGHT, std::vector<unsigned char>(WINDOW_WIDTH)){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(1920,
                              1080, 
                              0, 
                              &(Display::window), 
                              &(Display::renderer));
  SDL_RenderSetLogicalSize(
    Display::renderer,
    WINDOW_WIDTH,
    WINDOW_HEIGHT
  );
  SDL_SetRenderDrawColor(Display::renderer, 0, 0, 0, 255);
  SDL_RenderClear(Display::renderer);
  SDL_RenderPresent(Display::renderer);
  Display::texture = SDL_CreateTexture(
    Display::renderer,
    SDL_PIXELFORMAT_RGB888,
    SDL_TEXTUREACCESS_STREAMING,
    WINDOW_WIDTH,
    WINDOW_HEIGHT
  );
  
  if (Display::texture == nullptr) {
    std::cout << "Texture error: " << SDL_GetError() << std::endl;
    exit(1);
  }

  Display::clearScreen();
}

void Display::draw(const unsigned char& x, 
                const unsigned char& y,
                const unsigned char& n,
                const unsigned short& iPtr,
                const std::vector<unsigned char>& mem,
                unsigned char& vf) {

  vf = 0;
  unsigned char i = y;
  unsigned char j = x;
  
  std::cout << "(" << (int)j << ", " << (int)i << ")" << std::endl;

  for (int row = 0; row <= n; row++) {
    std::cout << "Sprite byte position: " << std::hex << iPtr + row << std::endl;
    unsigned char byte = mem[iPtr + row];
    j = x;
    std::cout << "Sprite byte: " << std::hex 
      << (int)byte << std::endl;

    for (int shift = 1; shift <= 8; shift++) {
      unsigned char bit = (byte >> (8 - shift)) & 0x0001;
      
      if (bit == 0x1 && logicScreen[i][j] == ON_COLOR) {
        logicScreen[i][j] = OFF_COLOR;
        vf = 1;
      }
      else if (bit == 0x1 && logicScreen[i][j] == OFF_COLOR) {
        logicScreen[i][j] = ON_COLOR;
      }


      if (j == WINDOW_WIDTH - 1) break;
      j++;
    }
    
    if (i == WINDOW_HEIGHT - 1) break;
    i++;

  }
  
  Display::draw();
}

void Display::clearScreen() {
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    for (int j = 0; j < WINDOW_WIDTH; j++) {
      logicScreen[i][j] = 0;
    }
  }
  
  Display::draw();
}

void Display::draw() {
  uint8_t * pixels;
  int pitch;
  SDL_LockTexture(
    Display::texture,
    nullptr,
    (void **)&pixels,
    &pitch
  );
  
  for (auto &y : logicScreen) {
    for (auto &x : y) {
      std::cout << (int)x;
    }
    std::cout << std::endl;
  }

  SDL_PixelFormat * pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    Uint32 *p = (Uint32 *)(pixels + pitch * i);

    for (int j = 0; j < WINDOW_WIDTH; j++) {

      if (logicScreen[i][j] == 0) {
        *p = SDL_MapRGB(pixelFormat, 0, 0, 0);
      }
      else {
        *p = SDL_MapRGB(pixelFormat, 255, 255, 255);
      }
      p++;
    }
  }

  SDL_UnlockTexture(Display::texture);
  SDL_RenderClear(Display::renderer);
  SDL_RenderCopy(
    Display::renderer,
    Display::texture,
    nullptr,
    nullptr
  );
  SDL_RenderPresent(Display::renderer);

}

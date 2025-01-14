#include "Keyboard.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <iostream>
#include <ostream>

Keyboard::Keyboard() : mappedKeys(16), pressedKeys(16, false), keysMapped{
    { SDLK_x, 0 },
    { SDLK_1, 1 },
    { SDLK_2, 2 },
    { SDLK_3, 3 },
    { SDLK_4, 0xc },
    { SDLK_q, 4 },
    { SDLK_w, 5 },
    { SDLK_e, 6 },
    { SDLK_r, 0xd },
    { SDLK_a, 7 },
    { SDLK_s, 8 },
    { SDLK_d, 9 },
    { SDLK_f, 0xe },
    { SDLK_z, 0xa },
    { SDLK_c, 0xb },
    { SDLK_v, 0xf }
}{
  Keyboard::mappedKeys[0] = SDLK_x;
  Keyboard::mappedKeys[1] = SDLK_1;
  Keyboard::mappedKeys[2] = SDLK_2;
  Keyboard::mappedKeys[3] = SDLK_3;
  Keyboard::mappedKeys[0xc] = SDLK_4;
  Keyboard::mappedKeys[4] = SDLK_q;
  Keyboard::mappedKeys[5] = SDLK_w;
  Keyboard::mappedKeys[6] = SDLK_e;
  Keyboard::mappedKeys[0xd] = SDLK_r;
  Keyboard::mappedKeys[7] = SDLK_a;
  Keyboard::mappedKeys[8] = SDLK_s;
  Keyboard::mappedKeys[9] = SDLK_d;
  Keyboard::mappedKeys[0xe] = SDLK_f;
  Keyboard::mappedKeys[0xa] = SDLK_z;
  Keyboard::mappedKeys[0xb] = SDLK_c;
  Keyboard::mappedKeys[0xf] = SDLK_v;
};

bool Keyboard::isPressed(unsigned char& key) {
  return Keyboard::pressedKeys[key];
}

unsigned char Keyboard::getKey() {
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_KEYDOWN) {
    if (Keyboard::keysMapped.count(event.key.keysym.sym) > 0) {
      Keyboard::pressedKeys[
        Keyboard::keysMapped[event.key.keysym.sym]
      ] = true;
    }
  }
  else if (event.type == SDL_KEYUP) {
    if (Keyboard::keysMapped.count(event.key.keysym.sym) > 0) {
       Keyboard::pressedKeys[
        Keyboard::keysMapped[event.key.keysym.sym]
      ] = false;
      return Keyboard::keysMapped[event.key.keysym.sym];
    }
  }
  
  return 0xFF;
}

void Keyboard::verifyKeyboard() {
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_KEYDOWN) {
    if (Keyboard::keysMapped.count(event.key.keysym.sym) > 0) {
      Keyboard::pressedKeys[
        Keyboard::keysMapped[event.key.keysym.sym]
      ] = true;
    }
  }
  else if (event.type == SDL_KEYUP) {
    if (Keyboard::keysMapped.count(event.key.keysym.sym) > 0) {
       Keyboard::pressedKeys[
        Keyboard::keysMapped[event.key.keysym.sym]
      ] = false;
    }
  }
}

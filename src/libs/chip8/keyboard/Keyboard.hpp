#ifndef KEYBOARD_H
  #define KEYBOARD_H
  #include <SDL2/SDL_events.h>
  #include <SDL2/SDL_keycode.h>
  #include <unordered_map>
  #include <vector>
class Keyboard {
    private:
      std::vector<SDL_Keycode> mappedKeys;
      std::vector<bool> pressedKeys;
      std::unordered_map<SDL_Keycode, int> keysMapped;

    public:
      void verifyKeyboard();
      bool isPressed(unsigned char& key);
      unsigned char getKey();
      Keyboard();
  };
#endif

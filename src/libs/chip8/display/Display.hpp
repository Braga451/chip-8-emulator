#ifndef CHIP_8_DISPLAY_H
  #define CHIP_8_DISPLAY_H
  #include <vector>
  #define WINDOW_WIDTH 64
  #define WINDOW_HEIGHT 32
  #define OFF_COLOR 0
  #define ON_COLOR 1
  #include <SDL2/SDL.h>
  class Display{
    private:
      SDL_Window * window;
      SDL_Renderer * renderer;
      SDL_Event event;
      SDL_Texture * texture; // The canvas
      std::vector<std::vector<unsigned char>> logicScreen;
      
      void draw();
      
    public:
      Display();
      ~Display() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
      }
      void draw(const unsigned char& x, 
                const unsigned char& y,
                const unsigned char& n,
                const unsigned short& iPtr,
                const std::vector<unsigned char>& mem,
                unsigned char& vf);
      void clearScreen();
  };
#endif


/*
  aedMouseCursor:
  A class to handle mouse Cursors 
  Initial design Rodrigo Hernandez,
  Idealy,it should remain completelly optional and somewhat independent from
  the whole library
*/

#ifndef AEDCURSOR_H
#define AEDCURSOR_H

#include "SDL.h"
#include "dllmacros.h"

class DLLEXPORT aedCursor
{
  public:
    aedCursor();
    ~aedCursor();
    inline void setAnchor(Uint8 x, Uint8 y)
    {
        xHot = x;
        yHot = y;
    };
    void setImage(SDL_Surface * Image);
    void render(SDL_Surface * Screen = NULL);

  private:
    Uint8 xHot;
    Uint8 yHot;
    SDL_Surface *CursorImage;
    SDL_Surface *SavedImage;
    SDL_Rect SavedRect;
};
#endif

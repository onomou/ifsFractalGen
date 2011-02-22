#include "aedCursor.h"

aedCursor::aedCursor()
{
    CursorImage = NULL;
    SavedImage = NULL;
    xHot = 0;
    yHot = 0;
};

aedCursor::~aedCursor()
{
    if(SavedImage != NULL)
        SDL_FreeSurface(SavedImage);
};

void
aedCursor::setImage(SDL_Surface * Image)
{
    if(SavedImage != NULL)
        SDL_FreeSurface(SavedImage);
    CursorImage = Image;
    SavedImage = SDL_CreateRGBSurface(CursorImage->flags,
                                      CursorImage->w,
                                      CursorImage->h,
                                      CursorImage->format->BitsPerPixel,
                                      CursorImage->format->Rmask,
                                      CursorImage->format->Gmask,
                                      CursorImage->format->Bmask,
                                      CursorImage->format->Amask);

};

void
aedCursor::render(SDL_Surface * Screen)
{
    SDL_Surface *Surface = Screen;
    int x, y;

    if(Surface == NULL)
        Surface = SDL_GetVideoSurface();
    SDL_BlitSurface(SavedImage, NULL, Surface, &SavedRect);
    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);
    SavedRect.w = CursorImage->w;
    SavedRect.h = CursorImage->h;
    SavedRect.x = x - xHot;
    SavedRect.y = y - yHot;
    SDL_BlitSurface(Surface, &SavedRect, SavedImage, NULL);
    SDL_BlitSurface(CursorImage, NULL, Surface, &SavedRect);
};

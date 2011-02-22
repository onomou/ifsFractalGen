#include "aedStaticImage.h"

void
aedStaticImage::setImage(aedImage img)
{
    m_Image = img;
}

void
aedStaticImage::render(void)
{
    SDL_Rect rect = { 0, 0, m_Surface->w, m_Surface->h };

    // If our image is indeed loaded...
    if(m_Image.getSurface() != NULL)
    {
        SDL_Surface *curSurf = m_Image.getSurface();
        SDL_Rect srcRect = { 0, 0, curSurf->w, curSurf->h };

        // The image is ALWAYS centered. I should include
        // some option to change this...

        // Width
        if(curSurf->w < rect.w)
        {
            rect.x = rect.x + (rect.w / 2 - curSurf->w / 2);
            rect.w = curSurf->w;
        }
        else
        {
            srcRect.x = curSurf->w / 2 - rect.w / 2;
            srcRect.w = rect.w;
        }

        // Height
        if(curSurf->h < rect.h)
        {
            rect.y = rect.y + (rect.h / 2 - curSurf->h / 2);
            rect.h = curSurf->h;
        }
        else
        {
            srcRect.y = curSurf->h / 2 - rect.h / 2;
            srcRect.h = rect.h;
        }

        SDL_SetAlpha(curSurf, 0, 0);
        // Time to blit everything
        SDL_BlitSurface(curSurf, &srcRect, m_Surface, &rect);
    }
}

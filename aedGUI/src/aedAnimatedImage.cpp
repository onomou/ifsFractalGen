#include "aedAnimatedImage.h"

aedAnimatedImage::aedAnimatedImage()
{
    m_CurrentFrame = 0;
    m_AutoAnimate = false;
    m_FPS = 0;
}

Uint16
aedAnimatedImage::getCurrentFrame(void)
{
    return m_CurrentFrame;
}

void
aedAnimatedImage::setCurrentFrame(Uint16 frm)
{
    m_CurrentFrame = frm;
}

Uint16
aedAnimatedImage::fastForward(void)
{
    m_CurrentFrame = m_Images.getSize() - 1;
    return m_CurrentFrame;
}

Uint16
aedAnimatedImage::rewind(void)
{
    m_CurrentFrame = 0;
    return m_CurrentFrame;
}

Uint16
aedAnimatedImage::previous(void)
{
    if(m_CurrentFrame > 0)
        m_CurrentFrame--;
    else
        fastForward();

    return m_CurrentFrame;
}

Uint16
aedAnimatedImage::next(void)
{
    if(m_CurrentFrame < m_Images.getSize())
        m_CurrentFrame++;
    else
        rewind();

    return m_CurrentFrame;
}

void
aedAnimatedImage::setImageBank(aedImageBank imgBank)
{
    m_Images = imgBank;
}

void
aedAnimatedImage::render(void)
{
    SDL_Rect rect = { 0, 0, m_Surface->w, m_Surface->h };

    // If our image is indeed loaded...
    if(m_Images.getImage(m_CurrentFrame).getSurface() != NULL)
    {
        SDL_Surface *curSurf = m_Images.getImage(m_CurrentFrame).getSurface();
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

        // Time to blit everything
        SDL_BlitSurface(curSurf, &srcRect, m_Surface, &rect);
    }

    if(m_AutoAnimate)
        next();
}

#include "aedApp.h"
#include "aedFont.h"

#include "SDL_ttf.h"
#ifndef HAVE_SDL_TTF
#include "SDL_ttf.c"
#endif

aedFont::aedFont()
{
    data = NULL;
}

aedFont::~aedFont()
{
    closeFont();
}

bool aedFont::openFont(const char *file, int pointsize)
{
    closeFont();
    data = TTF_OpenFont(file, pointsize);
    return (data != NULL);
}

bool aedFont::openFont(unsigned char *data, unsigned long int datasize,
                       int pointsize)
{
    closeFont();
    this->data = TTF_OpenFontRW(SDL_RWFromMem(data, datasize), 1, pointsize);
    return (this->data != NULL);
}

void
aedFont::closeFont()
{
    if(data)
    {
        TTF_CloseFont(data);
        data = NULL;
    }
}

int
aedFont::getStyle() const
{
    // The AED_FONT styles are the same as TTF styles
    return TTF_GetFontStyle(data);
}

void
aedFont::setStyle(int style)
{
    // The AED_FONT styles are the same as TTF styles
    TTF_SetFontStyle(data, style);
}


int
aedFont::getHeight() const
{
    return TTF_FontHeight(data);
}

int
aedFont::getAscent() const
{
    return TTF_FontAscent(data);
}

int
aedFont::getDescent() const
{
    return TTF_FontDescent(data);
}

int
aedFont::getLineSkip() const
{
    return TTF_FontLineSkip(data);
}

bool aedFont::getGlyphMetrics(Uint16 c, int *minx, int *maxx, int *miny,
                              int *maxy, int *advance)
{
    int
        status = TTF_GlyphMetrics(data, c, minx, maxx, miny, maxy, advance);

    return (status == 0);
}

int
aedFont::getTextSize(const char *text, Uint16 * w, Uint16 * h)
{
    int _w, _h;
    int status;

    if(pApp->getUTF8())
        status = TTF_SizeUTF8(data, text, &_w, &_h);
    else
        status = TTF_SizeText(data, text, &_w, &_h);
    *w = (Uint16) _w;
    *h = (Uint16) _h;
    return status;
}

SDL_Surface *
aedFont::renderTextSolid(const char *str, const aedColor & color)
{
    if(pApp->getUTF8())
        return TTF_RenderUTF8_Solid(data, str, color);
    else
        return TTF_RenderText_Solid(data, str, color);
}

SDL_Surface *
aedFont::renderTextShaded(const char *str, const aedColor & fg,
                          const aedColor & bg)
{
    if(pApp->getUTF8())
        return TTF_RenderUTF8_Shaded(data, str, fg, bg);
    else
        return TTF_RenderText_Shaded(data, str, fg, bg);
}

SDL_Surface *
aedFont::renderTextBlended(const char *str, const aedColor & color)
{
    if(pApp->getUTF8())
        return TTF_RenderUTF8_Blended(data, str, color);
    else
        return TTF_RenderText_Blended(data, str, color);
}

void
aedFont::renderTextSolid(SDL_Surface * s, int x, int y, const char *str,
                         const aedColor & color)
{
    SDL_Rect dst;
    SDL_Surface *tmp;

    tmp = renderTextSolid(str, color);
    if(tmp)
    {
        dst.x = x;
        dst.y = y;
        SDL_BlitSurface(tmp, NULL, s, &dst);
        SDL_FreeSurface(tmp);
    }
}

void
aedFont::renderTextShaded(SDL_Surface * s, int x, int y, const char *str,
                          const aedColor & fg, const aedColor & bg)
{
    SDL_Rect dst;
    SDL_Surface *tmp;

    tmp = renderTextShaded(str, fg, bg);
    if(tmp)
    {
        dst.x = x;
        dst.y = y;
        SDL_BlitSurface(tmp, NULL, s, &dst);
        SDL_FreeSurface(tmp);
    }
}

void
aedFont::renderTextBlended(SDL_Surface * s, int x, int y, const char *str,
                           const aedColor & color)
{
    SDL_Rect dst;
    SDL_Surface *tmp;

    tmp = renderTextBlended(str, color);
    if(tmp)
    {
        dst.x = x;
        dst.y = y;
        SDL_BlitSurface(tmp, NULL, s, &dst);
        SDL_FreeSurface(tmp);
    }
}

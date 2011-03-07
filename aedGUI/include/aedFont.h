
/*
 * The aedFont class
 * This class supplies an easy-to-use interface to FreeType
 * Initial design by Ivan Stankovic <pokemon@fly.srk.fer.hr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef AEDFONT_H
#define AEDFONT_H

#include "dllmacros.h"
#include "SDL.h"
#include "aedColor.h"

#ifdef _MSC_VER
#pragma warning(disable:4800)
#endif

#define AED_FONT_STYLE_NORMAL        0x00
#define AED_FONT_STYLE_BOLD          0x01
#define AED_FONT_STYLE_ITALIC        0x02
#define AED_FONT_STYLE_UNDERLINE     0x04

typedef struct _TTF_Font TTF_Font;

/** The aedFont class handles text rendering. */
class DLLEXPORT aedFont
{
  public:
    aedFont();
    virtual ~ aedFont();

    int getStyle() const;
    void setStyle(int style);

    int getHeight() const;
    int getAscent() const;
    int getDescent() const;
    int getLineSkip() const;

    bool getGlyphMetrics(Uint16 c, int *minx, int *maxx, int *miny, int *maxy,
                         int *advance);

    int getTextSize(const char *text, Uint16 * w, Uint16 * h);

    // These return rendered string as a surface which the caller must free
    SDL_Surface *renderTextSolid(const char *str, const aedColor & color);
    SDL_Surface *renderTextShaded(const char *str, const aedColor & fg,
                                  const aedColor & bg);
    SDL_Surface *renderTextBlended(const char *str, const aedColor & color);

    // These are just for convenience, render directly to the given surface
    void renderTextSolid(SDL_Surface * s, int x, int y, const char *str,
                         const aedColor & color);
    void renderTextShaded(SDL_Surface * s, int x, int y, const char *str,
                          const aedColor & fg, const aedColor & bg);
    void renderTextBlended(SDL_Surface * s, int x, int y, const char *str,
                           const aedColor & color);

  private:
      TTF_Font * data;

    // This isn't the best design, but it does what we need:
    // allow only aedApp to open and close fonts
    friend class aedApp;
    bool openFont(const char *file, int pointsize);
    bool openFont(unsigned char *data, unsigned long int datasize,
                  int pointsize);
    void closeFont();

  public:
    static Uint16 getUnicode(const char *utf8, int *advance)
    {
        int i = 0;
        Uint16 ch;

          ch = ((const unsigned char *) utf8)[i];
        if(ch >= 0xF0)
        {
            ch = (Uint16) (utf8[i] & 0x07) << 18;
            ch |= (Uint16) (utf8[++i] & 0x3F) << 12;
            ch |= (Uint16) (utf8[++i] & 0x3F) << 6;
            ch |= (Uint16) (utf8[++i] & 0x3F);
        }
        else if(ch >= 0xE0)
        {
            ch = (Uint16) (utf8[i] & 0x3F) << 12;
            ch |= (Uint16) (utf8[++i] & 0x3F) << 6;
            ch |= (Uint16) (utf8[++i] & 0x3F);
        }
        else if(ch >= 0xC0)
        {
            ch = (Uint16) (utf8[i] & 0x3F) << 6;
            ch |= (Uint16) (utf8[++i] & 0x3F);
        }
        *advance = (i + 1);
        return ch;
    }
};

#endif /* AEDFONT_H */

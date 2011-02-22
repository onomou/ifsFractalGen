
/*
 * The aedColor class
 * This class contains color values used by every widget
 * Initial design by Eduardo B. Fonseca <ebf@aedsolucoes.com.br>
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

#ifndef AEDCOLOR_H
#define AEDCOLOR_H

#include "dllmacros.h"
#include <iostream>
using namespace std;

class DLLEXPORT aedColor
{
  public:
    aedColor(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    aedColor()
    {
    }
    void setR(Uint8 c)
    {
        r = c;
    }
    void setG(Uint8 c)
    {
        g = c;
    }
    void setB(Uint8 c)
    {
        b = c;
    }
    void setA(Uint8 c)
    {
        a = c;
    }
    Uint8 getR(void)
    {
        return r;
    }
    Uint8 getG(void)
    {
        return g;
    }
    Uint8 getB(void)
    {
        return b;
    }
    Uint8 getA(void)
    {
        return a;
    }
    void dumpAll(void)
    {
        cout << "R: " << (int) r << endl;
        cout << "G: " << (int) g << endl;
        cout << "B: " << (int) b << endl;
        cout << "A: " << (int) a << endl;
    }
    void setAllColors(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    Uint32 getUint32(SDL_Surface * surface)
    {
        return SDL_MapRGBA(surface->format, r, g, b, a);
    }
    operator  SDL_Color() const
    {
        SDL_Color c;
          c.r = r;
          c.g = g;
          c.b = b;
          c.unused = a;
          return c;
    }
  private:
      Uint8 r, g, b, a;
};

#endif


/*
 * The aedImage class
 * This class is a simple wrapper around a SDL_Surface pointer
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

#ifndef AEDIMAGE_H
#define AEDIMAGE_H

#include "dllmacros.h"
#include "SDL.h"

class DLLEXPORT aedImage
{
  public:
    aedImage();
    aedImage(SDL_Surface * surf);
    virtual ~ aedImage();
    void setSurface(SDL_Surface * surf);
    SDL_Surface *getSurface(void);
    aedImage operator=(aedImage obj);
    void dumpAll(void);
  private:
      SDL_Surface * m_SurfacePointer;

};

#endif

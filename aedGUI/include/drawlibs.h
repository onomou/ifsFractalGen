
/*
 * drawlibs.h - code for manipulating SDL surfaces and drawing
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

#ifndef DRAWLIBS_H
#define DRAWLIBS_H

#include "SDL.h"

#define sgn(x) ((x<0)?-1:((x>0)?1:0))   /* macro to return the sign of a
                                           number */

DLLEXPORT SDL_Surface *CreateSurface(Uint32 flags, int width, int height,
                                     int bpp = 32);
DLLEXPORT void ConvertSurface(SDL_Surface ** surface);

DLLEXPORT void DrawPixel(SDL_Surface * screen, int x, int y, Uint32 color);
DLLEXPORT void DrawLine(SDL_Surface * screen, int x1, int y1, int x2, int y2,
                        Uint32 color);
DLLEXPORT void DrawRect(SDL_Surface * screen, int x1, int y1, int x2, int y2,
                        Uint32 color);
DLLEXPORT void DrawRound(SDL_Surface * screen, int x0, int y0, int w, int h,
                         int corner, Uint32 color);

DLLEXPORT void vertgradient(SDL_Surface * surf, SDL_Color c1, SDL_Color c2,
                            Uint8 alpha = 255);
DLLEXPORT void vertgradient2(SDL_Surface * surf, SDL_Rect & gradRect,
                             SDL_Color c1, SDL_Color c2, Uint8 alpha = 255);
DLLEXPORT void horizgradient2(SDL_Surface * surf, SDL_Rect & gradRect,
                              SDL_Color c1, SDL_Color c2, Uint8 alpha = 255);

DLLEXPORT Uint32 GetPixel(SDL_Surface * surface, int x, int y);
DLLEXPORT void FloodFill(SDL_Surface * screen, int x, int y, Uint32 c);
DLLEXPORT void DrawTriangle(SDL_Surface * s, int x[3], int y[3], Uint32 c);

enum
{
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT
};
DLLEXPORT void DrawArrow(SDL_Surface * s, int type, int x, int y, int a,
                         Uint32 color, bool fill = false, Uint32 fillcolor = 0);

#define SLOCK(surface) \
    do { \
        if(SDL_MUSTLOCK(surface)) \
	    SDL_LockSurface(surface); \
    } while(0)

#define SUNLOCK(surface) \
    do { \
        if(SDL_MUSTLOCK(surface)) \
	    SDL_UnlockSurface(surface); \
    } while(0)

#endif

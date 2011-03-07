
/*
 * The aedRect class
 * This class controls every widget position
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
#ifndef AEDRECT_H
#define AEDRECT_H

#include <iostream>
#include "dllmacros.h"
#include "SDL.h"

#define aedDEFAULTPOSITION aedRect(0,0,0,0)

class DLLEXPORT aedRect
{
  public:
    aedRect()
    {
        m_X = 0;
        m_Y = 0;
        m_W = 0;
        m_H = 0;
    }
    aedRect(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
    {
        m_X = x;
        m_Y = y;
        m_W = w;
        m_H = h;
    }
    aedRect(SDL_Rect & rect)
    {
        m_X = rect.x;
        m_Y = rect.y;
        m_W = rect.w;
        m_H = rect.h;
    }

    Sint32 getWidth(void)
    {
        return m_W;
    }
    Sint32 getHeight(void)
    {
        return m_H;
    }
    Sint32 getX(void)
    {
        return m_X;
    }
    Sint32 getY(void)
    {
        return m_Y;
    }
    Sint32 getX2(void)
    {
        return m_X + m_W;
    }
    Sint32 getY2(void)
    {
        return m_Y + m_H;
    }

    void setX(Sint32 X)
    {
        m_X = X;
    }
    void setY(Sint32 Y)
    {
        m_Y = Y;
    }
    void setWidth(Sint32 W)
    {
        m_W = W;
    }
    void setHeight(Sint32 H)
    {
        m_H = H;
    }
    void setRect(Sint32 X, Sint32 Y, Sint32 W, Sint32 H)
    {
        setX(X);
        setY(Y);
        setWidth(W);
        setHeight(H);
    }
    bool isPointIn(Sint32 x, Sint32 y)
    {
        if((x >= getX()) && (x <= getX2()) && (y >= getY()) && (y <= getY2()))
        {
            return true;
        }

        return false;
    }
    void dumpAll(void)
    {
        std::cout << "X: " << getX() << std::endl;
        std::cout << "Y: " << getY() << std::endl;
        std::cout << "W: " << getWidth() << std::endl;
        std::cout << "H: " << getHeight() << std::endl;
        std::cout << "--" << std::endl;
    }

    SDL_Rect getSDLRect(void)
    {
        SDL_Rect rect;

        rect.x = getX();
        rect.y = getY();
        rect.w = getWidth();
        rect.h = getHeight();
        return rect;
    }
    bool operator==(aedRect op2)
    {
        if((getX() == op2.getX()) && (getY() == op2.getY()) &&
           (getWidth() == op2.getWidth()) && (getHeight() == op2.getHeight()))
            return true;

        return false;
    }
    bool operator!=(aedRect op2)
    {
        if((getX() == op2.getX()) && (getY() == op2.getY()) &&
           (getWidth() == op2.getWidth()) && (getHeight() == op2.getHeight()))
            return false;

        return true;
    }

  private:
    Sint32 m_X, m_Y, m_W, m_H;
};

#endif /* AEDRECT_H */


/*
 * The aedAnimatedImage class
 * Simple widget for displaying little animations
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

#ifndef AEDANIMATEDIMAGE_H
#define AEDANIMATEDIMAGE_H

#include "aedWidget.h"
#include "aedImage.h"
#include "aedImageBank.h"

class aedAnimatedImage:public aedWidget
{
  public:
    aedAnimatedImage();
    virtual void render(void);

    void setImageBank(aedImageBank imgBank);
    Uint16 getCurrentFrame(void);
    void setCurrentFrame(Uint16 frm);
    Uint16 next(void);
    Uint16 previous(void);
    Uint16 rewind(void);
    Uint16 fastForward(void);

  private:
      aedImageBank m_Images;
    Uint16 m_CurrentFrame;
    bool m_AutoAnimate;
    Uint8 m_FPS;

};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedAnimatedImage >;
#endif

#endif


/*
 * The aedStaticImage class
 * Simple widget for displaying images
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

#ifndef AEDSTATICIMAGE_H
#define AEDSTATICIMAGE_H

#include "aedWidget.h"
#include "aedImage.h"

class DLLEXPORT aedStaticImage:public aedWidget
{
  public:
    void setImage(aedImage img);
    virtual void render(void);
  private:
      aedImage m_Image;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedStaticImage >;
#endif

#endif


/*
 * The aedProgressBar class
 * This class draws a progress bar on screen
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

#ifndef AEDPROGRESSBAR_H
#define AEDPROGRESSBAR_H

#include "aedWidget.h"
#include "aedStaticText.h"

/** This class implements a nice progress bar. */
class DLLEXPORT aedProgressBar:public aedWidget
{
  public:
    aedProgressBar();
    virtual ~ aedProgressBar();

    /** Set the value of the progress bar.
     * @param value some value in the range [0, 100] */
    void setValue(Uint8 value);

    /** Return the value of the progress bar. */
    Uint8 getValue(void) const
    {
        return m_PercentValue;
    }

    virtual void render(void);

  private:
      Uint8 m_PercentValue;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedProgressBar >;
#endif

#endif /* AEDPROGRESSBAR_H */

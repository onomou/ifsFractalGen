
/*
 * The aedScrollBar class
 * This class draws a scrollbar.
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
#ifndef AEDSCROLLBAR_H
#define AEDSCROLLBAR_H

#include "aedSlider.h"
#include "aedButton.h"
#include "aedSignal.h"

class DLLEXPORT aedScrollBar:public aedSlider
{
  public:
    aedScrollBar(aedOrientation orient = AED_VERTICAL);
    virtual ~ aedScrollBar();

    virtual void create(aedWidget * parent = NULL, aedRect pos =
                        aedDEFAULTPOSITION, std::string caption = "");
    virtual int wm_update(Uint32 msdelta);
    virtual int wm_lbuttonup(Uint16 x, Uint16 y);
    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual void render(void);

    aedRect getElevatorRect();

  private:
      aedButton m_ButtonUp, m_ButtonDown;
    void buttonUpClicked(void *caller, void *data);
    void buttonDownClicked(void *caller, void *data);
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedScrollBar >;
#endif

#endif /* AEDSCROLLBAR_H */

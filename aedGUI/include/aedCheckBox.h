
/*
 * The aedCheckBox class
 * This is the checkbox widget class
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

#ifndef AEDCHECKBOX_H
#define AEDCHECKBOX_H

#include "dllmacros.h"
#include "aedWidget.h"

/** The checkbox class. */
class DLLEXPORT aedCheckBox:public aedWidget
{
  public:
    aedCheckBox();
    virtual ~ aedCheckBox();

    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual void setCaption(const std::string & caption);
    virtual void render();
    virtual int wm_mouseenter();
    virtual int wm_mouseleave();

    /** Get checkbox state. */
    bool getState();

    /** Set checkbox state. */
    void setState(bool state);

  private:
      bool m_State;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedCheckBox >;
#endif

#endif /* AEDCHECKBOX_H */

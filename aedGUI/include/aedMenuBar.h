
/*
 * The aedMenuBar class
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
#ifndef AEDMENUBAR_H
#define AEDMENUBAR_H

#include "aedMenu.h"

/** The aedMenuBar class, used for creating simple menu bars,
 * obviously. */
class DLLEXPORT aedMenuBar:public aedWidget
{
  public:
    aedMenuBar();
    ~aedMenuBar();

    /** Attach a menu to a menu bar.
     * @param title title of the menu as displayed in the menu bar
     * @param menu pointer to the menu
     * @param pos position of the menu in the menu bar (-1 means end) 
     * @return index of the attached menu */
    unsigned int attachMenu(std::string title, aedMenu * menu, int pos = -1);

    /** Detach a menu from a menu bar.
     * @param index index of the menu to be removed */
    void detachMenu(unsigned int index);

    /** Detach a menu from a menu bar.
     * @param menu pointer to the menu to be removed */
    void detachMenu(aedMenu * menu);

    int wm_lbuttondown(Uint16 x, Uint16 y);
    int wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy);
    int wm_mouseleave();
    void render();

  private:
    int getIndexFromPixel(Uint16 x, Uint16 y, aedRect & rect);

  private:
    int m_Highlight, m_Active;
      std::vector < aedMenu * >m_Menus;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedMenuBar >;
#endif
#endif /* AEDMENUBAR_H */

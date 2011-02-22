
/*
 * The aedMenu class
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
#ifndef AEDMENU_H
#define AEDMENU_H

#include "aedWidget.h"

struct DLLEXPORT aedMenuItem
{
    std::string caption;
    class aedMenu *child;
};

/** This is a simple menu class. */
class DLLEXPORT aedMenu:public aedWidget
{
  public:
    aedMenu();
    ~aedMenu();

    /** Add an item to a menu.
     * @param caption text that will be displayed in the menu
     * @param child optional pointer to a submenu
     * @return index of the newly-added item in the menu
     */
    unsigned int addItem(const std::string & caption, aedMenu * child = NULL);

    /** Remove an item from a menu.
     * @param index index of the item you wish to remove from the menu */
    void removeItem(unsigned int index);

    int wm_lbuttondown(Uint16 x, Uint16 y);
    int wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy);
    int wm_mouseleave();
    int wm_mouseenter();
    void render();

  private:
    int getIndexFromPixel(Uint16 x, Uint16 y);

  private:
    int m_Highlight;
      std::vector < aedMenuItem > m_Items;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedMenu >;
#endif

#endif /* AEDMENU_H */

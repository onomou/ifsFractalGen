
/*
 * The aedRadioButtonGroup class
 * Implements radio buttons.
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

#ifndef AEDRADIOBUTTONGROUP_H
#define AEDRADIOBUTTONGROUP_H

#include "dllmacros.h"
#include "aedWidget.h"
#include "aedCheckBox.h"

/** Implements radio buttons (radio button groups, as we call them). */
class DLLEXPORT aedRadioButtonGroup:public aedWidget
{
  public:
    aedRadioButtonGroup();
    virtual ~ aedRadioButtonGroup();

    /** Add a button to the group.
     * @param caption caption of the button to add */
    void addButton(const std::string & caption);

    /** Remove a button from the group.
     * @param index index of the button in the group (first button has index 0)  */
    void removeButton(unsigned int index);

    /** Get the index of currently selected button. */
    int getSelectedButton() const
    {
        return m_Selected;
    }

    /** Select a button.
     * @param index index of the button to be selected */
    void selectButton(unsigned int index);

    int wm_lbuttondown(Uint16 x, Uint16 y);
    int wm_lostfocus();

  private:
    int m_Selected;
    Uint16 m_NextY;
      std::vector < aedCheckBox * >m_Buttons;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedRadioButtonGroup >;
#endif

#endif /* AEDRADIOBUTTONGROUP_H */

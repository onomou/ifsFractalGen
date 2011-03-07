
/*
 * The aedSpinner class
 * This class implements a spinner.
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
#ifndef AEDSPINNER_H
#define AEDSPINNER_H

#include "aedWidget.h"
#include "aedTextBox.h"
#include "aedButton.h"


class DLLEXPORT aedSpinner:public aedWidget
{
  public:
    aedSpinner();
    virtual void create(aedWidget * parent = NULL, aedRect pos =
                        aedDEFAULTPOSITION, std::string caption = "");

    int wm_update(Uint32 msdelta);
    void incrementValue();
    void decrementValue();

    Sint32 getValue() const
    {
        return m_Value;
    }
    void setValue(Sint32 val);

    void setRange(Sint32 minval, Sint32 maxval);
    void getRange(Sint32 * minval, Sint32 * maxval) const
    {
        if(minval)
            *minval = m_Min;
        if(maxval)
            *maxval = m_Max;
    }

  private:
      Sint32 m_Value, m_Min, m_Max;
    aedButton m_ButtonUp, m_ButtonDown;
    aedTextBox m_TextBox;

    void textbox_change(void *caller, void *data);
    void buttonUpClicked(void *caller, void *data);
    void buttonDownClicked(void *caller, void *data);
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedSpinner >;
#endif

#endif /* AEDSPINNER_H */

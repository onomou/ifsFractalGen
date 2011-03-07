
/*
 * The aedButton class
 * This is the base class for all button widgets
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

#ifndef AEDBUTTON_H
#define AEDBUTTON_H

#include "dllmacros.h"
#include "aedWidget.h"
#include "aedImage.h"
#include "aedStaticText.h"

enum aedButtonType
{
    ROUND,
    SQUARE,
    BUTTON_ARROW_UP,
    BUTTON_ARROW_DOWN,
    BUTTON_ARROW_LEFT,
    BUTTON_ARROW_RIGHT
};

/** The button class. Implements ordinary and toggled buttons.
 */
class DLLEXPORT aedButton:public aedWidget
{
  public:

    /** The constructor.
     * @param type type of the button you wish to create
     */
    aedButton(aedButtonType type = ROUND);
    virtual ~ aedButton();
    virtual void render(void);
    virtual int wm_update(Uint32 msdelta);
    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual int wm_lbuttonup(Uint16 x, Uint16 y);
    virtual int wm_keydown(aedKeyEvent & event);
    virtual int wm_mouseenter();
    virtual int wm_mouseleave();

    /** Set button's caption. */
    void setCaption(const std::string & caption);

    /** Set button's image.*/
    void setImage(aedImage image);

    /** Get button state (pressed or not). */
    bool getButtonState(void)
    {
        return m_ButtonDown;
    }

    /** Change button state (true means button is pressed). */
    void setButtonState(bool s);

    /** Set button type. */
    void setButtonType(aedButtonType type)
    {
        m_ButtonType = type;
    }

    /** Set toggle mode. */
    void setToggleMode(bool mode);

    /** Simulate a button down and button up event. */
    void click();

  protected:
    bool m_ButtonDown;
    bool m_ToggleMode;
    aedButtonType m_ButtonType;
    aedImage *m_Image;
    aedStaticText m_Label;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedButton >;
#endif

#endif

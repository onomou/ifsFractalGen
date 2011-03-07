
/*
 * The aedTextBox class
 * This class implements a textbox.
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
#ifndef AEDTEXTBOX_H
#define AEDTEXTBOX_H

#include "aedStaticText.h"

/** This class implements the textbox widget. */
class DLLEXPORT aedTextBox:public aedStaticText
{
  public:
    aedTextBox();
    virtual ~ aedTextBox();

    virtual int wm_update(Uint32 msdelta);
    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual int wm_lbuttonup(Uint16 x, Uint16 y);
    virtual int wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy);
    virtual int wm_mouseleave();
    virtual int wm_keydown(aedKeyEvent & event);
    virtual void render();

    /** Toggle textbox's read only property. */
    virtual void setReadOnly(bool state)
    {
        enable(!state);
    }
    virtual bool getReadOnly(void) const
    {
        return !isEnabled();
    }

    /** Set caret's blinking interval.
     * @param interval time interval in milliseconds (0 disables blinking) */
    virtual void setBlinkInterval(Uint32 interval)
    {
        m_BlinkInterval = interval;
    }

    bool getPasswordMode() const
    {
        return m_PasswordMode;
    }

    /** Enable/disable password mode. */
    void setPasswordMode(bool enable)
    {
        if(enable != m_PasswordMode)
        {
            m_PasswordMode = enable;
            setRenderState(true);
        }
    }

    /** Set the string of characters to be ignored when processing
     * keyboard events. */
    void setCharMask(std::string mask)
    {
        m_CharMask = mask;
    }
    std::string getCharMask()
    {
        return m_CharMask;
    }

    void setSelection(Uint32 start, Uint32 end)
    {
        // Clamp the selection to the size of the string
        if(start > m_Caption.size())
            start = m_Caption.size();
        if(end > m_Caption.size())
            end = m_Caption.size();

        m_SelectionStart = start;
        m_SelectionEnd = end;
    }

    void setSelectionStart(Uint32 start)
    {
        setSelection(m_SelectionStart, start);
    }

    void setSelectionEnd(Uint32 end)
    {
        setSelection(m_SelectionStart, end);
    }

    int calculateCaretPosition(Uint16 x, Uint16 y);

  protected:
    Uint16 m_CaretPos;
    bool m_Selecting;
    Uint32 m_SelectionStart, m_SelectionEnd;
    Uint32 m_BlinkInterval, m_TimeSinceLastUpdate;
    int m_StartPixel;
    bool m_ShowCaret;
    bool m_PasswordMode;

    std::string m_CharMask;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedTextBox >;
#endif

#endif


/*
 * The aedStaticText class
 * This class manages the (static) text drawing on screen
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

#ifndef AEDSTATICTEXT_H
#define AEDSTATICTEXT_H

#include "aedWidget.h"
#include "aedFont.h"

/** This class is used for displaying labels and is the base class for
 * the textbox widget. */
class DLLEXPORT aedStaticText:public aedWidget
{
  public:
    aedStaticText();
    virtual ~ aedStaticText();

    virtual void render(void);

    /** Set text justification.
     * @param just one of AED_JUST_LEFT, AED_JUST_CENTER, AED_JUST_RIGHT
     */
    virtual void setJustification(aedJusts just)
    {
        m_Justification = just;
        setRenderState(true);
    }

    /** Set font size. */
    void setFontSize(Uint8 size);
    Uint8 getFontSize(void) const
    {
        return m_FontSize;
    }

    /** Set text color. */
    void setTextColor(aedColor color)
    {
        m_TextColor = color;
        setRenderState(true);
    }

    /** Enable/disable bold text. */
    void setTextBold(bool bold);

    /** Enable/disable word wrapping. */
    void setWordWrap(bool state)
    {
        m_WordWrap = state;
        setRenderState(true);
    }

    int getPosAtPixel(int x, int y);
    virtual void setCaption(const std::string & caption);

  protected:
    void updateFont();

  protected:
    aedColor m_TextColor;
    aedJusts m_Justification;

  private:
    Uint8 m_FontSize;
    bool m_TextBold;
    bool m_WordWrap;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedStaticText >;
#endif

#endif

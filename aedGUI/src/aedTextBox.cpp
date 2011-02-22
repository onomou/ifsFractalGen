#include "aedTextBox.h"
#include "aedApp.h"
#include "aedTheme.h"

aedTextBox::aedTextBox()
{
    m_IsStatic = false;
    m_IsWidget = true;
    m_CanFocus = true;
    m_Caption = "";
    m_CharMask = "";
    m_CaretPos = 0;
    setSelection(0, 0);
    m_Selecting = false;
    m_StartPixel = 0;
    m_BlinkInterval = 600;
    m_PasswordMode = false;
    m_BorderColor = aedColor(255, 0, 0, 255);
    setBorder(AED_BORDER_ROUND);
}

aedTextBox::~aedTextBox()
{

}

int
aedTextBox::wm_update(Uint32 msdelta)
{
    if(hasFocus())
    {
        if(m_BlinkInterval > 0)
        {
            m_TimeSinceLastUpdate += msdelta;

            if(m_TimeSinceLastUpdate >= m_BlinkInterval)
            {
                m_TimeSinceLastUpdate = 0;
                m_ShowCaret = !m_ShowCaret;
                setRenderState(true);
            }
        }
        else
        {
            if(!m_ShowCaret)
            {
                m_ShowCaret = true;
                setRenderState(true);
            }
        }
    }
    else
    {
        if(m_ShowCaret)
        {
            m_ShowCaret = false;
            setRenderState(true);
        }
    }

    return 0;
}

int
aedTextBox::wm_lbuttondown(Uint16 x, Uint16 y)
{
    m_CaretPos = calculateCaretPosition(x, y);
    setSelection(m_CaretPos, m_CaretPos);
    m_Selecting = true;
    setRenderState(true);

    return (0);
}

int
aedTextBox::wm_lbuttonup(Uint16 x, Uint16 y)
{
    m_Selecting = false;
    setRenderState(true);

    return 0;
}

int
aedTextBox::wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy)
{
    if(m_Selecting == true)
    {
        int SelectionEnd = calculateCaretPosition(x, y);

        setSelectionEnd(SelectionEnd);
        setRenderState(true);
    }

    return 0;
}

int
aedTextBox::wm_mouseleave()
{
    m_Selecting = false;
    setRenderState(true);
    return 0;
}

int
aedTextBox::wm_keydown(aedKeyEvent & event)
{
    Uint16 key = event.unicode;

    for(Uint16 i = 0; i < m_CharMask.size(); i++)
    {
        if(m_CharMask[i] == key)
            return 0;
    }

    if(key == SDLK_TAB || key == SDLK_RETURN || key == SDLK_ESCAPE)
        return aedWidget::wm_keydown(event);

    Uint32 SelectStart = m_SelectionStart;
    Uint32 SelectEnd = m_SelectionEnd;

    if(SelectStart > SelectEnd)
        std::swap(SelectStart, SelectEnd);

    switch (event.sym)
    {
        case SDLK_LEFT:
            if(m_CaretPos > 0)
            {
                m_CaretPos--;
                m_ShowCaret = true;
            }
            break;

        case SDLK_RIGHT:
            if(m_CaretPos < m_Caption.size())
            {
                m_CaretPos++;
                m_ShowCaret = true;
            }
            break;

        case SDLK_HOME:
            m_CaretPos = 0;
            break;

        case SDLK_END:
            m_CaretPos = int (m_Caption.size());

            break;

        case SDLK_BACKSPACE:
            if(SelectStart != SelectEnd)
            {
                m_Caption.erase(m_Caption.begin() + SelectStart,
                                m_Caption.begin() + SelectEnd);
                m_CaretPos = SelectStart;
                setSelection(0, 0);
            }
            else if(m_Caption.size() > 0 && m_CaretPos > 0)
            {
                m_CaretPos--;
                m_Caption.erase(m_Caption.begin() + m_CaretPos);
            }
            break;

        case SDLK_DELETE:
            if(SelectStart != SelectEnd)
            {
                m_Caption.erase(m_Caption.begin() + SelectStart,
                                m_Caption.begin() + SelectEnd);
                m_CaretPos = SelectStart;
                setSelection(0, 0);
            }
            else if(m_Caption.size() > 0 && m_CaretPos <= m_Caption.size())
            {
                m_Caption.erase(m_Caption.begin() + m_CaretPos);
            }
            break;

        default:
            if(key)
            {
                if((key != SDLK_RSHIFT && key != SDLK_LSHIFT) &&
                   (SelectStart != SelectEnd))
                {
                    m_Caption.erase(m_Caption.begin() + SelectStart,
                                    m_Caption.begin() + SelectEnd);
                    m_CaretPos = SelectStart;
                    setSelection(0, 0);
                }
                m_Caption.insert(m_Caption.begin() + m_CaretPos, char (key));

                m_CaretPos++;
                m_ShowCaret = true;
            }
    }

    setRenderState(true);
    triggerEvent(TEXT_CHANGED, this, NULL);
    return (0);
}

void
aedTextBox::render(void)
{
    SDL_Surface *s1 = NULL;
    SDL_Rect src = {
        0, 0, 0, 0
    };
    SDL_Rect dest = {
        0, 0, m_Surface->w, m_Surface->h
    };
    std::string temp1, temp2, pass;
    Uint16 width1, height1, width2, height2;

    if(m_PasswordMode)
    {
        pass = m_Caption;
        m_Caption = std::string(m_Caption.size(), '*');
    }
    m_Theme->drawBox(this, m_Surface);
    if(m_CaretPos <= 0)
    {
        temp1 = "";
        temp2 = m_Caption;
    }
    else if(m_CaretPos < m_Caption.size())
    {
        temp1 = m_Caption.substr(0, m_CaretPos);
        temp2 = m_Caption.substr(m_CaretPos, m_Caption.size());
    }
    else
    {
        temp1 = m_Caption;
        temp2 = "";
    }
    m_Font->getTextSize(temp1.c_str(), &width1, &height1);
    m_Font->getTextSize(temp2.c_str(), &width2, &height2);

    aedColor thisColor = m_TextColor;

    if(!isEnabled())
        thisColor = aedColor(130, 130, 130, 255);

    thisColor.setA(this->getAlphaValue());
    s1 = m_Font->renderTextBlended(m_Caption.c_str(), thisColor);

    if(m_SelectionStart != m_SelectionEnd)
    {
        Uint16 SelectionStartX, SelectionWidth;
        Uint16 SelectionBoxHeight;

        Uint32 SelectStart = m_SelectionStart;
        Uint32 SelectEnd = m_SelectionEnd;

        if(SelectStart > SelectEnd)
            std::swap(SelectStart, SelectEnd);

        std::string BeforeSelection(m_Caption.substr(0, SelectStart));
        std::string SelectedText(m_Caption.
                                 substr(SelectStart, SelectEnd - SelectStart));

        m_Font->getTextSize(BeforeSelection.c_str(), &SelectionStartX,
                            &SelectionBoxHeight);
        m_Font->getTextSize(SelectedText.c_str(), &SelectionWidth,
                            &SelectionBoxHeight);

        aedColor SelectColor(0, 0, 200, 255);   // TODO: This needs to come from the theme

        SLOCK(s1);              // TODO: What should selected text look like?
        int x2 = SelectionStartX + SelectionWidth - 1;

        if(x2 >= s1->w)
            x2 = s1->w - 1;
        DrawRect(s1, SelectionStartX, 0, x2, SelectionBoxHeight - 1,
                 SelectColor.getUint32(s1));
        SUNLOCK(s1);
    }

    dest.x += cPad + 1;
    dest.y += cPad;
    dest.w -= cPad * 2 + 2;
    dest.h -= cPad * 3;

    if(s1)
    {
        if(width1 < m_StartPixel)
        {
            m_StartPixel = int (width1 - (dest.w / 4));

            if(m_StartPixel < 0)
                m_StartPixel = 0;
        }
        else if(width1 > m_StartPixel + dest.w)
        {
            m_StartPixel = int (width1 - dest.w + (dest.w / 4));

            if(m_StartPixel + dest.w > s1->w)
                m_StartPixel = s1->w - dest.w;
        }

        if(width1 + width2 < dest.w)
        {
            if(m_Justification == AED_JUST_RIGHT)
                dest.x += dest.w - (width1 + width2);
            else if(m_Justification == AED_JUST_CENTER)
                dest.x += (dest.w - (width1 + width2)) / 2;
        }

        src.x = m_StartPixel;
        src.y = 0;
        src.h = s1->h;
        src.w = dest.w;
        SDL_BlitSurface(s1, &src, m_Surface, &dest);
        SDL_FreeSurface(s1);
    }

    // caret
    if(hasFocus() && m_ShowCaret && isEnabled())
    {
        SLOCK(m_Surface);
        DrawLine(m_Surface,
                 dest.x + width1 - m_StartPixel, dest.y,
                 dest.x + width1 - m_StartPixel,
                 dest.y + dest.h, m_TextColor.getUint32(m_Surface));
        SUNLOCK(m_Surface);
    }

    if(m_PasswordMode)
        m_Caption = pass;
}

int
aedTextBox::calculateCaretPosition(Uint16 x, Uint16 y)
{
    aedRect myPos;
    Uint16 width, height;

    if(m_Caption.size() < 1)
        return 0;

    myPos = getMyPosition();
    myPos.setX(myPos.getX() + cPad + 1);
    myPos.setY(myPos.getY() + cPad);
    myPos.setWidth(myPos.getWidth() - (cPad * 2) + 2);
    myPos.setHeight(myPos.getHeight() - (cPad * 2));

    m_Font->getTextSize(m_Caption.c_str(), &width, &height);

    if(width < myPos.getWidth())
    {
        if(m_Justification == AED_JUST_RIGHT)
            myPos.setX(myPos.getX() + myPos.getWidth() - width);
        else if(m_Justification == AED_JUST_CENTER)
            myPos.setX(myPos.getX() + ((myPos.getWidth() - width) / 2));
    }

    return getPosAtPixel(x - myPos.getX(), y - myPos.getY()) - 1;
}

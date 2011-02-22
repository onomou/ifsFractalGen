#include "aedStaticText.h"
#include "aedApp.h"
#include "aedLog.h"
#include "aedTheme.h"

aedStaticText::aedStaticText()
{
    m_IsStatic = true;
    m_TextBold = false;
    m_WordWrap = false;
    m_Justification = AED_JUST_LEFT;
    m_FontSize = m_Theme->defaultFontSize;
    m_TextColor = m_Theme->defaultTextColor;
}

aedStaticText::~aedStaticText()
{
}

int
aedStaticText::getPosAtPixel(int x, int y)
{
// ignore y for now
    Uint16 charPos, width, height;

    std::string string;

    for(charPos = 1; charPos <= m_Caption.size(); charPos++)
    {
        string = m_Caption.substr(0, charPos);
        m_Font->getTextSize(string.c_str(), &width, &height);

        if(width > x)
            return (charPos);
    }

    if(x <= 0)
        return (0);
    else if(x > width)
        return (int (m_Caption.size()));

    return (0);
}

void
aedStaticText::setFontSize(Uint8 size)
{
    m_FontSize = size;
    updateFont();
}

void
aedStaticText::setTextBold(bool bold)
{
    m_TextBold = bold;
    updateFont();
}

void
aedStaticText::updateFont()
{
    m_Font =
        pApp->requestFont(m_Theme->defaultFontName, m_FontSize,
                          m_TextBold ? AED_FONT_STYLE_BOLD :
                          AED_FONT_STYLE_NORMAL);
    setRenderState(true);
}

void
aedStaticText::setCaption(const std::string & caption)
{
    m_Caption = caption;
    setRenderState(true);
}

void
aedStaticText::render(void)
{
    aedRect myPos(0, 0, m_Surface->w, m_Surface->h);
    SDL_Rect src, dest;

    std::string::size_type startPos = 0, endPos = 0;
    Uint16 line = 0, width = 0, height = 0;
    size_t x;

    std::string temp;
    SDL_Surface *s1 = NULL;

    SDL_Rect rect = myPos.getSDLRect();

    if((m_Caption.size() == 0) || (rect.w <= 4) || (rect.h <= 4))
        return;

    aedColor thisColor = m_TextColor;

    if(!isEnabled())
        thisColor = aedColor(130, 130, 130, 255);

    thisColor.setA(this->getAlphaValue());
    s1 = m_Font->renderTextBlended(m_Caption.c_str(), thisColor);

    src.x = 0;
    src.y = 0;
    src.w = myPos.getWidth();
    src.h = s1->h;

    dest.x = myPos.getX();
    dest.y = myPos.getY();
    dest.w = src.w;
    dest.h = src.h;

    // vertical alignment
    m_Font->getTextSize(m_Caption.c_str(), &width, &height);
    if(width <= myPos.getWidth())
        dest.y = myPos.getY() + int ((myPos.getHeight() - height) / 2);

    for(line = 1; endPos < m_Caption.size(); line++)
    {
        if(src.x + src.w > s1->w)
            src.w = s1->w - src.x;

        if(dest.y + dest.h > myPos.getY() + myPos.getHeight())
            break;

        endPos = getPosAtPixel(src.x + src.w, src.y) - 1;
        if(endPos < 1)
            break;

        temp = m_Caption.substr(startPos, endPos);

        if(m_WordWrap)
        {
            for(x = temp.size(); x > 0; x--)
            {
                if(temp.substr(x, 1) == " ")
                {
                    endPos = startPos + x + 1;
                    temp = temp.substr(startPos, endPos);
                    break;
                }
            }
        }

        m_Font->getTextSize(temp.c_str(), &width, &height);
        src.w = width;
        dest.w = src.w;

        if(m_Justification == AED_JUST_RIGHT)
            dest.x = myPos.getX() + (myPos.getWidth() - dest.w);
        else if(m_Justification == AED_JUST_CENTER)
            dest.x = myPos.getX() + int ((myPos.getWidth() - dest.w) / 2);

        SDL_SetAlpha(s1, 0, 0);
        SDL_BlitSurface(s1, &src, m_Surface, &dest);

        src.x += src.w;
        src.w = myPos.getWidth();
        dest.x = myPos.getX();
        dest.y += dest.h;
        startPos = endPos;
    }

    SDL_FreeSurface(s1);
}

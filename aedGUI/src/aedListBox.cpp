#include "aedListBox.h"
#include "aedTheme.h"
#include "aedApp.h"

aedListBox::aedListBox()
{
    m_IsWidget = true;
    m_currentLine = 0;
    setBorder(AED_BORDER_ROUND);
}

void
aedListBox::add(std::string string)
{
    m_Strings.push_back(string);
    m_ReRender = true;
}

void
aedListBox::insert(int line, std::string string)
{
    m_Strings.insert(m_Strings.begin() + line, string);
    m_ReRender = true;
}

void
aedListBox::del(int line)
{
    m_Strings.erase(m_Strings.begin() + line);
    m_ReRender = true;
}

void
aedListBox::del(int startLine, int stopLine)
{
    m_Strings.erase(m_Strings.begin() + startLine,
                    m_Strings.begin() + stopLine);
    m_ReRender = true;
}

int
aedListBox::size()
{
    return (int (m_Strings.size()));
}

int
aedListBox::wm_update(Uint32 msdelta)
{
    return (0);
}

void
aedListBox::render(void)
{
    aedRect myPos(0, 0, m_Surface->w, m_Surface->h);
    SDL_Surface *s1 = NULL;
    SDL_Rect src, dest;
    Uint16 line;

    myPos.setX(myPos.getX() + (cPad * 2));
    myPos.setY(myPos.getY() + cPad);
    myPos.setWidth(myPos.getWidth() - (2 * cPad * 2));
    myPos.setHeight(myPos.getHeight() - (2 * cPad));

    src.x = 0;
    src.y = 0;
    dest.x = myPos.getX();
    dest.y = myPos.getY();

    for(line = m_currentLine; line < m_Strings.size(); line++)
    {
        if(m_Strings[line] != "")
        {
            if(isEnabled())
            {
                aedColor thisColor = aedColor(0, 0, 0, 255);

                thisColor.setA(this->getAlphaValue());
                s1 = m_Font->renderTextBlended(m_Strings[line].c_str(),
                                               thisColor);
            }
            else
            {
                s1 = m_Font->renderTextBlended(m_Strings[line].c_str(),
                                               aedColor(130, 130,
                                                        130,
                                                        this->getAlphaValue()));
            }

            src.w = myPos.getWidth();
            src.h = s1->h;
            dest.w = src.w;
            dest.h = src.h;

            if(dest.y > myPos.getY() + myPos.getHeight())
            {
                break;
            }
            else if(myPos.getY() + myPos.getHeight() - dest.y < src.h)
            {
                src.h = myPos.getY() + myPos.getHeight() - dest.y;
            }

            if(src.w > s1->w)
            {
                src.w = s1->w;
            }

            SDL_BlitSurface(s1, &src, m_Surface, &dest);
            SDL_FreeSurface(s1);
        }

        dest.y += dest.h + 1;
    }

}

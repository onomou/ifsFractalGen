#include "aedFont.h"
#include "aedMenuBar.h"

#define PADDING 8

aedMenuBar::aedMenuBar()
{
    m_Highlight = -1;
    m_Active = -1;
    setBorder(AED_BORDER_ROUND);
}

aedMenuBar::~aedMenuBar()
{
}

unsigned int
aedMenuBar::attachMenu(std::string title, aedMenu * menu, int pos)
{
    menu->setCaption(title);
    menu->hide();
    m_Menus.push_back(menu);
    setRenderState(true);
    return (unsigned int) (m_Menus.size() - 1);
}

void
aedMenuBar::detachMenu(unsigned int index)
{
    if(index < m_Menus.size())
    {
        m_Menus.erase(m_Menus.begin() + index);
        setRenderState(true);
    }
}

void
aedMenuBar::detachMenu(aedMenu * menu)
{
    std::vector < aedMenu * >::iterator p;

    p = std::find(m_Menus.begin(), m_Menus.end(), menu);
    if(p != m_Menus.end())
    {
        m_Menus.erase(p);
        setRenderState(true);
    }
}

void
aedMenuBar::render()
{
    unsigned int i;
    SDL_Surface *s;
    SDL_Rect dst;

    // needs centering!
    dst.x = PADDING;
    dst.y = PADDING;
    for(i = 0; i < m_Menus.size(); i++)
    {
        aedColor color(0, 0, 0, 255);

        if(i == (unsigned) m_Highlight)
            color = aedColor(255, 255, 255, 255);

        s = m_Font->renderTextBlended(m_Menus[i]->getCaption().c_str(), color);
        dst.w = s->w;
        dst.h = s->h;
        if(i == (unsigned) m_Highlight)
        {
            SDL_Surface *bg = CreateSurface(SDL_SWSURFACE, s->w, s->h);

            SDL_FillRect(bg, NULL, SDL_MapRGBA(bg->format, 0, 0, 255, 255));
            SDL_SetAlpha(bg, SDL_SRCALPHA, 170);
            SDL_BlitSurface(bg, NULL, m_Surface, &dst);
            SDL_FreeSurface(bg);
        }

        SDL_BlitSurface(s, NULL, m_Surface, &dst);
        dst.x += s->w + PADDING;
        SDL_FreeSurface(s);
    }
    setRenderState(true);
}

int
aedMenuBar::getIndexFromPixel(Uint16 x, Uint16 y, aedRect & rect)
{
    aedRect r(PADDING, PADDING, 0, getPos().getHeight());
    Uint16 w, h;

    for(Uint16 i = 0; i < m_Menus.size(); i++)
    {
        m_Font->getTextSize(m_Menus[i]->getCaption().c_str(), &w, &h);
        r.setWidth(w);
        if(r.isPointIn(x, y))
        {
            rect = r;
            return i;
        }
        r.setX(r.getX() + w + PADDING);
    }

    return -1;
}

int
aedMenuBar::wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy)
{
    int i;
    aedRect r;

    i = getIndexFromPixel(x - getPos().getX(), y - getPos().getY(), r);
    if(m_Active != -1 && i != m_Active && i != m_Highlight && i != -1)
    {
        m_Menus[m_Active]->wm_mouseleave();
        m_Active = -1;
        wm_lbuttondown(x, y);
    }

    if(i != m_Highlight)
    {
        m_Highlight = i;
        setRenderState(true);
    }
    return 0;
}

int
aedMenuBar::wm_lbuttondown(Uint16 x, Uint16 y)
{
    int i;
    aedRect r;

    i = getIndexFromPixel(x - getRealPos().getX(), y - getRealPos().getY(), r);
    if(i != -1)
    {
//              getParent()->addWidget(m_Menus[i]);
//              m_Menus[i]->setPos(r.getX() + getRealPos().getX(), getRealPos().getY() + getPos().getHeight());

        this->addWidget(m_Menus[i]);
        m_Menus[i]->setPos(r.getX(), getPos().getHeight() - 2);
        m_Menus[i]->show();
        m_Active = i;
        setRenderState(true);
    }
    return 0;
}

int
aedMenuBar::wm_mouseleave()
{
    m_Highlight = -1;
    if(m_Active != -1 && !m_Menus[m_Active]->isMouseOver())
    {
        m_Menus[m_Active]->wm_mouseleave();
        m_Active = -1;
    }
    setRenderState(true);
    return 0;
}

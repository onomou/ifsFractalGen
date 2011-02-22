#include "aedMenu.h"
#include "aedUpdateManager.h"
#define SPACING 4

aedMenu::aedMenu()
{
    m_CanFocus = true;
    m_Highlight = -1;
    setBorder(AED_BORDER_ROUND);
}

aedMenu::~aedMenu()
{
}

unsigned int
aedMenu::addItem(const std::string & caption, aedMenu * child)
{
    aedMenuItem mi;

    mi.caption = caption;
    mi.child = child;
    m_Items.push_back(mi);
    setSize(100,
            int (m_Items.size()) * (m_Font->getHeight() + SPACING) + SPACING);
    return (unsigned int) (m_Items.size() - 1);
}

void
aedMenu::removeItem(unsigned int index)
{
    if(index < m_Items.size())
        m_Items.erase(m_Items.begin() + index);
}

void
aedMenu::render()
{
    SDL_Surface *s;
    SDL_Rect dst;

    dst.x = 4;
    dst.y = SPACING;
    size_t nItemsSize = m_Items.size();

    for(size_t i = 0; i < nItemsSize; ++i)
    {
        aedColor color(0, 0, 0, 255);

        if((int) i == m_Highlight)
            color = aedColor(255, 255, 255, 255);

        s = m_Font->renderTextBlended(m_Items[i].caption.c_str(), color);
        if((int) i == m_Highlight)
        {
            SDL_Surface *bg =
                CreateSurface(SDL_SWSURFACE, m_Surface->w - 8, s->h);
            SDL_FillRect(bg, NULL, SDL_MapRGBA(bg->format, 0, 0, 255, 255));
            SDL_SetAlpha(bg, SDL_SRCALPHA, 170);
            SDL_BlitSurface(bg, NULL, m_Surface, &dst);
            SDL_FreeSurface(bg);
        }

        SDL_BlitSurface(s, NULL, m_Surface, &dst);
        dst.y += s->h + SPACING;
        SDL_FreeSurface(s);
    }
}

int
aedMenu::getIndexFromPixel(Uint16 x, Uint16 y)
{
    y -= getRealPos().getY();
    return (y - SPACING) / (m_Font->getHeight() + SPACING);
}

int
aedMenu::wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy)
{
    int i = getIndexFromPixel(x, y);

    if(i != m_Highlight)
    {
        m_Highlight = i;
        setRenderState(true);
    }

    return 0;
}

int
aedMenu::wm_mouseleave()
{
    m_Highlight = -1;
    hide();
    if(getParent())
    {
        aedRect r = getRealPos();

        getParent()->wm_paint(SDL_GetVideoSurface(), r.getX(), r.getY(),
                              r.getWidth(), r.getHeight());
        pUpdateMgr->addRect(r);

        getParent()->removeWidget(this);
    }
    return 0;
}

int
aedMenu::wm_mouseenter()
{
    return 0;
}

int
aedMenu::wm_lbuttondown(Uint16 x, Uint16 y)
{
    return 0;
}

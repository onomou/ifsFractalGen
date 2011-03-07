#include <stack>
#include "aedWidget.h"
#include "aedUpdateManager.h"
#include "aedApp.h"
#include "drawlibs.h"
#include "rint.h"

// flags used in findChildAtCoord()
enum
{ NO_STATIC = 0x01 };

aedWidget::aedWidget()
{
    if(!pApp)
    {
        std::cerr << "You must first create an application object!\n";
        exit(1);
    }

    if(pApp->getRoot() == NULL)
    {
        std::cerr << "You must first create the root widget!\n";
        exit(1);
    }

    init();

    // inherit from root widget
    m_Theme = pApp->getRoot()->m_Theme;
    m_Font = pApp->getRoot()->m_Font;
    m_BGColor = m_Theme->colorDefaultWidgetBackground;
}

// This constructor is to be used only by the "root" Widget
aedWidget::
aedWidget(Uint16 screenWidth, Uint16 screenHeight, bool takeOverScreen)
{
    if(!pApp)
    {
        std::cerr << "You must first create the aedApp object!\n";
        exit(1);
    }

    init();
    pApp->setRoot(this);

    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    setPos(0, 0);
    setSize(m_ScreenWidth, m_ScreenHeight);

    // get the default theme
    m_Theme = pApp->getDefaultTheme();
    m_BGColor = m_Theme->colorDefaultWidgetBackground;

    std::string tmp = pApp->getDefaultFontName();
    if(!tmp.empty())
        m_Theme->defaultFontName = tmp;

    m_Font =
        pApp->requestFont(m_Theme->defaultFontName, m_Theme->defaultFontSize);
}

aedWidget::~aedWidget()
{
    if(pApp->getFocusWidget() == this)
        pApp->setFocusWidget(NULL);

    if(m_Parent)
        m_Parent->removeWidget(this);

    if(m_Surface)
        SDL_FreeSurface(m_Surface);
}

void
aedWidget::create(aedWidget * parent, aedRect pos, std::string caption)
{
    setCaption(caption);

    if(parent != NULL)
        parent->addWidget(this);

    setPos(pos.getX(), pos.getY());
    setSize(pos.getWidth(), pos.getHeight());
}

void
aedWidget::init(void)
{
    m_Id = "";
    m_userdata = NULL;
    m_BGColor = aedColor(255, 255, 255, 0);
    m_ReRender = true;
    m_ShowWidget = true;
    m_IsEnabled = true;
    m_IsStatic = false;
    m_IsWidget = false;
    m_CanFocus = false;
    m_HasActiveBorder = false;
    m_Parent = NULL;
    m_Surface = NULL;
    m_Flags = 0;
    m_ScreenWidth = 0;
    m_ScreenHeight = 0;
    m_CurTicks = 0;
    m_LastTicks = 0;
    m_Caption = "";
    m_AlphaValue = 255;
    m_Border = AED_BORDER_NONE;
    m_Theme = NULL;
    m_Font = NULL;
}

void
aedWidget::setId(const std::string & id)
{
    if(id.empty())
        return;

    if(pApp->getRoot()->findWidget(id) == NULL)
        m_Id = id;
    else
        pLog->warning("widget ID already exists: '%s'\n", id.c_str());
}

void
aedWidget::setPos(Sint16 x, Sint16 y)
{
    m_Pos.setX(x);
    m_Pos.setY(y);
}

void
aedWidget::update(void)
{
    m_CurTicks = SDL_GetTicks();
    updateAllChildren(m_CurTicks - m_LastTicks);
    m_LastTicks = m_CurTicks;
}

void
aedWidget::updateAllChildren(Uint32 msdelta)
{
    wm_update(msdelta);

    for(Uint16 i = 0; i < m_Children.size(); i++)
        m_Children[i]->updateAllChildren(msdelta);
}

bool aedWidget::tabFocus()
{
    if(!isEnabled())
    {
        return false;
    }
    if(m_CanFocus)
    {
        pApp->setFocusWidget(this);
        return true;
    }
    return tabFocusChildren();
}

bool aedWidget::tabFocusChildren()
{
    std::vector < aedWidget * >::iterator p = m_Children.begin();
    while(p != m_Children.end())
    {
        if((*p)->tabFocus())
        {
            return true;
        }
        ++p;
    }
    return false;
}

void
aedWidget::tabFocusParent()
{
    if(m_Parent)
    {
        std::vector < aedWidget * >::iterator p =
            std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(),
                      this);
        ++p;
        while(p != m_Parent->m_Children.end())
        {
            if((*p)->tabFocus())
            {
                return;
            }
            ++p;
        }
        m_Parent->tabFocusParent();
        return;
    }
    tabFocusChildren();
}

void
aedWidget::setSize(Sint16 w, Sint16 h)
{
    m_Pos.setWidth(w);
    m_Pos.setHeight(h);

    if(m_Surface)
    {
        SDL_FreeSurface(m_Surface);
        m_Surface = NULL;
    }
}

void
aedWidget::setTheme(aedTheme * theme)
{
    Uint16 i;

    m_Theme = theme;
    setRenderState(true);

    for(i = 0; i < m_Children.size(); i++)
        m_Children[i]->setTheme(theme);
}

aedWidget *
aedWidget::findWidget(const std::string & id)
{
    std::stack < aedWidget * >s;
    Uint32 i;
    aedWidget *p;

    if(m_Id == id)
        return this;

    for(i = 0; i < m_Children.size(); i++)
        s.push(m_Children[i]);

    while(!s.empty())
    {
        p = s.top();
        if(p->m_Id == id)
            return p;

        s.pop();
        for(i = 0; i < p->m_Children.size(); i++)
            s.push(p->m_Children[i]);
    }

    return NULL;
}

aedWidget *
aedWidget::findWidget(aedWidget * pointer)
{
    std::stack < aedWidget * >s;
    Uint32 i;
    aedWidget *p;

    if(pointer == this)
        return this;

    for(i = 0; i < m_Children.size(); i++)
        s.push(m_Children[i]);

    while(!s.empty())
    {
        p = s.top();
        if(p == pointer)
            return p;

        s.pop();
        for(i = 0; i < p->m_Children.size(); i++)
            s.push(p->m_Children[i]);
    }

    return NULL;
}

bool
aedWidget::addWidget(aedWidget * win)
{
    if(!win || win == this)
        return false;

    std::vector < aedWidget * >::iterator p =
        std::find(m_Children.begin(), m_Children.end(), win);

    if(p == m_Children.end())
    {
        m_Children.push_back(win);
        win->m_Parent = this;

        if(win->m_BGColor.getA() == 0)  // transparency check
            win->m_BGColor = m_BGColor;

        win->setRenderState(true);
        return true;
    }

    return false;
}

bool
aedWidget::removeWidget(aedWidget * win)
{
    std::vector < aedWidget * >::iterator p =
        std::find(m_Children.begin(), m_Children.end(), win);

    if(p != m_Children.end())
    {
        m_Children.erase(p);
        win->m_Parent = NULL;
        return true;
    }
    return false;
}

void
aedWidget::bringToTop(void)
{
    if(m_Parent == NULL)
        return;

    /* We need to save parent's pointer here because
     * removeWindget() will set our m_Parent to NULL. */
    aedWidget *parent = m_Parent;

    parent->removeWidget(this);
    parent->addWidget(this);
    parent->bringToTop();
}

bool
aedWidget::isMouseOver()
{
    int x, y;

    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);

    return getRealPos().isPointIn(x, y);
}

bool
aedWidget::hasFocus(void) const
{
    return pApp->getFocusWidget() == this;
}

Uint16
aedWidget::clientxToScreen(Uint16 x)
{
    if(m_Parent == NULL)
        return 0;

    Uint16 posicao_relativa = m_Parent->getRealPos().getX();
    aedRect myPositionOffset = m_Parent->getPositionOffset();

    return posicao_relativa + x + myPositionOffset.getX();
}

Uint16
aedWidget::clientyToScreen(Uint16 y)
{
    if(m_Parent == NULL)
        return 0;

    Uint16 posicao_relativa = m_Parent->getRealPos().getY();
    aedRect myPositionOffset = m_Parent->getPositionOffset();

    return posicao_relativa + y + myPositionOffset.getY();
}

Sint32
aedWidget::clientwToScreen(Sint32 w)
{
    if(m_Parent == NULL)
        return 0;

    aedRect myPositionOffset = m_Parent->getPositionOffset();

    return w + myPositionOffset.getWidth();
}

Sint32
aedWidget::clienthToScreen(Sint32 h)
{
    if(m_Parent == NULL)
        return 0;

    aedRect myPositionOffset = m_Parent->getPositionOffset();

    return h + myPositionOffset.getHeight();
}


void
aedWidget::screenToClient(Uint16 & x, Uint16 & y)
{
    Uint16 offsetx = clientxToScreen();
    Uint16 offsety = clientyToScreen();

    x = x - offsetx + m_PositionOffset.getX();
    y = y - offsety + m_PositionOffset.getY();
}

aedWidget *
aedWidget::findChildAtCoord(Uint16 x, Uint16 y, Uint8 flags)
{
    int i;
    int nChildrenSize = int (m_Children.size() - 1);

    for(i = nChildrenSize; i >= 0; --i)
    {
        aedWidget *found = m_Children[i]->findChildAtCoord(x, y, flags);

        if(found)
        {
            if(flags & NO_STATIC)
            {
                if(!found->m_IsStatic && found->isEnabled())
                    return found;
            }
            else
                return found;
        }

    }

    if(isVisible() && getRealPos().isPointIn(x, y))
        return this;

    return NULL;
}

void
aedWidget::setAlphaValue(Uint8 alpha)
{
    m_AlphaValue = alpha;
    setRenderState(true);

    for(Uint16 i = 0; i < m_Children.size(); i++)
        m_Children[i]->setAlphaValue(alpha);
}

void
aedWidget::makeAllDirty(void)
{
    m_ReRender = true;

    for(Uint16 i = 0; i < m_Children.size(); i++)
        m_Children[i]->makeAllDirty();
}

void
aedWidget::render()
{
    /* Nothing */
}

void
aedWidget::renderAll(SDL_Surface * screen)
{
    bool force = false;
    SDL_Rect rect = getMyPosition().getSDLRect();

    if(!isVisible())
        return;

    if(!screen)
        screen = SDL_GetVideoSurface();

    if(m_ReRender)
    {
        if(!m_Surface)
            m_Surface = CreateSurface(SDL_SWSURFACE, rect.w, rect.h);

        //SDL_FillRect(m_Surface, NULL, SDL_MapRGBA(m_Surface->format, 0,0,0,0));
        if((rect.w > 0) && (rect.h > 0))
            SDL_FillRect(m_Surface, NULL, m_BGColor.getUint32(m_Surface));

        render();

        if(m_Border != AED_BORDER_NONE)
            m_Theme->drawBorder(this, m_Surface);

        // we only update the parent if it hasn't been re-rendered again
        if(m_Parent)
            if(!getParent()->getRenderState())
                getParent()->wm_paint(screen, rect.x, rect.y, rect.w, rect.h);

        SDL_BlitSurface(m_Surface, NULL, screen, &rect);
        pUpdateMgr->addRect(rect);

        force = true;
    }

    for(Uint16 i = 0; i < m_Children.size(); i++)
    {
        if(force)
            m_Children[i]->setRenderState(true);
        m_Children[i]->renderAll(screen);
    }

    m_ReRender = false;

    // this relies on the fact that the user will
    // always call renderAll on the root widget
    if(pApp->getRoot() == this)
        pUpdateMgr->updateRects(screen);
}

aedRect aedWidget::getPosition(void)
{
    return getMyPosition();
}

aedRect
aedWidget::getMyTranslatedPosition(void)
{
    aedRect myPosition = getPos();
    aedWidget *parent = m_Parent;

    if(NULL != parent)
    {
        if(myPosition.getX() == CENTERX)
            myPosition.setX((parent->getRealPos().getWidth() / 2) -
                            (myPosition.getWidth() / 2));

        if(myPosition.getY() == CENTERY)
            myPosition.setY((parent->getRealPos().getHeight() / 2) -
                            (myPosition.getHeight() / 2));

        if(myPosition.getX() == FULLX)
            myPosition.setX((parent->getRealPos().getWidth()) -
                            (myPosition.getWidth()));

        if(myPosition.getY() == FULLY)
            myPosition.setY((parent->getRealPos().getHeight()) -
                            (myPosition.getHeight()));

        if(myPosition.getWidth() == TAKEOVERX)
            myPosition.setWidth(parent->getRealPos().getWidth() -
                                myPosition.getX() -
                                parent->getPositionOffset().getX() * 2);

        if(myPosition.getHeight() == TAKEOVERY)
            myPosition.setHeight(parent->getRealPos().getHeight() -
                                 myPosition.getY() -
                                 parent->getPositionOffset().getY() * 2);
    }

    m_PixelPos = myPosition;

    return myPosition;
}

aedRect
aedWidget::getMyPosition(void)
{
    aedRect myPosition = getMyTranslatedPosition();

    myPosition.setX(clientxToScreen(myPosition.getX()));
    myPosition.setY(clientyToScreen(myPosition.getY()));

    // have to rethink this:
//    myPosition.setWidth(clientwToScreen(myPosition.getWidth()));
//    myPosition.setHeight(clienthToScreen(myPosition.getHeight()));

    return myPosition;
}

aedRect
aedWidget::getPos(void)
{
    return m_Pos;
}

aedRect
aedWidget::getPositionOffset(void)
{
    return m_PositionOffset;
}

///////////////////////////////////////////////////////////////////

aedWidget *
aedWidget::getTopMostWidget(void)
{
    aedWidget *win = this;

    while(win != NULL && !win->m_IsWidget)
    {
        if(win->m_Children.size() > 0)
            win = win->m_Children[win->m_Children.size() - 1];
        else
            win = NULL;
    }

    if(win == NULL)
        return this;

    return win;
}

/* The messages */

int
aedWidget::wm_keydown(aedKeyEvent & event)
{
    // Default processing of tab switches widget focus
    if(event.sym == SDLK_TAB)
    {
        if(!tabFocusChildren())
            tabFocusParent();
    }
    if(event.sym == SDLK_RETURN)
    {
        pApp->clickOkayButton();
    }
    if(event.sym == SDLK_ESCAPE)
    {
        pApp->clickCancelButton();
    }
    return 0;
}

int
aedWidget::wm_lbuttondown(Uint16 x, Uint16 y)
{
    m_isLPressed = true;
    return 0;
}

int
aedWidget::wm_lbuttonup(Uint16 x, Uint16 y)
{
    m_isLPressed = false;
    return 0;
}

int
aedWidget::wm_rbuttondown(Uint16 x, Uint16 y)
{
    m_isRPressed = true;
    return 0;
}

int
aedWidget::wm_rbuttonup(Uint16 x, Uint16 y)
{
    m_isRPressed = true;
    return 0;
}

int
aedWidget::wm_sizechanged(Uint16 screenWidth, Uint16 screenHeight)
{
    if(NULL == m_Parent)        // we are the root widget
    {
        m_ScreenWidth = screenWidth - 1;
        m_ScreenHeight = screenHeight - 1;
        setSize(m_ScreenWidth, m_ScreenHeight);
    }

    aedRect pos(0, 0, screenWidth, screenHeight);

    triggerEvent(SCREEN_RESOLUTION_CHANGED, this, (void *) &pos);

    for(Uint16 i = 0; i < m_Children.size(); i++)
        m_Children[i]->wm_sizechanged(screenWidth, screenHeight);

    return 0;
}

int
aedWidget::wm_update(Uint32 msdelta)
{
    return 0;
}

int
aedWidget::wm_paint(SDL_Surface * screen, Uint16 x, Uint16 y, Uint16 w,
                    Uint16 h)
{
    if(m_Parent)
        m_Parent->wm_paint(screen, x, y, w, h);

    SDL_Rect rect = { x, y, w, h };
    SDL_Rect destRect = { x, y, w, h };

    if(m_Surface)
    {
        aedRect myPos = getMyPosition();

        rect.x = rect.x - myPos.getX();
        rect.y = rect.y - myPos.getY();
        SDL_BlitSurface(m_Surface, &rect, screen, &destRect);
    }
    return 0;
}

// Event Handling Functions
bool aedWidget::pollEvent(SDL_Event * event)
{
    bool
        bIsEventWaiting = SDL_PollEvent(event);

    if(bIsEventWaiting)
    {
        processEvent(event);
    }
    return bIsEventWaiting;
};

void
aedWidget::processEvent(SDL_Event * event)
{
    static aedWidget *last = NULL;
    aedWidget *child;
    aedKeyEvent keyevent;
    aedWidget *pFocused = pApp->getFocusWidget();

    switch (event->type)
    {
        case SDL_KEYDOWN:
            keyevent.sym = event->key.keysym.sym;
            keyevent.unicode = event->key.keysym.unicode;
            if(pFocused != NULL && pFocused->isEnabled())
            {
                triggerEvent(KEY_DOWN, pFocused, NULL);
                pFocused->wm_keydown(keyevent);
            }
            break;

        case SDL_MOUSEMOTION:
            if(pFocused != NULL && pFocused->isEnabled() &&
               (pFocused->isLPressed() || pFocused->isRPressed()))
            {
                child = pFocused;
            }
            else
            {
                child = findChildAtCoord(event->motion.x, event->motion.y,
                                         NO_STATIC);
            }

            last = last ? pApp->getRoot()->findWidget(last) : NULL;

            if(last &&
               !last->getRealPos().isPointIn(event->motion.x, event->motion.y))
                last->wm_mouseleave();
            last = NULL;
            if(child != NULL && child->isEnabled())
            {
                if(child != last)
                    child->wm_mouseenter();

                last = child;
                child->wm_mousemove(event->motion.x, event->motion.y,
                                    event->motion.xrel, event->motion.yrel);

                aedRect pos(event->motion.x, event->motion.y, 0, 0);

                child->triggerEvent(MOUSE_MOVE, child, (void *) &pos);
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            child = findChildAtCoord(event->button.x, event->button.y,
                                     NO_STATIC);

            // Note: after these two lines, pApp->getFocusWidget() may return
            // NULL or child
            if(child != pApp->getFocusWidget())
                pApp->setFocusWidget(child);

            pFocused = pApp->getFocusWidget();
            if(pFocused != NULL && pFocused->isEnabled())
            {
                if(event->button.button == SDL_BUTTON_LEFT)
                {
                    pFocused->wm_lbuttondown(event->button.x, event->button.y);
                    pFocused->triggerEvent(MOUSE_LBUTTON_DOWN, pFocused, NULL);
                }
                else if(event->button.button == SDL_BUTTON_RIGHT)
                {
                    pFocused->wm_rbuttondown(event->button.x, event->button.y);
                    pFocused->triggerEvent(MOUSE_RBUTTON_DOWN, pFocused, NULL);
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:

            if(pFocused != NULL && pFocused->isEnabled())
            {
                if(event->button.button == SDL_BUTTON_LEFT)
                {
                    pFocused->wm_lbuttonup(event->button.x, event->button.y);
                    pFocused->triggerEvent(MOUSE_LBUTTON_UP, pFocused, NULL);
                }
                else if(event->button.button == SDL_BUTTON_RIGHT)
                {
                    pFocused->wm_rbuttonup(event->button.x, event->button.y);
                    pFocused->triggerEvent(MOUSE_RBUTTON_UP, pFocused, NULL);
                }
            }
            break;

        case SDL_VIDEOEXPOSE:
            makeAllDirty();
            break;

        case SDL_ACTIVEEVENT:
            if(event->active.state & (SDL_APPACTIVE | SDL_APPINPUTFOCUS))
            {
                if(event->active.gain == 1)
                    makeAllDirty();
            }
            break;

        case SDL_VIDEORESIZE:
            {
                Uint16 width = event->resize.w;
                Uint16 height = event->resize.h;

                SDL_Surface *screen = SDL_GetVideoSurface();

                screen =
                    SDL_SetVideoMode(width, height,
                                     screen->format->BitsPerPixel,
                                     screen->flags);
                if(screen == NULL)
                    pLog->fatal("SDL_SetVideoMode failed: %s\n",
                                SDL_GetError());

                wm_sizechanged(width, height);
                makeAllDirty();
            }
            break;
    }
    /*
       // This is not working now, it does but causes all widgets to process
       // Messages
       for(size_t i = 0;i<ChildrenCount;++i)
       {
       m_Children[i]->processEvent(event);
       }
     */
};

#if !defined(HAVE_RINT)
double
rint(double x)
{
    double f, n = 0.;

    f = modf(x, &n);

    if(x > 0.)
    {
        if(f > .5)
            n += 1.;
    }
    else if(x < 0.)
    {
        if(f < -.5)
            n -= 1.;
    }
    return n;
}
#endif

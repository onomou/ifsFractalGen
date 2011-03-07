#include "aedButton.h"
#include "aedTheme.h"
#include "drawlibs.h"

aedButton::aedButton(aedButtonType type)
{
    m_ToggleMode = false;
    m_ButtonDown = false;
    m_IsWidget = true;
    m_CanFocus = true;
    m_ButtonType = type;
    m_Image = NULL;
    m_Label.create(this, aedRect(0, 0, 0, 0));
}

aedButton::~aedButton()
{
    if(this == pApp->getOkayButton())
        pApp->setOkayButton(NULL);
    if(this == pApp->getCancelButton())
        pApp->setCancelButton(NULL);
}

void
aedButton::setButtonState(bool s)
{
    m_ButtonDown = s;
    setRenderState(true);
}

void
aedButton::setToggleMode(bool mode)
{
    m_ToggleMode = mode;
}

void
aedButton::click()
{
    wm_lbuttondown(0, 0);
    triggerEvent(MOUSE_LBUTTON_DOWN, this, NULL);
    wm_lbuttonup(0, 0);
    triggerEvent(MOUSE_LBUTTON_UP, this, NULL);
}

int
aedButton::wm_keydown(aedKeyEvent & event)
{
    if(event.sym == SDLK_RETURN || event.sym == SDLK_SPACE)
    {
        click();
        return 0;
    }
    return aedWidget::wm_keydown(event);
}

int
aedButton::wm_lbuttondown(Uint16 x, Uint16 y)
{
    if(m_ToggleMode)
    {
        setButtonState(!m_ButtonDown);
        triggerEvent(STATE_CHANGED, this, NULL);
    }
    else
        setButtonState(true);
    return 0;
}

int
aedButton::wm_lbuttonup(Uint16 x, Uint16 y)
{
    if(!m_ToggleMode)
        setButtonState(false);
    return 0;
}

// This should really be handled in wm_mouseleave...
int
aedButton::wm_update(Uint32 msdelta)
{
    if(m_ButtonDown && !isMouseOver())
        this->wm_lbuttonup(0, 0);

    return 0;
}

void
aedButton::setCaption(const std::string & caption)
{
    m_Caption = caption;
    m_Label.setCaption(caption);
    setRenderState(true);
}

void
aedButton::setImage(aedImage image)
{
    if(m_Image)
        delete m_Image;

    m_Image = new aedImage(image);
    setRenderState(true);
}

void
aedButton::render(void)
{
    if(m_ButtonType == ROUND)
        m_Theme->drawButton(this, m_Surface);
    else if(m_ButtonType == SQUARE)
        m_Theme->drawSquareButton(this, m_Surface);
    else if(m_ButtonType >= BUTTON_ARROW_UP)
    {
        m_Theme->drawSquareButton(this, m_Surface);

        int a = m_Surface->w;

        if(a > m_Surface->h)
            a = m_Surface->h;
        a /= 2;

        DrawArrow(m_Surface, m_ButtonType - BUTTON_ARROW_UP,
                  m_Surface->w / 2, m_Surface->h / 2, a,
                  SDL_MapRGBA(m_Surface->format, 0, 0, 0, 255),
                  isMouseOver(), SDL_MapRGBA(m_Surface->format, 0, 0, 0, 255));
    }

    SDL_Rect rect = { 0, 0, 0, 0 };

    if(m_Image)
    {
        rect.w = m_Image->getSurface()->w;
        rect.h = m_Image->getSurface()->h;

        // Center image vertically
        rect.y += getPos().getHeight() / 2 - rect.h / 2;

        if(m_Caption.size() == 0)
            rect.x += getPos().getWidth() / 2 - rect.w / 2;
        else
            rect.x += 4;

        SDL_BlitSurface(m_Image->getSurface(), 0, m_Surface, &rect);

        // Caption's position
        rect.x += rect.w + 4;
        m_Label.getFont()->getTextSize(m_Caption.c_str(), &rect.w, &rect.h);
        rect.y = getPos().getHeight() / 2 - rect.h / 2;
    }
    else
    {
        // Center the caption
        m_Label.getFont()->getTextSize(m_Caption.c_str(), &rect.w, &rect.h);
        rect.x += getPos().getWidth() / 2 - rect.w / 2;
        rect.y += getPos().getHeight() / 2 - rect.h / 2;
    }

    m_Label.setPos(rect.x, rect.y);
    m_Label.setSize(rect.w, rect.h);
}

int
aedButton::wm_mouseenter()
{
    setRenderState(true);
    return 0;
}

int
aedButton::wm_mouseleave()
{
    setRenderState(true);
    return 0;
}

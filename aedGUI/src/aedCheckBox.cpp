#include "aedWidget.h"
#include "aedCheckBox.h"

aedCheckBox::aedCheckBox()
{
    m_IsWidget = true;
    m_State = false;
    m_CanFocus = true;
}

aedCheckBox::~aedCheckBox()
{
}

void
aedCheckBox::setCaption(const std::string & caption)
{
    m_Caption = caption;
    setRenderState(true);
}

int
aedCheckBox::wm_lbuttondown(Uint16 x, Uint16 y)
{
    m_State = !m_State;
    setRenderState(true);
    triggerEvent(STATE_CHANGED, this, NULL);
    return 0;
}

bool aedCheckBox::getState()
{
    return m_State;
}

void
aedCheckBox::setState(bool state)
{
    if(m_State != state)
    {
        m_State = state;
        setRenderState(true);
        triggerEvent(STATE_CHANGED, this, NULL);
    }
}

void
aedCheckBox::render()
{
    m_Theme->drawCheckBox(this, m_Surface);
}

int
aedCheckBox::wm_mouseenter()
{
    setRenderState(true);
    return 0;
}

int
aedCheckBox::wm_mouseleave()
{
    setRenderState(true);
    return 0;
}

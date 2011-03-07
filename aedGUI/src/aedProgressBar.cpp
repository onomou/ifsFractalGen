#include "aedProgressBar.h"
#include "drawlibs.h"
#include "aedTheme.h"

aedProgressBar::aedProgressBar()
{
    m_CanFocus = false;
    m_IsWidget = true;
    m_PercentValue = 0;
    setBorder(AED_BORDER_ROUND);
}

aedProgressBar::~aedProgressBar()
{
}

void
aedProgressBar::render(void)
{
    m_Theme->drawProgressBar(this, m_Surface);
}

void
aedProgressBar::setValue(Uint8 value)
{
    if(value != m_PercentValue)
    {
        if(value > 100)
            value = 100;

        m_PercentValue = value;
        setRenderState(true);
    }
}

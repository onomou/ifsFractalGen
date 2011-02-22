#include "aedFrame.h"
#include "aedTheme.h"
#include "aedApp.h"

aedFrame::aedFrame()
{
    m_FontSize = m_Theme->defaultFontSize;
    m_PositionOffset.setX(2);
    m_PositionOffset.setY((Sint16) (m_FontSize * 1.5));
}

aedFrame::~aedFrame()
{
}

void
aedFrame::render(void)
{
    m_Theme->drawFrame(this, m_Surface);
}

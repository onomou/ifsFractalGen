#include "aedBoxSizer.h"
#include "rint.h"

aedBoxSizer::aedBoxSizer(aedWidget * parent, boxType type)
{
    m_Parent = parent;
    m_BoxType = type;
}

void
aedBoxSizer::calculateSize(void)
{
    if(m_BoxType == aedVERTICAL)
    {
        aedRect rect = m_Parent->getMyTranslatedPosition();
        Uint16 eachHeight =
            rint((rect.getHeight() -
                  (m_Parent->getPositionOffset().getY()))) / m_Widgets.size();

        for(unsigned int i = 0; i < m_Widgets.size(); i++)
        {
            //aedRect       widgetRect      = m_Widgets[i]->getPos();
            m_Widgets[i]->setPos(0, i * eachHeight);
            m_Widgets[i]->setSize(rect.getWidth() -
                                  (m_Parent->getPositionOffset().getX() * 2),
                                  eachHeight);
        }

    }
    else
    {


    }
}

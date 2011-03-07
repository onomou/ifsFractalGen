#include "aedRadioButtonGroup.h"
#include "drawlibs.h"

aedRadioButtonGroup::aedRadioButtonGroup()
{
    m_NextY = 0;
    m_Selected = -1;
    m_CanFocus = true;
}

aedRadioButtonGroup::~aedRadioButtonGroup()
{
    for(Uint8 i = 0; i < m_Buttons.size(); i++)
        delete m_Buttons[i];
}

void
aedRadioButtonGroup::addButton(const std::string & caption)
{
    aedCheckBox *p;

    p = new aedCheckBox;
    p->create(this, aedRect(1, m_NextY, getPos().getWidth(), 18), caption);

    p->setStatic(true);

    m_Buttons.push_back(p);
    m_NextY += p->getPos().getHeight() + 4;

    if(m_Selected == -1)
    {
        m_Selected = 0;
        p->setState(true);
    }

    setRenderState(true);
}

void
aedRadioButtonGroup::removeButton(unsigned int index)
{
    if(index < m_Buttons.size())
    {
        aedCheckBox *p;

        p = m_Buttons[index];
        m_NextY -= p->getPos().getHeight() + 4;
        this->removeWidget(p);
        m_Buttons.erase(m_Buttons.begin() + index);
        delete p;

        setRenderState(true);

        // Move lower buttons up one row
        for(unsigned int i = index; i < m_Buttons.size(); i++)
        {
            p = m_Buttons[i];
            p->setPos(p->getPos().getX(), p->getPos().getY() -
                      p->getPos().getHeight() - 4);
        }
    }
}

void
aedRadioButtonGroup::selectButton(unsigned int index)
{
    if(m_Selected >= 0 && index < m_Buttons.size())
    {
        if((unsigned int) m_Selected != index)
        {
            m_Buttons[m_Selected]->setState(false);
            m_Buttons[index]->setState(true);
            if(pApp->getFocusWidget() == this)
            {
                m_Buttons[m_Selected]->setActiveBorder(false);
                m_Buttons[index]->setActiveBorder(true);
            }
            m_Selected = index;

            setRenderState(true);
            triggerEvent(STATE_CHANGED, this, NULL);
        }
        else
        {
            if(pApp->getFocusWidget() == this)
            {
                m_Buttons[m_Selected]->setActiveBorder(true);
                m_Buttons[m_Selected]->setRenderState(true);
            }
        }
    }
}

int
aedRadioButtonGroup::wm_lbuttondown(Uint16 x, Uint16 y)
{
    for(unsigned int i = 0; i < m_Buttons.size(); i++)
    {
        if(m_Buttons[i]->getRealPos().isPointIn(x, y))
        {
            selectButton(i);
            break;
        }
    }

    return 0;
}

int
aedRadioButtonGroup::wm_lostfocus()
{
    m_Buttons[m_Selected]->setActiveBorder(false);
    return 0;
}

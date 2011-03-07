#include <cstdlib>
#include "aedSpinner.h"

aedSpinner::aedSpinner()
{
    m_Min = -(1 << 30);
    m_Max = (1 << 30) - 1;
    m_Value = 0;
}

void
aedSpinner::create(aedWidget * parent, aedRect pos, std::string caption)
{
    aedWidget::create(parent, pos, caption);
    m_ButtonUp.setButtonType(SQUARE);
    m_ButtonDown.setButtonType(SQUARE);

    Uint32 dx = 20;

    m_TextBox.create(this, aedRect(0, 0, pos.getWidth() - dx, pos.getHeight()));
    m_TextBox.setCaption("0");
    m_TextBox.setJustification(AED_JUST_RIGHT);
    m_TextBox.setCharMask("qwertyuiop[]asdfghjkl;'\\zxcvbnm,./=)(*&^%$#@!~ ");
    m_TextBox.connect(TEXT_CHANGED,
                      aedCallBack(this, &aedSpinner::textbox_change));

    m_ButtonUp.create(this, aedRect(pos.getWidth() - dx, 0,
                                    dx, pos.getHeight() / 2));
    m_ButtonDown.create(this, aedRect(pos.getWidth() - dx, pos.getHeight() / 2,
                                      dx, pos.getHeight() / 2));

    m_ButtonUp.connect(MOUSE_LBUTTON_DOWN,
                       aedCallBack(this, &aedSpinner::buttonUpClicked));
    m_ButtonDown.connect(MOUSE_LBUTTON_DOWN,
                         aedCallBack(this, &aedSpinner::buttonDownClicked));

    m_ButtonUp.setBorder(AED_BORDER_NONE);
    m_ButtonDown.setBorder(AED_BORDER_NONE);
    m_ButtonUp.setButtonType(BUTTON_ARROW_UP);
    m_ButtonDown.setButtonType(BUTTON_ARROW_DOWN);
}

void
aedSpinner::setValue(Sint32 val)
{
    if(val >= m_Min && val <= m_Max)
    {
        char tmp[32];

        sprintf(tmp, "%d", val);
        m_TextBox.setCaption(tmp);
        m_Value = val;
        setRenderState(true);
        triggerEvent(VALUE_CHANGED, this, NULL);
    }
}

void
aedSpinner::setRange(Sint32 minval, Sint32 maxval)
{
    if(minval >= maxval)
        return;

    m_Min = minval;
    m_Max = maxval;

    if(m_Value < m_Min || m_Value > m_Max)
        setValue(m_Min);
}

void
aedSpinner::incrementValue()
{
    setValue(m_Value + 1);
}

void
aedSpinner::decrementValue()
{
    setValue(m_Value - 1);
}

void
aedSpinner::buttonUpClicked(void *caller, void *data)
{
    incrementValue();
}

void
aedSpinner::buttonDownClicked(void *caller, void *data)
{
    decrementValue();
}

void
aedSpinner::textbox_change(void *caller, void *data)
{
    Sint32 val;

    // This is just a quick hack... needs rethinking
    // Note that calling m_TextBox.setCaption() from here
    // results in a recursive call
    if(m_TextBox.getCaption().empty())
    {
        m_TextBox.setCaption("0");
        return;
    }

    val = atoi(m_TextBox.getCaption().c_str());
    if(val >= m_Min && val <= m_Max)
    {
        m_Value = val;
        triggerEvent(VALUE_CHANGED, this, NULL);
    }
}

int
aedSpinner::wm_update(Uint32 msdelta)
{
    Uint8 state;
    int mx, my;

    state = SDL_GetMouseState(&mx, &my);
    if(!getRealPos().isPointIn(mx, my))
        return 0;

/*
    if(state & SDL_BUTTON(1))
    {
        if(m_ButtonDown.getRealPos().isPointIn(mx, my))
            decrementValue();
        else if(m_ButtonUp.getRealPos().isPointIn(mx, my))
				incrementValue();
	}
*/
    return 0;
}

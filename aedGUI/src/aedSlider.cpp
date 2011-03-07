#include "aedSlider.h"
#include "drawlibs.h"
#include "aedTheme.h"
#include "rint.h"

aedSlider::aedSlider(aedOrientation orient)
{
    // we won't trigger an event from here
    m_Min = 0;
    m_Max = 10;
    m_Value = 0;
    setBigChange(2);
    m_Orientation = orient;
    m_Dragging = false;
    setRenderState(true);
    m_IsWidget = true;
    m_CanFocus = true;
}

aedSlider::~aedSlider()
{
}

void
aedSlider::setValue(Uint32 val)
{
    if(val >= m_Min && val <= m_Max)
    {
        m_Value = val;
        setRenderState(true);
        triggerEvent(VALUE_CHANGED, this, NULL);
    }
}

void
aedSlider::setRange(Uint32 minval, Uint32 maxval)
{
    if(minval >= maxval)
        return;

    m_Min = minval;
    m_Max = maxval;

    if(m_Value < m_Min || m_Value > m_Max)
    {
        m_Value = m_Min;
        triggerEvent(VALUE_CHANGED, this, NULL);
    }

    setRenderState(true);
}

void
aedSlider::setBigChange(Uint32 bigChange)
{
    if(bigChange > m_Max - m_Min)
        return;

    m_BigChange = bigChange;
}

void
aedSlider::incrementValue(bool bigChange)
{
    if(bigChange)
        m_Value += m_BigChange;
    else
        m_Value++;

    if(m_Value > m_Max)
        m_Value = m_Max;

    setRenderState(true);
    triggerEvent(VALUE_CHANGED, this, NULL);
}

void
aedSlider::decrementValue(bool bigChange)
{
    if(m_Value == 0)
        return;

    if(bigChange)
        m_Value -= m_BigChange;
    else
        m_Value--;

    if(m_Value < m_Min)
        m_Value = m_Min;

    setRenderState(true);
    triggerEvent(VALUE_CHANGED, this, NULL);
}

// Calculate and return elevator's rect
// Elevator's position is absolute because
// of getRealPos()
aedRect
aedSlider::getElevatorRect()
{
    Uint32 max, min;
    double elevatorSize;
    SDL_Rect rect;

    rect = getRealPos().getSDLRect();
    getRange(&min, &max);

    if(m_Orientation == AED_VERTICAL)
        elevatorSize = ((double) rect.h / (double) ((max - min) + 1));
    else
        elevatorSize = ((double) rect.w / (double) ((max - min) + 1));

    if(m_Orientation == AED_VERTICAL)
    {
        rect.x++;
        rect.w -= 2;
        rect.h = (Uint16) rint(elevatorSize);
        rect.y += (Uint16) rint(elevatorSize * (getValue() - min));
    }
    else
    {
        rect.y++;
        rect.h -= 2;
        rect.w = (Uint16) rint(elevatorSize);
        rect.x += (Uint16) rint(elevatorSize * (getValue() - min));
    }

    return aedRect(rect.x, rect.y, rect.w, rect.h);
}

int
aedSlider::wm_update(Uint32 msdelta)
{
    Uint8 state;
    int mx, my;

    state = SDL_GetMouseState(&mx, &my);
    if(!getRealPos().isPointIn(mx, my))
        return 0;

    if(m_Dragging && (state & SDL_BUTTON(1)) &&
       !getElevatorRect().isPointIn(mx, my))
    {
        SDL_Rect rect = getElevatorRect().getSDLRect();
        Uint32 min;

        getRange(&min, NULL);
        if(getOrientation() == AED_VERTICAL)
            setValue((Uint32) (min + rint(my - getRealPos().getY()) / rect.h));
        else
            setValue((Uint32) (min + rint(mx - getRealPos().getX()) / rect.w));
        setRenderState(true);
    }

    return 0;
}

int
aedSlider::wm_lbuttonup(Uint16 x, Uint16 y)
{
    m_Dragging = false;
    return 0;
}

int
aedSlider::wm_lbuttondown(Uint16 x, Uint16 y)
{
    SDL_Rect rect;

    if(getElevatorRect().isPointIn(x, y))
        m_Dragging = true;

    rect = getElevatorRect().getSDLRect();
    if(getOrientation() == AED_VERTICAL)
    {
        if(y < rect.y)
            decrementValue();
        else if(y > rect.y + rect.h)
            incrementValue();
    }
    else
    {
        if(x < rect.x)
            decrementValue();
        else if(x > rect.x + rect.w)
            incrementValue();
    }

    setRenderState(true);

    return 0;
}

void
aedSlider::render(void)
{
    m_Theme->drawSlider(this, m_Surface);
}

#include "aedScrollBar.h"
#include "aedButton.h"
#include "rint.h"
#include "aedTheme.h"

aedScrollBar::aedScrollBar(aedOrientation o):aedSlider(o)
{
}

void
aedScrollBar::create(aedWidget * parent, aedRect pos, std::string caption)
{
    aedWidget::create(parent, pos, caption);

    if(getOrientation() == AED_VERTICAL)
    {
        m_ButtonUp.create(this, aedRect(0, 0, TAKEOVERX, 18));
        m_ButtonDown.create(this, aedRect(0, FULLY, TAKEOVERX, 18));
        m_ButtonUp.setButtonType(BUTTON_ARROW_UP);
        m_ButtonDown.setButtonType(BUTTON_ARROW_DOWN);
    }
    else
    {
        m_ButtonUp.create(this, aedRect(0, 0, 18, TAKEOVERY));
        m_ButtonDown.create(this, aedRect(FULLX, 0, 18, TAKEOVERY));
        m_ButtonUp.setButtonType(BUTTON_ARROW_RIGHT);
        m_ButtonDown.setButtonType(BUTTON_ARROW_LEFT);
    }

    m_ButtonUp.connect(MOUSE_LBUTTON_DOWN,
                       aedCallBack(this, &aedScrollBar::buttonUpClicked));
    m_ButtonDown.connect(MOUSE_LBUTTON_DOWN,
                         aedCallBack(this, &aedScrollBar::buttonDownClicked));

    m_ButtonUp.setBorder(AED_BORDER_NONE);
    m_ButtonDown.setBorder(AED_BORDER_NONE);
}

aedScrollBar::~aedScrollBar()
{
}

aedRect
aedScrollBar::getElevatorRect()
{
    Uint32 max, min;
    double elevatorSize;
    SDL_Rect rect;

    rect = getRealPos().getSDLRect();
    getRange(&min, &max);

    if(getOrientation() == AED_VERTICAL)
    {
        rect.h -= 36;
        elevatorSize = ((double) rect.h / (double) ((max - min) + 1));
        rect.x++;
        rect.w -= 2;
        rect.h = (Uint16) rint(elevatorSize);
        rect.y += m_ButtonUp.getPos().getHeight() +
            (Uint16) rint(elevatorSize * (getValue() - min));
    }
    else
    {
        rect.w -= 36;
        elevatorSize = ((double) rect.w / (double) ((max - min) + 1));
        rect.y++;
        rect.h -= 2;
        rect.w = (Uint16) rint(elevatorSize);
        rect.x += m_ButtonUp.getPos().getWidth() +
            (Uint16) rint(elevatorSize * (getValue() - min));
    }

    return aedRect(rect.x, rect.y, rect.w, rect.h);
}

int
aedScrollBar::wm_update(Uint32 msdelta)
{
    Uint8 state;
    int mx, my;

    state = SDL_GetMouseState(&mx, &my);
    if(!getRealPos().isPointIn(mx, my))
        return 0;

    if(state & SDL_BUTTON(1))
    {
        if(m_ButtonDown.getRealPos().isPointIn(mx, my))
            incrementValue();
        else if(m_ButtonUp.getRealPos().isPointIn(mx, my))
            decrementValue();
    }

    if(m_Dragging && (state & SDL_BUTTON(1)) &&
       !getElevatorRect().isPointIn(mx, my))
    {
        SDL_Rect rect = getElevatorRect().getSDLRect();
        Uint32 min;

        getRange(&min, NULL);
        if(getOrientation() == AED_VERTICAL)
        {
            setValue((Uint32) (min + rint(my - getRealPos().getY() -
                                          m_ButtonUp.getPos().getHeight()) /
                               rect.h));
        }
        else
        {
            setValue((Uint32) (min + rint(mx - getRealPos().getX() -
                                          m_ButtonUp.getPos().getWidth()) /
                               rect.w));
        }
        setRenderState(true);
    }

    return 0;
}

int
aedScrollBar::wm_lbuttonup(Uint16 x, Uint16 y)
{
    m_Dragging = false;
    return 0;
}

int
aedScrollBar::wm_lbuttondown(Uint16 x, Uint16 y)
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
aedScrollBar::buttonUpClicked(void *caller, void *data)
{
    aedWidget *btn = (aedWidget *) caller;
    aedScrollBar *scroll = dynamic_cast < aedScrollBar * >(btn->getParent());

    scroll->decrementValue();
}

void
aedScrollBar::buttonDownClicked(void *caller, void *data)
{
    aedWidget *btn = (aedWidget *) caller;
    aedScrollBar *scroll = dynamic_cast < aedScrollBar * >(btn->getParent());

    scroll->incrementValue();
}

void
aedScrollBar::render(void)
{
    m_Theme->drawScrollBar(this, m_Surface);
}

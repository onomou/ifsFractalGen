#include "aedWindow.h"
 aedWindow::aedWindow() 
{
    m_InitComplete = false;
    m_isLPressed = false;
    m_Moveable = false;
    m_Caption = "";
    m_TitleBarBorderX = 5;
    m_TitleBarBorderY = 5;
    m_TitleBarHeight = 25;
    m_CloseButton = NULL;
    m_MinimizeButton = NULL;
    m_MaximizeButton = NULL;
    m_CaptionLabel = NULL;
}

aedWindow::~aedWindow() 
{
    delete m_CloseButton;
    delete m_MinimizeButton;
    delete m_MaximizeButton;
    delete m_CaptionLabel;
}
void
aedWindow::create(aedWidget * parent, aedRect pos, std::string caption,
                  Uint32 flags) 
{
    if(flags & WF_TITLEBAR)
        
    {
        int TitleBarX = m_TitleBarBorderX;
        int TitleBarY = m_TitleBarBorderY;
        int TitleBarWidth = pos.getWidth() - (2 * m_TitleBarBorderX);
        int TitleBarHeight = m_TitleBarHeight;
        int ClientX = m_TitleBarBorderX;
        int ClientY = TitleBarY + TitleBarHeight + m_TitleBarBorderY;
        int ClientWidth = pos.getWidth() - (2 * m_TitleBarBorderX);
        int ClientHeight = pos.getHeight() - ClientY - m_TitleBarBorderY;

        
            // Create the client area
            m_ClientArea.create(this,
                                aedRect(ClientX, ClientY, ClientWidth,
                                        ClientHeight));
        m_ClientArea.setUserData(reinterpret_cast < void *>(this));

        m_ClientArea.setBGColor(aedColor(240, 240, 240, 255));
        m_CaptionLabel = new aedStaticText();
        m_CaptionLabel->create(this, aedRect(0, 0, 0, 0));
        m_CaptionLabel->setUserData(reinterpret_cast < void *>(this));

        m_CaptionLabel->bringToTop();
        if(flags & WF_CLOSEBUTTON)
            
        {
            int ButtonSpacing = 2;     // Eeep, magic number
            int ButtonWidth = m_TitleBarHeight - ButtonSpacing;
            int ButtonHeight = m_TitleBarHeight - (ButtonSpacing * 2);
            int ButtonX = pos.getWidth() - ButtonWidth - m_TitleBarBorderY;
            int ButtonY = TitleBarY + ButtonSpacing;

            m_CloseButton = new aedButton();
            m_CloseButton->create(this,
                                    aedRect(ButtonX, ButtonY, ButtonWidth,
                                            ButtonHeight), "X");
            m_CloseButton->setUserData(reinterpret_cast < void *>(this));

            m_CloseButton->connect(MOUSE_LBUTTON_UP,
                                    aedCallBack
                                    (__aedInternal__WindowCloseButtonHandler));
            m_CloseButton->bringToTop();
        }
    }
    
    else
        
    {
        m_ClientArea.create(this, pos);
    }
    if(flags & WF_MOVEABLE)
        
    {
        m_Moveable = true;
    }
    
        // TODO: Rest of creation flags
        aedWidget::create(parent, pos, caption);
    m_InitComplete = true;
}
void
aedWindow::render(void) 
{
    
        // Position our title caption correctly
        Uint16 CaptionWidth, CaptionHeight;
    int x, y;

    m_CaptionLabel->getFont()->getTextSize(m_Caption.c_str(), &CaptionWidth,
                                             &CaptionHeight);
    m_CaptionLabel->setPos(m_TitleBarBorderX * 2,
                            (m_TitleBarHeight / 2) + m_TitleBarBorderY -
                            (CaptionHeight / 2));
    m_CaptionLabel->setSize(CaptionWidth, CaptionHeight);
    
        // Draw the actual window
        m_Theme->drawBox(reinterpret_cast < aedWidget * >(this), m_Surface);
    m_Theme->drawBorder(reinterpret_cast < aedWidget * >(this), m_Surface);
    
        // Draw a box around the title bar
    int TitleBarX1 = m_TitleBarBorderX;
    int TitleBarY1 = m_TitleBarBorderY;
    int TitleBarX2 = m_Pos.getWidth() - (2 * m_TitleBarBorderX);
    int TitleBarY2 = m_TitleBarHeight + m_TitleBarBorderY;

    if(m_CloseButton != NULL)
        
    {
        TitleBarX2 -= (m_CloseButton->getPos().getWidth());
    }
    aedColor temp(0, 0, 255, 255);
    
        // we need to lock the surface before touching its pixels...
        SLOCK(m_Surface);
    DrawRect(m_Surface, TitleBarX1, TitleBarY1, TitleBarX2, TitleBarY2,
              temp.getUint32(m_Surface));
    SUNLOCK(m_Surface);
}
int
aedWindow::wm_lbuttondown(Uint16 x, Uint16 y) 
{
    if(!m_Moveable)
        
    {
        return 0;
    }
    screenToClient(x, y);
    if((x > (m_Pos.getX() + 5)) &&
         (x < (m_Pos.getX() + m_Pos.getWidth() - 5)))
        
    {
        if((y > m_Pos.getY() + 5) && y < m_Pos.getY() + 45)
            
        {
            m_isLPressed = true;
            dragXoffset = x - m_Pos.getX();
            dragYoffset = y - m_Pos.getY();
        }
    }
    return 0;
}
int
aedWindow::wm_lbuttonup(Uint16 x, Uint16 y) 
{
    m_isLPressed = false;
    return 0;
}
int
aedWindow::wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy) 
{
    if(m_isLPressed)
        
    {
        
            //setPos(getPos().getX() + dx, getPos().getY() + dy);
            setPos(x - dragXoffset, y - dragYoffset);
        
            //setRenderState(true);
            getParent()->setRenderState(true);
    }
    return 0;
}
void
aedWindow::Maximize() 
{
    
        // TODO: Write me
} void

aedWindow::Minimize() 
{
    
        // TODO: Write me
} void
__aedInternal__WindowCloseButtonHandler(void *caller, void *data) 
{
    aedButton * TheButton = reinterpret_cast < aedButton * >(caller);
    aedWidget * TheWindow =
        reinterpret_cast < aedWidget * >(TheButton->getUserData());
    if(TheWindow != NULL)
        
    {
        TheWindow->hide();
    }
}
void
__aedInternal__WindowMaxButtonHandler(void *caller, void *data) 
{
    aedButton * TheButton = reinterpret_cast < aedButton * >(caller);
    aedWindow * TheWindow =
        reinterpret_cast < aedWindow * >(TheButton->getUserData());
    if(TheWindow != NULL)
        
    {
        TheWindow->Maximize();
    }
}
void
__aedInternal__WindowMinButtonHandler(void *caller, void *data) 
{
    aedButton * TheButton = reinterpret_cast < aedButton * >(caller);
    aedWindow * TheWindow =
        reinterpret_cast < aedWindow * >(TheButton->getUserData());
    if(TheWindow != NULL)
        
    {
        TheWindow->Minimize();
    }
}



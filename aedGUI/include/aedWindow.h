
#ifndef AEDWINDOW_H
#define AEDWINDOW_H
    
#include "dllmacros.h"
#include "aedWidget.h"
#include "aedStaticText.h"
#include "aedButton.h"
enum aedWinFlags 
{ WF_PLAIN = 0, WF_RESIZEABLE = 1, WF_MOVEABLE = 2, WF_TITLEBAR =
        4, WF_CLOSEBUTTON = 8, WF_MAXBUTTON = 16, WF_MINBUTTON = 32, 
};
const Uint32 WF_NORMAL =
    WF_RESIZEABLE | WF_MOVEABLE | WF_TITLEBAR | WF_CLOSEBUTTON;
class DLLEXPORT aedWindow:public aedWidget 
{
  public:aedWindow();
    virtual ~ aedWindow();
    virtual void create(aedWidget * parent = NULL, aedRect pos =
                          aedDEFAULTPOSITION, std::string caption =
                          "", Uint32 flags = WF_NORMAL);
    virtual void render(void);
    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual int wm_lbuttonup(Uint16 x, Uint16 y);
    virtual int wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy);
    virtual void setCaption(const std::string & caption) 
    {
        m_Caption = caption;
        m_CaptionLabel->setCaption(caption);
    } aedWidget * getClientArea() 
    {
        return &m_ClientArea;
    }
    virtual bool addWidget(aedWidget * win) 
    {
        if(m_InitComplete == false || win->getUserData() == this)
            
        {
            return aedWidget::addWidget(win);
        }
        
        else
            
        {
            return m_ClientArea.addWidget(win);
        }
    }
    virtual bool removeWindget(aedWidget * win) 
    {
        if(m_InitComplete == false || win->getUserData() == this)
            
        {
            return aedWidget::removeWidget(win);
        }
        
        else
            
        {
            return m_ClientArea.removeWidget(win);
        }
    }
    void Maximize();
    void Minimize();

  private:bool m_InitComplete;
    
        //bool                  m_Dragging;
        bool m_Moveable;
    int m_TitleBarBorderX;
    int m_TitleBarBorderY;
    int m_TitleBarHeight;

    aedWidget m_ClientArea;
    aedButton * m_CloseButton;
    aedButton * m_MinimizeButton;
    aedButton * m_MaximizeButton;
    aedStaticText * m_CaptionLabel;
    Uint16 dragXoffset;
    Uint16 dragYoffset;
};
void __aedInternal__WindowCloseButtonHandler(void *caller, void *data);
void __aedInternal__WindowMaxButtonHandler(void *caller, void *data);
void __aedInternal__WindowMinButtonHandler(void *caller, void *data);


#if defined(_MSC_VER)
    DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedButton >;

#endif  /*  */
    
#endif  /*  */

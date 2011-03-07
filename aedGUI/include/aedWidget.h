
/*
 * The aedWidget class
 * The "visible widgets" base class
 * Initial design by Eduardo B. Fonseca <ebf@aedsolucoes.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef AEDWIDGET_H
#define AEDWIDGET_H

#include "aedObject.h"
#include "aedRect.h"
#include "aedColor.h"
#include "aedFont.h"
#include "aedTheme.h"
#include "aedApp.h"

#define sgn(x) ((x<0)?-1:((x>0)?1:0))

#define TAKEOVERX -1
#define TAKEOVERY -1
#define CENTERX -2
#define CENTERY -2
#define FULLX -3
#define FULLY -3
#define TAKEOVERPARENT TAKEOVERX, TAKEOVERY

/////////////////////////////////////
// TODO: we need to change this to a real event class... one we can use
// on the aedEventManager and on the aedSignal class.
struct aedKeyEvent
{
    Uint16 sym;
    Uint16 unicode;
};

// Justifications
enum aedJusts
{
    AED_JUST_LEFT = 1,
    AED_JUST_CENTER,
    AED_JUST_RIGHT
};

// Border types
enum
{
    AED_BORDER_NONE = 0,
    AED_BORDER_ROUND = 1,
    AED_BORDER_SQUARED = 2
};

const Uint16 cPad = 3;

class DLLEXPORT aedWidget:public aedObject
{
  public:
    aedWidget();
    virtual ~ aedWidget();

    // The root constructor
    aedWidget(Uint16 screenWidth, Uint16 screenHeight, bool takeOverScreen =
              true);

    // The CREATE function
    virtual void create(aedWidget * parent = NULL, aedRect pos =
                        aedDEFAULTPOSITION, std::string caption = "");

    // Basic Functions
    void setStatic(bool value)
    {
        m_IsStatic = value;
    }

    virtual void enable(bool value)
    {
        m_IsEnabled = value;
        setRenderState(true);
    }

    void show(void)
    {
        m_ShowWidget = true;
        setRenderState(true);
    }
    void hide(void)
    {
        m_ShowWidget = false;
        if(getParent())
            getParent()->setRenderState(true);
    }
    bool isVisible(void) const
    {
        return m_ShowWidget;
    }
    bool isLPressed(void) const
    {
        return m_isLPressed;
    }
    bool isRPressed(void) const
    {
        return m_isRPressed;
    }
    void bringToTop(void);
    bool hasFocus() const;
    aedWidget *getTopMostWidget();

    // Coords
    Uint16 getScreenWidth(void) const
    {
        return m_ScreenWidth;
    }
    Uint16 getScreenHeight(void) const
    {
        return m_ScreenHeight;
    }
    void setPos(Sint16 x, Sint16 y);
    void setSize(Sint16 w, Sint16 h);

    void screenToClient(Uint16 & x, Uint16 & y);
    Uint16 clientxToScreen(Uint16 x = 0);
    Uint16 clientyToScreen(Uint16 y = 0);
    Sint32 clientwToScreen(Sint32 w = 0);
    Sint32 clienthToScreen(Sint32 h = 0);

    aedRect getPositionOffset(void);
    aedRect getPos(void);
    aedRect getPosition(void);
    aedRect getMyTranslatedPosition(void);
    aedRect getRealPos(void)
    {
        return getMyPosition();
    }

    // Drawing Functions
    bool getRenderState(void) const
    {
        return m_ReRender;
    }
    void setRenderState(bool status)
    {
        m_ReRender = status;
        if(m_ReRender)
            makeAllDirty();
    }

    void renderAll(SDL_Surface * screen = NULL);

    virtual void render(void);
    aedColor getBGColor(void) const
    {
        return m_BGColor;
    }

    void setBGColor(aedColor color)
    {
        m_BGColor = color;
    }
    void setActiveBorder(bool value)
    {
        m_HasActiveBorder = value;
    }
    bool getActiveBorder(void) const
    {
        return m_HasActiveBorder;
    }

    void setAlphaValue(Uint8 alpha);
    Uint8 getAlphaValue(void) const
    {
        return m_AlphaValue;
    }

    bool isMouseOver(void);

    // Caption Functions
    virtual void setCaption(const std::string & caption)
    {
        m_Caption = caption;
    }
    virtual std::string getCaption(void) const
    {
        return m_Caption;
    }

    // Border stuff
    int getBorder() const
    {
        return m_Border;
    }
    void setBorder(int border)
    {
        if(m_Border != border)
        {
            m_Border = border;
            setRenderState(true);
        }
    }

    aedColor getBorderColor() const
    {
        return m_BorderColor;
    }
    void setBorderColor(aedColor color)
    {
        m_BorderColor = color;
        setRenderState(true);
    }

    void setFont(aedFont * font)
    {
        m_Font = font;
        setRenderState(true);
    }
    aedFont *getFont() const
    {
        return m_Font;
    }

    void setTheme(aedTheme * Theme);

    aedTheme *getTheme() const
    {
        return m_Theme;
    }

    aedWidget *findWidget(const std::string & id);
    aedWidget *findWidget(aedWidget * pointer);
    virtual bool addWidget(aedWidget * win);
    virtual bool removeWidget(aedWidget * win);
    aedWidget *getParent() const
    {
        return m_Parent;
    }

    // Messages
    virtual int wm_paint(SDL_Surface * screen = NULL, Uint16 x = 0, Uint16 y =
                         0, Uint16 w = 0, Uint16 h = 0);
    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual int wm_lbuttonup(Uint16 x, Uint16 y);
    virtual int wm_rbuttondown(Uint16 x, Uint16 y);
    virtual int wm_rbuttonup(Uint16 x, Uint16 y);
    virtual int wm_sizechanged(Uint16 screenWidth, Uint16 screenHeight);
    virtual int wm_keydown(aedKeyEvent & event);
    virtual int wm_update(Uint32 msdelta);
    virtual int wm_gotfocus()
    {
        return 0;
    }
    virtual int wm_lostfocus()
    {
        return 0;
    }
    virtual int wm_mouseenter()
    {
        return 0;
    }
    virtual int wm_mouseleave()
    {
        return 0;
    }
    virtual int wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy)
    {
        return 0;
    }

    bool isEnabled() const
    {
        if(getParent())
            return m_IsEnabled && getParent()->isEnabled();
        return m_IsEnabled;
    }
    bool canFocus() const
    {
        return m_CanFocus && isEnabled();
    }

    void update();

    void setId(const std::string & id);

    const std::string & getId() const
    {
        return m_Id;
    }

    void setUserData(void *userdata)
    {
        m_userdata = userdata;
    }

    void *getUserData()
    {
        return m_userdata;
    }

    bool pollEvent(SDL_Event * event);

  protected:
    aedRect getMyPosition(void);
    void init(void);

  private:
    void processEvent(SDL_Event * event);
    aedWidget *findChildAtCoord(Uint16 x, Uint16 y, Uint8 flags = 0);
    void makeAllDirty(void);
    void updateAllChildren(Uint32 msdelta);
    bool tabFocus();
    bool tabFocusChildren();
    void tabFocusParent();

  protected:
    std::string m_Caption;
    bool m_IsEnabled:1;
    bool m_IsStatic:1;
    bool m_CanFocus:1;
    bool m_IsWidget:1;
    bool m_ShowWidget:1;
    bool m_ReRender:1;
    bool m_HasActiveBorder:1;
    int m_Border;
    aedColor m_BorderColor;
    aedColor m_BGColor;

    aedFont *m_Font;
    aedTheme *m_Theme;

    // The widget's drawing surface.
    SDL_Surface *m_Surface;

    Uint8 m_AlphaValue;
    Uint8 m_Flags;

    // The widget position, in aedGUI Virtual Units (AVU)
    aedRect m_Pos;

    // Here we "cache" the widget position in pixel values
    aedRect m_PixelPos;

    // Position Offset
    aedRect m_PositionOffset;
    bool m_isLPressed;
    bool m_isRPressed;

  private:
    std::string m_Id;
    void *m_userdata;
    aedWidget *m_Parent;

    std::vector < aedWidget * >m_Children;
    Uint32 m_CurTicks, m_LastTicks;

    Uint16 m_ScreenWidth, m_ScreenHeight;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedWidget >;
#endif

#endif

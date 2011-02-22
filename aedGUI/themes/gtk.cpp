#include "aedTheme.h"
#include "aedButton.h"
#include "aedFrame.h"
#include "aedSlider.h"
#include "aedScrollBar.h"
#include "drawlibs.h"

class DLLEXPORT gtkStyle:public aedTheme
{
  public:
    gtkStyle();
    void drawBox(aedWidget * widget, SDL_Surface * screen);
    void drawBorder(aedWidget * widget, SDL_Surface * screen);
    void drawButton(aedButton * widget, SDL_Surface * screen);
};

extern "C" DLLEXPORT aedTheme *
get_theme_instance()
{
    return new gtkStyle;
}

extern "C" DLLEXPORT Uint16
get_pif_version()
{
    return AEDGUI_PIF_VERSION;
}

gtkStyle::gtkStyle()
{
    m_ThemeInfo.name = "GTK-Style";
    m_ThemeInfo.author = "The aedGUI Team";
    m_ThemeInfo.description = "GTK-Style theme for aedGUI";

    defaultFontName = "";
    defaultFontSize = 8;
    defaultTextColor.setAllColors(0, 0, 0, 255);

    colorBoxBackground.setAllColors(255, 255, 255, 255);
    colorButtonNormal1.setAllColors(213, 214, 213, 255);
    colorButtonNormal2.setAllColors(213, 214, 213, 255);
    colorButtonPushed1.setAllColors(197, 194, 197, 255);
    colorButtonPushed2.setAllColors(197, 194, 197, 255);
    colorButtonOver1.setAllColors(213, 214, 213, 255);
    colorButtonOver2.setAllColors(213, 214, 213, 255);
}

void
gtkStyle::drawButton(aedButton * widget, SDL_Surface * screen)
{
    Uint16 screenWidth = screen->w - 1;
    Uint16 screenHeight = screen->h - 1;
    bool btnDown = widget->getButtonState();

    //bool setActiveBorder = true;

    if(!btnDown)
    {
        SDL_FillRect(screen, NULL, colorButtonNormal1.getUint32(screen));
        DrawLine(screen, 0, 0, screenWidth - 1, 0,
                 aedColor(255, 255, 255,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, 0, -1, 0, screenHeight - 1,
                 aedColor(255, 255, 255,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, 0, screenHeight, screenWidth, screenHeight,
                 aedColor(0, 0, 0,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, screenWidth, 0, screenWidth, screenHeight,
                 aedColor(0, 0, 0,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, 2, screenHeight - 1, screenWidth - 1, screenHeight - 1,
                 aedColor(148, 149, 148,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, screenWidth - 1, 2, screenWidth - 1, screenHeight - 1,
                 aedColor(148, 149, 148,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
    }
    else
    {
        SDL_FillRect(screen, NULL, colorButtonPushed1.getUint32(screen));
        DrawLine(screen, 0, 0, screenWidth - 1, 0,
                 aedColor(148, 149, 148,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, 0, -1, 0, screenHeight - 1,
                 aedColor(148, 149, 148,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, 0, screenHeight, screenWidth, screenHeight,
                 aedColor(0, 0, 0,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, screenWidth, 0, screenWidth, screenHeight,
                 aedColor(0, 0, 0,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, 2, screenHeight - 1, screenWidth - 1, screenHeight - 1,
                 aedColor(255, 255, 255,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
        DrawLine(screen, screenWidth - 1, 2, screenWidth - 1, screenHeight - 1,
                 aedColor(255, 255, 255,
                          PROCESS_ALPHA(255,
                                        widget->getAlphaValue())).
                 getUint32(screen));
    }

}

void
gtkStyle::drawBorder(aedWidget * widget, SDL_Surface * screen)
{
    DrawLine(screen, 0, 0, screen->w, 0,
             aedColor(213, 214, 205,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
    DrawLine(screen, screen->w - 1, 0, screen->w - 1, screen->h - 1,
             aedColor(213, 214, 205,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
    DrawLine(screen, 0, -1, 0, screen->h - 1,
             aedColor(213, 214, 205,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
    DrawLine(screen, screen->w - 2, 1, screen->w - 2, screen->h - 2,
             aedColor(246, 246, 238,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
    DrawLine(screen, 1, 1, screen->w - 2, 1,
             aedColor(172, 170, 164,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
    DrawLine(screen, 1, 0, 1, screen->h - 2,
             aedColor(172, 170, 164,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
    DrawLine(screen, 1, screen->h - 2, screen->w - 2, screen->h - 2,
             aedColor(246, 246, 238,
                      PROCESS_ALPHA(255,
                                    widget->getAlphaValue())).
             getUint32(screen));
}

void
gtkStyle::drawBox(aedWidget * widget, SDL_Surface * screen)
{

    SDL_Rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = screen->w;
    rect.h = screen->h;

    aedColor thisColor = colorBoxBackground;

    thisColor.setA(PROCESS_ALPHA(thisColor.getA(), widget->getAlphaValue()));
    SDL_FillRect(screen, &rect, thisColor.getUint32(screen));

}

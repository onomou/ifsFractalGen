#include "aedTheme.h"
#include "aedButton.h"
#include "aedFrame.h"
#include "aedSlider.h"
#include "aedScrollBar.h"
#include "aedProgressBar.h"
#include "aedCheckBox.h"
#include "drawlibs.h"
#include "rint.h"

// This is the default theme
aedTheme::aedTheme()
{
    // Fill in the theme info struct
    m_ThemeInfo.name = "aedGUI-default";
    m_ThemeInfo.author = "The aedGUI Team";
    m_ThemeInfo.description = "This is the default aedGUI theme";

    defaultFontName = "";
    defaultFontSize = 12;
    defaultTextColor.setAllColors(0, 0, 0, 255);

    colorBoxBackground.setAllColors(255, 255, 255, 255);
    colorButtonNormal1.setAllColors(255, 255, 255, 255);
    colorButtonNormal2.setAllColors(230, 230, 230, 255);
    colorButtonPushed1.setAllColors(210, 210, 210, 255);
    colorButtonPushed2.setAllColors(240, 240, 240, 255);
    colorButtonOver1.setAllColors(255, 255, 255, 255);
    colorButtonOver2.setAllColors(246, 242, 238, 255);

    colorDefaultWidgetBackground.setAllColors(238, 238, 230, 255);
}

aedTheme::~aedTheme()
{
}

void
aedTheme::drawCheckBox(aedCheckBox * widget, SDL_Surface * screen)
{
    aedFont *font = widget->getFont();

    std::string caption = widget->getCaption();
    int sy, maxWidth = 18, maxHeight = 18;
    SDL_Rect gradRect;
    Uint32 borderColor;

    if(!caption.empty())
    {
        font->renderTextBlended(screen, 20,
                                (widget->getPos().getHeight() -
                                 font->getHeight()) / 2, caption.c_str(),
                                defaultTextColor);
    }

    sy = (widget->getPos().getHeight() - maxHeight) / 2;
    gradRect.x = 1;
    gradRect.y = sy + 1;
    gradRect.w = maxWidth - 2;
    gradRect.h = maxHeight - 2;

    if(widget->getActiveBorder())
    {
        borderColor = SDL_MapRGBA(screen->format, 80, 80, 255,
                                  PROCESS_ALPHA(255, widget->getAlphaValue()));
    }
    else
    {
        borderColor = SDL_MapRGBA(screen->format, 0, 0, 0,
                                  PROCESS_ALPHA(60, widget->getAlphaValue()));
    }

    if(widget->getState())
    {
        vertgradient2(screen, gradRect, colorButtonPushed1, colorButtonPushed2,
                      widget->getAlphaValue());
    }
    else
    {
        vertgradient2(screen, gradRect, colorButtonNormal1, colorButtonNormal2,
                      widget->getAlphaValue());
    }

    SLOCK(screen);
    DrawPixel(screen, 2, sy + 2, borderColor);
    DrawPixel(screen, 2, sy + maxHeight - 2, borderColor);
    DrawPixel(screen, maxWidth - 2, sy + 2, borderColor);
    DrawPixel(screen, maxWidth - 2, sy + maxHeight - 2, borderColor);

    DrawLine(screen, 2, sy + 1, maxWidth - 3, sy + 1, borderColor);
    DrawLine(screen, 1, sy + 2, 1, sy + maxHeight - 3, borderColor);
    DrawLine(screen, maxWidth - 1, sy + 2, maxWidth - 1, sy + maxHeight - 3,
             borderColor);
    DrawLine(screen, 2, sy + maxHeight - 1, maxWidth - 3, sy + maxHeight - 1,
             borderColor);

    borderColor =
        SDL_MapRGBA(screen->format, 255, 255, 255,
                    PROCESS_ALPHA(255, widget->getAlphaValue()));
    DrawLine(screen, 2, sy + 2, 2, sy + maxHeight - 3, borderColor);
    DrawLine(screen, maxWidth - 2, sy + 2, maxWidth - 2, sy + maxHeight - 3,
             borderColor);
    DrawLine(screen, 2, sy + 2, maxWidth - 3, sy + 2, borderColor);
    DrawLine(screen, 2, sy + maxHeight - 2, maxWidth - 3, sy + maxHeight - 2,
             borderColor);

    SUNLOCK(screen);
}

// This should be optimized somehow...http://www.planetquake.com/polycount/resources/quake2/q2frameslist.shtml
void
aedTheme::drawProgressBar(aedProgressBar * widget, SDL_Surface * screen)
{
    const Uint16 BARSIZE = 5;
    const Uint16 BARSPACESIZE = 2;
    const Uint16 BARTOTALSIZE = BARSIZE + BARSPACESIZE;
    Uint16 i, numOfGrads;
    SDL_Rect rect, curRect, gradRect, surRect;
    SDL_Color c1, c2;
    SDL_Surface *fillSurface;
    Uint8 percentValue = widget->getValue();

    rect.x = 0;
    rect.y = 0;
    rect.w = screen->w;
    rect.h = screen->h;

    if(percentValue == 0 || percentValue > 100)
        return;

    if(rect.w <= 8 || rect.h <= 8)
        return;

    rect.x += 4;
    rect.y += 4;
    rect.w -= 8;
    rect.h -= 8;

    fillSurface = CreateSurface(SDL_SWSURFACE, 5, rect.h);
    ConvertSurface(&fillSurface);

    c1.r = 80;
    c1.g = 150;
    c1.b = 0;
    c2.r = 100;
    c2.g = 220;
    c2.b = 100;

    gradRect.x = 0;
    gradRect.y = 0;
    gradRect.w = fillSurface->w;
    gradRect.h = rect.h / 4;

    vertgradient2(fillSurface, gradRect, c1, c2, widget->getAlphaValue());

    gradRect.x = 0;
    gradRect.y = rect.h / 4;
    gradRect.w = fillSurface->w;
    gradRect.h = rect.h - gradRect.h;

    vertgradient2(fillSurface, gradRect, c2, c1, widget->getAlphaValue());

    rect.w = (Uint16) rint(((double) rect.w * percentValue) / 100.0);
    numOfGrads = rect.w / BARTOTALSIZE;

    curRect.x = rect.x - BARTOTALSIZE;
    curRect.y = rect.y;
    curRect.w = BARSIZE;
    curRect.h = rect.h;

    surRect.x = 0;
    surRect.y = 0;
    surRect.w = curRect.w;
    surRect.h = curRect.h;

    for(i = 0; i < numOfGrads; i++)
    {
        curRect.x = rect.x + (BARTOTALSIZE * i);
        SDL_BlitSurface(fillSurface, &surRect, screen, &curRect);
    }

    Uint16 remaining = rect.w % BARTOTALSIZE;

    if(remaining > 0)
    {
        surRect.w = remaining;
        curRect.x = curRect.x + BARTOTALSIZE;
        SDL_BlitSurface(fillSurface, &surRect, screen, &curRect);
    }

    SDL_FreeSurface(fillSurface);
}

// gradient colors here are lousy... need something better, not
// based on button gradients ;)
void
aedTheme::drawScrollBar(aedScrollBar * widget, SDL_Surface * screen)
{
    Uint16 maxHeight = screen->h - 1;
    Uint16 maxWidth = screen->w - 1;
    SDL_Color c1 = colorButtonPushed1;
    SDL_Color c2 = colorButtonPushed2;
    SDL_Rect rect = { 1, 1, screen->w - 2, screen->h - 2 };

    if(widget->getOrientation() == AED_VERTICAL)
        horizgradient2(screen, rect, c1, c2, widget->getAlphaValue());
    else
        vertgradient2(screen, rect, c1, c2, widget->getAlphaValue());

    c1.r = 21, c1.g = 45, c1.b = 72;
    c2 = c1;
    c2.b = 242;
    c2.g = 178;

    rect = widget->getElevatorRect().getSDLRect();
    rect.x -= widget->getRealPos().getX();
    rect.y -= widget->getRealPos().getY();

    // draw the elevator
    vertgradient2(screen, rect, c1, c2, widget->getAlphaValue());

    SLOCK(screen);
    Uint32 borderColor = SDL_MapRGBA(screen->format, 0, 0, 0,
                                     PROCESS_ALPHA(60,
                                                   widget->getAlphaValue()));

    DrawLine(screen, -1, 0, maxWidth, 0, borderColor);
    DrawLine(screen, 0, 0, 0, maxHeight, borderColor);
    DrawLine(screen, maxWidth, 0, maxWidth, maxHeight, borderColor);
    DrawLine(screen, 0, maxHeight, maxWidth, maxHeight, borderColor);
    SUNLOCK(screen);
}

void
aedTheme::drawSlider(aedSlider * widget, SDL_Surface * screen)
{
    Uint16 maxHeight = screen->h - 1;
    Uint16 maxWidth = screen->w - 1;
    SDL_Color c1 = colorButtonPushed1;
    SDL_Color c2 = colorButtonPushed2;
    SDL_Rect rect = { 1, 1, screen->w - 2, screen->h - 2 };

    if(widget->getOrientation() == AED_VERTICAL)
        horizgradient2(screen, rect, c1, c2, widget->getAlphaValue());
    else
        vertgradient2(screen, rect, c1, c2, widget->getAlphaValue());

    // We have to stop using the button's color for everything...
    c1.r = 21, c1.g = 45, c1.b = 72;
    c2 = c1;
    c2.b = 242;
    c2.g = 178;

    rect = widget->getElevatorRect().getSDLRect();
    rect.x -= widget->getRealPos().getX();
    rect.y -= widget->getRealPos().getY();

    // draw the elevator
    vertgradient2(screen, rect, c1, c2, widget->getAlphaValue());

    SLOCK(screen);
    Uint32 borderColor = SDL_MapRGBA(screen->format, 0, 0, 0,
                                     PROCESS_ALPHA(60,
                                                   widget->getAlphaValue()));

    DrawLine(screen, -1, 0, maxWidth, 0, borderColor);
    DrawLine(screen, 0, 0, 0, maxHeight, borderColor);
    DrawLine(screen, maxWidth, 0, maxWidth, maxHeight, borderColor);
    DrawLine(screen, 0, maxHeight, maxWidth, maxHeight, borderColor);

    SUNLOCK(screen);
}

void
aedTheme::drawFrame(aedFrame * widget, SDL_Surface * screen)
{
    Uint16 minY = 0;
    Uint16 maxHeight = screen->h - 1;
    Uint16 maxWidth = screen->w - 1;
    aedFont *font = widget->getFont();

    if(widget->getCaption().size() > 0)
        minY = font->getHeight() / 2;

    SLOCK(screen);
    Uint32 borderColor = SDL_MapRGBA(screen->format, 0, 0, 0,
                                     PROCESS_ALPHA(60,
                                                   widget->getAlphaValue()));

    DrawPixel(screen, 2, minY + 2, borderColor);
    DrawPixel(screen, 2, maxHeight - 2, borderColor);
    DrawPixel(screen, maxWidth - 2, minY + 2, borderColor);
    DrawPixel(screen, maxWidth - 2, maxHeight - 2, borderColor);

    DrawLine(screen, 2, minY + 1, maxWidth - 3, minY + 1, borderColor);
    DrawLine(screen, 1, minY + 2, 1, maxHeight - 3, borderColor);
    DrawLine(screen, maxWidth - 1, minY + 2, maxWidth - 1, maxHeight - 3,
             borderColor);
    DrawLine(screen, 2, maxHeight - 1, maxWidth - 3, maxHeight - 1,
             borderColor);

    borderColor =
        SDL_MapRGBA(screen->format, 255, 255, 255,
                    PROCESS_ALPHA(255, widget->getAlphaValue()));
    DrawLine(screen, 2, minY + 2, 2, maxHeight - 3, borderColor);
    DrawLine(screen, maxWidth - 2, minY + 2, maxWidth - 2, maxHeight - 3,
             borderColor);
    DrawLine(screen, 2, minY + 2, maxWidth - 3, minY + 2, borderColor);
    DrawLine(screen, 2, maxHeight - 2, maxWidth - 3, maxHeight - 2,
             borderColor);

    SUNLOCK(screen);

    if(widget->getCaption().size() > 0)
    {
        SDL_Rect rect = { 0, 0, screen->w, screen->h };

        aedColor m_TextColor(0, 0, 0, 255);

        rect.x = 11;
        font->getTextSize(widget->getCaption().c_str(), &rect.w, &rect.h);

        SDL_FillRect(screen, &rect, widget->getBGColor().getUint32(screen));

        font->renderTextBlended(screen, rect.x, rect.y,
                                widget->getCaption().c_str(), m_TextColor);
    }
}

void
aedTheme::drawBox(aedWidget * widget, SDL_Surface * screen)
{
    SDL_Rect rect;

    if(widget->getBorder() == AED_BORDER_ROUND)
    {
        rect.x = 3;
        rect.y = 3;
        rect.w = screen->w - 6;
        rect.h = screen->h - 6;
    }
    else
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = screen->w;
        rect.h = screen->h;
    }

    aedColor thisColor = colorBoxBackground;

    thisColor.setA(PROCESS_ALPHA(thisColor.getA(), widget->getAlphaValue()));
    SDL_FillRect(screen, &rect, thisColor.getUint32(screen));
}

void
aedTheme::drawBorder(aedWidget * widget, SDL_Surface * screen)
{
    Uint16 maxWidth = screen->w - 1;
    Uint16 maxHeight = screen->h - 1;
    Uint32 borderColor;

    SLOCK(screen);

    if(widget->hasFocus() && widget->isEnabled())
    {
        borderColor = SDL_MapRGBA(screen->format, 80, 80, 255,
                                  PROCESS_ALPHA(255, widget->getAlphaValue()));
    }
    else
    {
        borderColor = SDL_MapRGBA(screen->format, 0, 0, 0,
                                  PROCESS_ALPHA(60, widget->getAlphaValue()));
    }

    if(widget->getBorder() == AED_BORDER_ROUND)
    {
        DrawPixel(screen, 2, 2, borderColor);
        DrawPixel(screen, 2, maxHeight - 2, borderColor);
        DrawPixel(screen, maxWidth - 2, 2, borderColor);
        DrawPixel(screen, maxWidth - 2, maxHeight - 2, borderColor);

        DrawLine(screen, 2, 1, maxWidth - 3, 1, borderColor);
        DrawLine(screen, 1, 2, 1, maxHeight - 3, borderColor);
        DrawLine(screen, maxWidth - 1, 2, maxWidth - 1, maxHeight - 3,
                 borderColor);
        DrawLine(screen, 2, maxHeight - 1, maxWidth - 3, maxHeight - 1,
                 borderColor);

        borderColor =
            SDL_MapRGBA(screen->format, 255, 255, 255,
                        PROCESS_ALPHA(255, widget->getAlphaValue()));
        DrawLine(screen, 2, 2, 2, maxHeight - 3, borderColor);
        DrawLine(screen, maxWidth - 2, 2, maxWidth - 2, maxHeight - 3,
                 borderColor);
        DrawLine(screen, 2, 2, maxWidth - 3, 2, borderColor);
        DrawLine(screen, 2, maxHeight - 2, maxWidth - 3, maxHeight - 2,
                 borderColor);
    }
    else
    {
        /* AED_BORDER_SQUARED */
        DrawLine(screen, -1, 0, maxWidth, 0, borderColor);
        DrawLine(screen, 0, 0, 0, maxHeight, borderColor);
        DrawLine(screen, maxWidth, 0, maxWidth, maxHeight, borderColor);
        DrawLine(screen, 0, maxHeight, maxWidth, maxHeight, borderColor);
    }

    SUNLOCK(screen);

#if 0
    //    SDL_FillRect(screen, NULL, widget->getBGColor().getUint32(screen));
    SLOCK(screen);
    DrawLine(screen, 0, 0, screen->w, 0,
             SDL_MapRGBA(screen->format, 213, 214, 205, 255));
    DrawLine(screen, 0, -1, 0, screen->h - 1,
             SDL_MapRGBA(screen->format, 213, 214, 205, 255));
    DrawLine(screen, 1, 1, screen->w - 2, 1,
             SDL_MapRGBA(screen->format, 172, 170, 164, 255));
    DrawLine(screen, 1, 0, 1, screen->h - 2,
             SDL_MapRGBA(screen->format, 172, 170, 164, 255));
    DrawLine(screen, 1, screen->h - 2, screen->w - 2, screen->h - 2,
             SDL_MapRGBA(screen->format, 246, 246, 238, 255));
    SUNLOCK(screen);
#endif

}

void
aedTheme::drawButton(aedButton * widget, SDL_Surface * screen)
{
    Uint16 maxWidth = screen->w - 1;
    Uint16 maxHeight = screen->h - 1;
    Uint32 borderColor = 0;
    bool btnDown = widget->getButtonState();

    SLOCK(screen);
    if(widget->isEnabled() && widget->hasFocus())       // We have Focus
    {
        borderColor =
            SDL_MapRGBA(screen->format, 80, 80, 255,
                        PROCESS_ALPHA(255, widget->getAlphaValue()));
        DrawPixel(screen, 2, 2, borderColor);
        DrawPixel(screen, 2, maxHeight - 2, borderColor);
        DrawPixel(screen, maxWidth - 2, 2, borderColor);
        DrawPixel(screen, maxWidth - 2, maxHeight - 2, borderColor);

        DrawLine(screen, 2, 1, maxWidth - 3, 1, borderColor);
        DrawLine(screen, 1, 2, 1, maxHeight - 3, borderColor);
        DrawLine(screen, maxWidth - 1, 2, maxWidth - 1, maxHeight - 3,
                 borderColor);
        DrawLine(screen, 2, maxHeight - 1, maxWidth - 3, maxHeight - 1,
                 borderColor);
    }
    else
    {
        // this has to change
        borderColor =
            SDL_MapRGBA(screen->format, 0, 0, 0,
                        PROCESS_ALPHA(60, widget->getAlphaValue()));
        DrawPixel(screen, 2, 2, borderColor);
        DrawPixel(screen, 2, maxHeight - 2, borderColor);
        DrawPixel(screen, maxWidth - 2, 2, borderColor);
        DrawPixel(screen, maxWidth - 2, maxHeight - 2, borderColor);

        DrawLine(screen, 2, 1, maxWidth - 3, 1, borderColor);
        DrawLine(screen, 1, 2, 1, maxHeight - 3, borderColor);
        DrawLine(screen, maxWidth - 1, 2, maxWidth - 1, maxHeight - 3,
                 borderColor);
        DrawLine(screen, 2, maxHeight - 1, maxWidth - 3, maxHeight - 1,
                 borderColor);
    }

    // Internal Borders
    borderColor =
        SDL_MapRGBA(screen->format, 255, 255, 255,
                    PROCESS_ALPHA(255, widget->getAlphaValue()));
    DrawLine(screen, maxWidth - 2, 2, maxWidth - 2, maxHeight - 3, borderColor);
    DrawLine(screen, 2, maxHeight - 2, maxWidth - 3, maxHeight - 2,
             borderColor);

    borderColor =
        SDL_MapRGBA(screen->format, 238, 238, 230,
                    PROCESS_ALPHA(255, widget->getAlphaValue()));
    DrawLine(screen, 2, 2, 2, maxHeight - 3, borderColor);

    if(btnDown)
    {
        // External Shadow
        borderColor =
            SDL_MapRGBA(screen->format, 0, 0, 0,
                        PROCESS_ALPHA(25, widget->getAlphaValue()));
        DrawLine(screen, 2, 0, maxWidth - 4, 0, borderColor);
        DrawLine(screen, 0, 2, 0, maxHeight - 4, borderColor);
        DrawPixel(screen, 1, 2, borderColor);
        DrawPixel(screen, 2, 1, borderColor);
    }
    else
    {
        // External Shadow
        borderColor =
            SDL_MapRGBA(screen->format, 0, 0, 0,
                        PROCESS_ALPHA(25, widget->getAlphaValue()));
        DrawLine(screen, 3, maxHeight, maxWidth - 3, maxHeight, borderColor);
        DrawLine(screen, maxWidth, 3, maxWidth, maxHeight - 3, borderColor);
        DrawPixel(screen, maxWidth - 1, maxHeight - 2, borderColor);
        DrawPixel(screen, maxWidth - 2, maxHeight - 1, borderColor);

    }

    // Color "rendering"
    //
    SDL_Rect gradRect;

    gradRect.x = 3;
    gradRect.y = 3;
    gradRect.w = maxWidth - 5;
    gradRect.h = maxHeight - 5;

    SDL_Color c1, c2;

    if(btnDown)
    {
        c1 = colorButtonPushed1;
        c2 = colorButtonPushed2;
    }
    else if(!widget->isMouseOver())
    {
        c1 = colorButtonNormal1;
        c2 = colorButtonNormal2;
    }
    else
    {
        c1 = colorButtonOver1;
        c2 = colorButtonOver2;
    }

    SUNLOCK(screen);
    vertgradient2(screen, gradRect, c1, c2, widget->getAlphaValue());
}

void
aedTheme::drawSquareButton(aedButton * widget, SDL_Surface * screen)
{
    SDL_Color c1, c2;
    SDL_Rect rect;
    bool btnDown = widget->getButtonState();

    if(btnDown)
    {
        c1 = colorButtonPushed1;
        c2 = colorButtonPushed2;
    }
    else if(!widget->isMouseOver())
    {
        c1 = colorButtonNormal1;
        c2 = colorButtonNormal2;
    }
    else
    {
        c1 = colorButtonOver1;
        c2 = colorButtonOver2;
    }


    rect.x = 1;
    rect.y = 1;
    rect.w = screen->w - 2;
    rect.h = screen->h - 2;

    Uint16 maxHeight = screen->h - 1;
    Uint16 maxWidth = screen->w - 1;

    SLOCK(screen);
    Uint32 borderColor;

    if(widget->isEnabled() && widget->hasFocus())
    {
        borderColor = SDL_MapRGBA(screen->format, 80, 80, 255,
                                  PROCESS_ALPHA(255, widget->getAlphaValue()));
    }
    else
    {
        borderColor = SDL_MapRGBA(screen->format, 0, 0, 0,
                                  PROCESS_ALPHA(60, widget->getAlphaValue()));
    }

    DrawLine(screen, -1, 0, maxWidth, 0, borderColor);
    DrawLine(screen, 0, 0, 0, maxHeight, borderColor);
    DrawLine(screen, maxWidth, 0, maxWidth, maxHeight, borderColor);
    DrawLine(screen, 0, maxHeight, maxWidth, maxHeight, borderColor);
    SUNLOCK(screen);

    vertgradient2(screen, rect, c1, c2, widget->getAlphaValue());
}

#ifndef AEDTHEME_H
#define AEDTHEME_H

#include <string>
#include "dllmacros.h"
#include "drawlibs.h"
#include "aedColor.h"

class aedColor;
class aedWidget;
class aedButton;
class aedStaticText;
class aedTextBox;
class aedDialogBox;
class aedProgressBar;
class aedFrame;
class aedCheckBox;
class aedSlider;
class aedScrollBar;
class aedRadioButtonGroup;
class aedListBox;

// This has to be modified after every breakage of
// binary compatibility of the plugin interface.
#define AEDGUI_PIF_VERSION (0 * 100 + 1 * 10 + 7)

#define DRAWFUNC(x) virtual void draw##x(aed##x *widget, SDL_Surface *screen)

struct aedThemeInfo
{
    std::string name;
    std::string author;
    std::string description;
};

class DLLEXPORT aedTheme
{
  public:
    aedTheme();
    virtual ~ aedTheme();

    const aedThemeInfo *getThemeInfo()
    {
        return &m_ThemeInfo;
    }

    // Font properties
    std::string defaultFontName;
    Uint8 defaultFontSize;
    aedColor defaultTextColor;

    // Colors
    aedColor colorBoxBackground;
    aedColor colorButtonNormal1;
    aedColor colorButtonNormal2;
    aedColor colorButtonPushed1;
    aedColor colorButtonPushed2;
    aedColor colorButtonOver1;
    aedColor colorButtonOver2;

    aedColor colorDefaultWidgetBackground;

    // Drawing functions
    DRAWFUNC(Button);
    DRAWFUNC(Slider);
    DRAWFUNC(ScrollBar);
    DRAWFUNC(Frame);
    DRAWFUNC(ProgressBar);
    DRAWFUNC(CheckBox);

    virtual void drawBorder(aedWidget * widget, SDL_Surface * screen);
    virtual void drawBox(aedWidget * widget, SDL_Surface * screen);
    virtual void drawSquareButton(aedButton * widget, SDL_Surface * screen);

  protected:
    aedThemeInfo m_ThemeInfo;
};

#define PROCESS_ALPHA(a,b) ( (Uint8) ((((Uint32)a * b) >> 8) & 0xff) )

#endif /* AEDTHEME_H */

#include "aedTheme.h"
#include "aedButton.h"
#include "aedFrame.h"
#include "aedSlider.h"
#include "aedScrollBar.h"
#include "drawlibs.h"

class DLLEXPORT industrialStyle:public aedTheme
{
  public:
    industrialStyle();
//      void drawButton(aedButton * widget, SDL_Surface * screen);
};

extern "C" DLLEXPORT aedTheme *
get_theme_instance()
{
    return new industrialStyle;
}

extern "C" DLLEXPORT Uint16
get_pif_version()
{
    return AEDGUI_PIF_VERSION;
}

industrialStyle::industrialStyle()
{
    m_ThemeInfo.name = "Industrial Style";
    m_ThemeInfo.author = "The aedGUI Team";
    m_ThemeInfo.description = "Industrial Style theme for aedGUI";

    defaultFontName = "";
    defaultFontSize = 8;
    defaultTextColor.setAllColors(0, 0, 0, 255);

    colorDefaultWidgetBackground.setAllColors(246, 246, 246, 255);
    colorBoxBackground.setAllColors(255, 255, 255, 255);
    colorButtonNormal1.setAllColors(246, 246, 246, 255);
    colorButtonNormal2.setAllColors(236, 236, 236, 255);
    colorButtonPushed1.setAllColors(222, 228, 222, 255);
    colorButtonPushed2.setAllColors(212, 208, 212, 255);
    colorButtonOver1.setAllColors(255, 255, 255, 255);
    colorButtonOver2.setAllColors(245, 245, 245, 255);
}

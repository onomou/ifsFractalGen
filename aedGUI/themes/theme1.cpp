#include "aedTheme.h"
#include "aedButton.h"
#include "aedFrame.h"
#include "aedSlider.h"
#include "aedScrollBar.h"
#include "drawlibs.h"

class DLLEXPORT myTheme:public aedTheme
{
  public:
    myTheme();
};

extern "C" DLLEXPORT aedTheme *
get_theme_instance()
{
    return new myTheme;
}

extern "C" DLLEXPORT Uint16
get_pif_version()
{
    return AEDGUI_PIF_VERSION;
}

myTheme::myTheme()
{
    // Fill in the theme info struct
    m_ThemeInfo.name = "theme1";
    m_ThemeInfo.author = "The aedGUI team";
    m_ThemeInfo.description = "This is a test theme for aedGUI";

    defaultFontName = "";
    defaultFontSize = 8;
    defaultTextColor.setAllColors(255, 0, 0, 255);

    colorBoxBackground.setAllColors(255, 255, 255, 255);
    colorButtonNormal1.setAllColors(255, 255, 255, 255);
    colorButtonNormal2.setAllColors(230, 230, 230, 255);
    colorButtonPushed1.setAllColors(210, 210, 210, 255);
    colorButtonPushed2.setAllColors(240, 240, 240, 255);
    colorButtonOver1.setAllColors(255, 255, 255, 255);
    colorButtonOver2.setAllColors(246, 242, 238, 255);
}

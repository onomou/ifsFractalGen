#ifndef AEDAPP_H
#define AEDAPP_H

#include <iostream>
#include <vector>
#include <string>

#include "dllmacros.h"
#include "aedFont.h"
#include "aedWidget.h"
#include "aedLog.h"
#include "aedTheme.h"

struct DLLEXPORT aedFontEntry
{
    std::string full_path;
    std::string name;
    Uint8 ptsize;
    class aedFont *font_ptr;
};

struct DLLEXPORT aedMemFontEntry
{
    std::string name;
    unsigned char *data;
    Uint32 data_size;
    Uint8 ptsize;
    class aedFont *font_ptr;
};

struct DLLEXPORT aedThemeEntry
{
    void *handle;
    aedTheme *theme;
};

/**
 * The application class.
 * 
 * The aedApp class enables you to load custom themes and use memory fonts,
 * set the application's root widget, modify aedGUI's logging behaviour etc.
 * You have to create only one object of this class and it must happen before
 * any widgets are created.
 */
class DLLEXPORT aedApp
{
  public:

    /** The default constructor. */
    aedApp();
    ~aedApp();

    /**
     * Search global font cache for the specified font.
     * If the font isn't present in the cache or in the
     * list of memory fonts, disk is searched.
     * 
     * @param name font name
     * @param size font size
     * @param style font style
     * @return pointer to the requested font or NULL if the font
     * was not found
     */
    aedFont *requestFont(std::string name, Uint8 size, int style =
                         AED_FONT_STYLE_NORMAL);

    /** Add font path to the list of paths which will be used
     * for searching fonts.
     * 
     * @param path path to the font files
     * @see requestFont
     */
    void addFontPath(std::string path);

    /** Register a memory font for use by aedGUI.
     * 
     * @param name name under which the font will be known
     * @param data pointer to the font data
     * @param size size of the font data
     */
    void addMemoryFont(std::string name, unsigned char *data,
                       unsigned long int size);

    /** Dump all font paths to std::cerr. */
    void dumpFontPaths();

    /** Set the default font name. 
     * 
     * @param name font name
     */
    void setDefaultFontName(std::string name)
    {
        m_DefaultFontName = name;
    }

    /** Return the default font name. */
    std::string getDefaultFontName() const
    {
        return m_DefaultFontName;
    }

    /** Set the application's root widget. */
    void setRoot(aedWidget * w)
    {
        m_Root = w;
    }

    /** Get the root widget. */
    aedWidget *getRoot() const
    {
        return m_Root;
    }

    aedLog *getLog() const;

    /** Load a theme plugin.
     * 
     * @param file filename of the theme plugin
     * @return pointer to the loaded theme or NULL if 
     * the theme couldn't be loaded
     */
    aedTheme *loadTheme(std::string file);

    /** Unload the theme and free all resources allocated to it.
     * @param theme pointer to a theme returned previously by
     * loadTheme()
     */
    void unloadTheme(aedTheme * theme);

    /** Return a pointer to the default theme. */
    aedTheme *getDefaultTheme() const;

    /** Return a pointer to the widget that has focus. */
    aedWidget *getFocusWidget() const
    {
        return m_FocusWidget;
    }

    /** Set focus. */
    void setFocusWidget(aedWidget * win);

    /** Handle okay and cancel buttons. */
    void setOkayButton(aedButton * button)
    {
        m_okayButton = button;
    }
    void setCancelButton(aedButton * button)
    {
        m_cancelButton = button;
    }
    aedButton *getOkayButton()
    {
        return m_okayButton;
    }
    aedButton *getCancelButton()
    {
        return m_cancelButton;
    }
    void clickOkayButton();
    void clickCancelButton();

    /** Enable or disable UTF8 text rendering.
     * Rendering UTF8 text is disabled by default. */
    void setUTF8(bool enable)
    {
        m_UTF8 = enable;
    }
    bool getUTF8() const
    {
        return m_UTF8;
    }

  private:
      std::string tryFontPath(std::string path, std::string name);

  private:
    std::vector < aedFontEntry > m_Fonts;       // opened fonts
    std::vector < aedMemFontEntry > m_MemFonts; // memory fonts ready to use
    std::vector < std::string > m_FontPaths;    // font paths
    std::vector < aedThemeEntry > m_Themes;     // loaded themes
    std::string m_DefaultFontName;
    aedWidget *m_Root;
    aedWidget *m_FocusWidget;
    aedButton *m_okayButton;
    aedButton *m_cancelButton;

    bool m_UTF8;
};

#ifdef AEDGUI
// Global pointer to the per-process aedApp object
extern DLLEXPORT aedApp *pApp;
#endif

#endif /* AEDAPP_H */

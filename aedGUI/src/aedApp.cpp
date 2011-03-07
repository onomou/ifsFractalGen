#include <cstdio>
#include <cctype>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "aedApp.h"
#include "aedLog.h"
#include "aedUpdateManager.h"
#include "aedWidget.h"
#include "aedButton.h"
#include "SDL_ttf.h"

#ifdef SDL_1_2_5
#include "SDL_loadso.c"
#else
#include "SDL_loadso.h"
#endif

#if !defined(THEME_EXT) && defined(WIN32)
#define THEME_EXT ".dll"
#endif


DLLEXPORT aedApp *pApp = NULL;
DLLEXPORT aedLog *pLog = NULL;
DLLEXPORT aedUpdateManager *pUpdateMgr = NULL;

aedApp::aedApp()
{
    if(pApp)
    {
        std::cerr << "Attempt to create another aedApp object!\n";
        exit(1);
    }

    pApp = this;
    m_Root = NULL;
    m_FocusWidget = NULL;
    m_okayButton = NULL;
    m_cancelButton = NULL;
    m_UTF8 = false;

    pLog = new aedLog;
    pUpdateMgr = new aedUpdateManager;

    // Initialize SDL_ttf
    if(TTF_Init() < 0)
        pLog->fatal("Couldn't init FreeType engine, exiting.\n");

    // Default font paths
    addFontPath("./");

    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    // Create the default theme (it's always in m_Themes[0])
    aedThemeEntry te;

    te.handle = NULL;
    te.theme = new aedTheme;
    m_Themes.push_back(te);
}

// Cleanup everything and give statistics about used fonts and themes
aedApp::~aedApp()
{
    Uint16 i, nmfe = 0;

    m_Root = NULL;
    m_FocusWidget = NULL;

    // m_Themes[0] is the default theme and can't be unloaded so we just
    // free the memory associated with it
    delete m_Themes[0].theme;

    for(i = 1; i < m_Themes.size(); i++)
    {
        delete m_Themes[i].theme;

        SDL_UnloadObject(m_Themes[i].handle);
    }
    pLog->notice("Themes used (including the default one): %d\n",
                 m_Themes.size());
    m_Themes.clear();

    for(i = 0; i < m_Fonts.size(); i++)
    {
        m_Fonts[i].font_ptr->closeFont();
        delete m_Fonts[i].font_ptr;
    }

    for(i = 0; i < m_MemFonts.size(); i++)
    {
        if(m_MemFonts[i].font_ptr)
        {
            m_MemFonts[i].font_ptr->closeFont();
            delete m_MemFonts[i].font_ptr;

            nmfe++;
        }
    }

    pLog->notice("Fonts used: %d from disk, %d(%d) from memory\n",
                 m_Fonts.size(), nmfe, m_MemFonts.size());

    m_Fonts.clear();
    m_MemFonts.clear();

    TTF_Quit();

    delete pLog;

    pLog = NULL;

    delete pUpdateMgr;

    pUpdateMgr = NULL;

    pApp = NULL;
}

void
aedApp::setFocusWidget(aedWidget * win)
{
    if(win && !win->canFocus())
        win = NULL;

    if(m_FocusWidget != NULL && m_FocusWidget != m_Root)
    {
        // tell the old widget that it lost focus
        m_FocusWidget->setActiveBorder(false);
        m_FocusWidget->setRenderState(true);
        m_FocusWidget->wm_lostfocus();
        m_FocusWidget->triggerEvent(LOST_FOCUS, m_FocusWidget, NULL);
    }

    m_FocusWidget = win;

    if(m_FocusWidget && m_FocusWidget != m_Root)
    {
        m_FocusWidget->setActiveBorder(true);
        m_FocusWidget->setRenderState(true);
        m_FocusWidget->wm_gotfocus();
        m_FocusWidget->triggerEvent(GOT_FOCUS, m_FocusWidget, NULL);
    }
}

void
aedApp::clickOkayButton()
{
    if(m_okayButton)
        m_okayButton->click();
}

void
aedApp::clickCancelButton()
{
    if(m_cancelButton)
        m_cancelButton->click();
}

aedTheme *
aedApp::loadTheme(std::string file)
{
    void *handle;
    aedTheme *p;
    Uint16 pif_version;
    aedTheme *(*get_theme_instance) ();

    Uint16(*get_pif_version) ();

    handle = SDL_LoadObject(file.c_str());
    if(!handle)
    {
        // Try adding extension to the filename
        std::string fext = file + THEME_EXT;
        handle = SDL_LoadObject(fext.c_str());
        if(!handle)
        {
            pLog->error("Couldn't load theme %s (%s)\n", file.c_str(),
                        SDL_GetError());
            return NULL;
        }
        file += THEME_EXT;
    }

    // check if there's a version mismatch
    get_pif_version = (Uint16(*)())SDL_LoadFunction(handle, "get_pif_version");
    if(get_pif_version == NULL)
    {
        pLog->error("Couldn't load get_pif_version from %s (%s)\n",
                    file.c_str(), SDL_GetError());
        SDL_UnloadObject(handle);
        return NULL;
    }

    pif_version = get_pif_version();
    if(pif_version != AEDGUI_PIF_VERSION)
    {
        pLog->error("Theme %s has interface version %d, should have"
                    " %d instead.\n", file.c_str(), pif_version,
                    AEDGUI_PIF_VERSION);
        SDL_UnloadObject(handle);
        return NULL;
    }

    get_theme_instance =
        (aedTheme * (*)())SDL_LoadFunction(handle, "get_theme_instance");
    if(!get_theme_instance)
    {
        pLog->error("Couldn't load get_theme_instance from %s (%s)\n",
                    file.c_str(), SDL_GetError());
        SDL_UnloadObject(handle);
        return NULL;
    }

    // get newly allocated theme
    p = get_theme_instance();

    aedThemeEntry te;

    te.handle = handle;
    te.theme = p;
    m_Themes.push_back(te);

    if(p->defaultFontName.empty() && !m_DefaultFontName.empty())
        p->defaultFontName = m_DefaultFontName;

    pLog->notice("Loaded theme %s\n", file.c_str());
    return p;
}

void
aedApp::unloadTheme(aedTheme * theme)
{
    size_t i;
    size_t nThemeCount = m_Themes.size();

    for(i = 0; i < nThemeCount; i++)
    {
        if(m_Themes[i].theme == theme)
            break;
    }

    if(i == nThemeCount)
        pLog->warning("Trying to unload theme which is not loaded: %p\n",
                      theme);
    else
    {
        SDL_UnloadObject(m_Themes[i].handle);
        m_Themes.erase(m_Themes.begin() + i);
    }
}

aedTheme *
aedApp::getDefaultTheme() const
{
    return m_Themes[0].theme;
}

aedLog *
aedApp::getLog() const
{
    return pLog;
}

void
aedApp::addFontPath(std::string path)
{
    pLog->notice("Adding font path '%s'\n", path.c_str());
    m_FontPaths.push_back(path);
}

void
aedApp::addMemoryFont(std::string name, unsigned char *data,
                      unsigned long int size)
{
    aedMemFontEntry mfe;

    pLog->notice("Adding memory font: %s at 0x%x, size %lu bytes\n",
                 name.c_str(), (unsigned long) data, size);
    mfe.name = name;
    mfe.data = data;
    mfe.data_size = size;
    mfe.ptsize = 0;
    mfe.font_ptr = NULL;
    m_MemFonts.push_back(mfe);
}

void
aedApp::dumpFontPaths()
{
    Uint16 i;

    pLog->notice("Dumping font paths:\n");
    for(i = 0; i < m_FontPaths.size(); i++)
        pLog->notice("    %s\n", m_FontPaths[i].c_str());
}

std::string aedApp::tryFontPath(std::string path, std::string name)
{
    int
        i = int (name.find_last_of("."));

    if(i == -1 || name.substr(i, 4) != ".ttf")
        name.append(".ttf");

    path.append(name);

    FILE *
        fp = fopen(path.c_str(), "rb");

    if(fp)
    {
        fclose(fp);
        return path;
    }

    return "";
}

aedFont *
aedApp::requestFont(std::string name, Uint8 size, int style)
{
    Uint16 i;

    // Sanity check
    if(name == "")
    {
        pLog->fatal("requested font \"\", perhaps you forgot to call "
                    "aedApp::setDefaultFontName()?\n");
    }

    if(size == 0)
        pLog->fatal("requested font with size 0!\n");

    // First try list of currently opened fonts
    for(i = 0; i < m_Fonts.size(); i++)
    {
        if(m_Fonts[i].name == name)
        {
            if(m_Fonts[i].ptsize == size &&
               m_Fonts[i].font_ptr->getStyle() == style)
                return m_Fonts[i].font_ptr;
        }
    }
    for(i = 0; i < m_Fonts.size(); i++)
    {
        if(m_Fonts[i].name == name)
        {
            // We have to reopen the same font with a different size or style
            pLog->notice("Font entry (%s, %d, %d) partially found in cache, "
                         "loading new entry.\n", name.c_str(), size, style);

            aedFontEntry fe;

            fe.full_path = m_Fonts[i].full_path;
            fe.name = name;
            fe.ptsize = size;
            fe.font_ptr = new aedFont();
            if(!fe.font_ptr->openFont(fe.full_path.c_str(), size))
            {
                pLog->error("Couldn't open font %s\n", fe.full_path.c_str());
                delete fe.font_ptr;

                return NULL;
            }
            fe.font_ptr->setStyle(style);

            m_Fonts.push_back(fe);
            return fe.font_ptr;
        }
    }

    // Try memory fonts then
    Sint16 last_index = -1;

    for(i = 0; i < m_MemFonts.size(); i++)
    {
        if(m_MemFonts[i].name == name)
        {
            last_index = i;
            if(m_MemFonts[i].ptsize == size &&
               m_MemFonts[i].font_ptr->getStyle() == style)
                return m_MemFonts[i].font_ptr;
        }
    }

    if(last_index != -1)
    {
        i = last_index;
        pLog->notice("Font entry (%s, %d, %d) not found in cache, "
                     "loading from memory.\n", name.c_str(), size, style);
        aedMemFontEntry mfe;

        mfe.name = name;
        mfe.data = m_MemFonts[i].data;
        mfe.data_size = m_MemFonts[i].data_size;
        mfe.ptsize = size;
        mfe.font_ptr = new aedFont();
        if(!mfe.font_ptr->openFont(mfe.data, mfe.data_size, mfe.ptsize))
        {
            pLog->error("Couldn't create memory font (%s, %d, %d)\n",
                        name.c_str(), size, style);
            delete mfe.font_ptr;

            return NULL;
        }
        mfe.font_ptr->setStyle(style);

        m_MemFonts.push_back(mfe);
        return mfe.font_ptr;
    }

    // Finally, try all font paths...
    pLog->notice("Font entry (%s, %d, %d) not found in cache, searching "
                 "font paths.\n", name.c_str(), size, style);

    for(i = 0; i < m_FontPaths.size(); i++)
    {
        std::string path = tryFontPath(m_FontPaths[i], name);
        if(path != "")
        {
            aedFontEntry fe;

            fe.full_path = path;
            fe.name = name;
            fe.ptsize = size;
            fe.font_ptr = new aedFont();
            if(!fe.font_ptr->openFont(fe.full_path.c_str(), size))
            {
                pLog->error("Couldn't open font %s\n", fe.full_path.c_str());
                delete fe.font_ptr;

                return NULL;
            }
            fe.font_ptr->setStyle(style);

            pLog->notice("New entry (%s, %d, %d) created from %s\n",
                         name.c_str(), size, style, fe.full_path.c_str());
            m_Fonts.push_back(fe);
            return fe.font_ptr;
        }
    }

    pLog->fatal("Couldn't find font %s\n", name.c_str());
    return NULL;
}

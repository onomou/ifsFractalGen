#include "aedApp.h"
#include "aedStaticRTF.h"

#ifdef HAVE_SDL_RTF

#include "SDL_rtf.h"

static void *
CreateFont(const char *name, RTF_FontFamily family, int size, int style)
{
    int font_style = AED_FONT_STYLE_NORMAL;

    if(style & RTF_FontBold)
        font_style |= AED_FONT_STYLE_BOLD;
    if(style & RTF_FontItalic)
        font_style |= AED_FONT_STYLE_ITALIC;
    if(style & RTF_FontUnderline)
        font_style |= AED_FONT_STYLE_UNDERLINE;

    return pApp->requestFont(pApp->getDefaultFontName(), size, font_style);
}

static int
GetLineSpacing(void *_font)
{
    aedFont *font = (aedFont *) _font;

    return font->getLineSkip();
}

static int
GetCharacterOffsets(void *_font, const char *text, int *byteOffsets,
                    int *pixelOffsets, int maxOffsets)
{
    aedFont *font = (aedFont *) _font;
    int i = 0;
    int bytes = 0;
    int pixels = 0;
    int advance;
    Uint16 ch;

    while(*text && i < maxOffsets)
    {
        byteOffsets[i] = bytes;
        pixelOffsets[i] = pixels;
        ++i;

        ch = aedFont::getUnicode(text, &advance);
        text += advance;
        bytes += advance;
        font->getGlyphMetrics(ch, NULL, NULL, NULL, NULL, &advance);
        pixels += advance;
    }
    if(i < maxOffsets)
    {
        byteOffsets[i] = bytes;
        pixelOffsets[i] = pixels;
    }
    return i;
}

static SDL_Surface *
RenderText(void *_font, const char *text, SDL_Color fg)
{
    aedFont *font = (aedFont *) _font;

    return font->renderTextBlended(text, aedColor(fg.r, fg.g, fg.b, 255));
}

static void
FreeFont(void *_font)
{
}

aedStaticRTF::aedStaticRTF()
{
    m_IsStatic = true;
    m_doc = NULL;
    m_docOffset = 0.0f;
}

aedStaticRTF::~aedStaticRTF()
{
    if(m_doc)
    {
        RTF_FreeContext(m_doc);
    }
}

bool aedStaticRTF::setDocument(char *text, int textlen)
{
    RTF_FontEngine
        fontEngine;

    if(m_doc)
    {
        RTF_FreeContext(m_doc);
        m_doc = NULL;
        m_docOffset = 0.0f;
    }
    if(!text)
    {
        return true;
    }
    if(!m_doc)
    {
        fontEngine.version = RTF_FONT_ENGINE_VERSION;
        fontEngine.CreateFont = CreateFont;
        fontEngine.GetLineSpacing = GetLineSpacing;
        fontEngine.GetCharacterOffsets = GetCharacterOffsets;
        fontEngine.RenderText = RenderText;
        fontEngine.FreeFont = FreeFont;
        m_doc = RTF_CreateContext(&fontEngine);
        if(m_doc == NULL)
        {
            return false;
        }
    }
    if(RTF_Load_RW(m_doc, SDL_RWFromMem(text, textlen), 1) < 0)
    {
        RTF_FreeContext(m_doc);
        m_doc = NULL;
        return false;
    }
    return true;
}

int
aedStaticRTF::getDocumentHeight(int width)
{
    int height = 0;

    if(m_doc)
    {
        height = RTF_GetHeight(m_doc, width);
    }
    return height;
}

void
aedStaticRTF::setDocumentOffset(float offset)
{
    m_docOffset = offset;
    setRenderState(true);
}

void
aedStaticRTF::render(void)
{
    int height = getDocumentHeight(m_Surface->w);

    if(height > 0)
    {
        int offset = 0;

        if(height > m_Surface->h)
        {
            offset = (int) (m_docOffset * (height - m_Surface->h));
        }
        RTF_Render(m_doc, m_Surface, NULL, offset);
    }
}

#else

/* Stubs for an empty widget if we don't have SDL_rtf support */

struct _RTF_Context
{
};

aedStaticRTF::aedStaticRTF()
{
    m_IsStatic = true;
    m_doc = NULL;
    m_docOffset = 0.0f;
}

aedStaticRTF::~aedStaticRTF()
{
}

bool aedStaticRTF::setDocument(char *text, int textlen)
{
    return false;
}

int
aedStaticRTF::getDocumentHeight(int width)
{
    return 0;
}

void
aedStaticRTF::setDocumentOffset(float offset)
{
}

void
aedStaticRTF::render(void)
{
}

#endif // HAVE_SDL_RTF

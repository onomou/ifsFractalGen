#include "aedDialogBox.h"

void
aedDialogBox::render(void)
{
    SDL_Rect rect = { 0, 0, m_Surface->w, m_Surface->h };
    SDL_FillRect(m_Surface, &rect, 0xFFFFFFFF);
}

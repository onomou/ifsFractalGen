
/*
 * The aedTextBox class
 * This widget provides an entry textbox on screen
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

#ifndef AEDLISTBOX_H
#define AEDLISTBOX_H

#include "aedWidget.h"
#include "aedFont.h"

class DLLEXPORT aedListBox:public aedWidget
{
  public:
    aedListBox();

    virtual void add(std::string string);
    virtual void insert(int line, std::string string);
    virtual void del(int line);
    virtual void del(int startLine, int endLine);
    virtual int size();

    virtual int wm_update(Uint32 msdelta);
    virtual void render(void);

  protected:
      std::vector < std::string > m_Strings;
    int m_currentLine;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedListBox >;
#endif

#endif

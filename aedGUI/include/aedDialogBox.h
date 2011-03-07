
/*
 * The aedDialogBox class
 * This is the base class for all dialog boxes.
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

#ifndef AEDDIALOGBOX
#define AEDDIALOGBOX

#include "dllmacros.h"
#include "aedWidget.h"

class DLLEXPORT aedDialogBox:public aedWidget
{
    virtual void render(void);
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedDialogBox >;
#endif

#endif

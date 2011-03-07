
/*
 * The aedSignal class
 * This class delivers signals from the GUI library.
 * Initial design by Eduardo B. Fonseca <ebf@aedsol.com>
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

#ifndef AEDSIGNAL_H
#define AEDSIGNAL_H

#include <iostream>
#include <vector>
#include "dllmacros.h"
#include "SDL.h"
#include "aedCallBack.h"


class DLLEXPORT aedSignal
{
  public:
    aedSignal();
    aedSignal(aedCallBack & cbf);
    virtual ~ aedSignal();
    void connect(aedCallBack & cbf);
    void trigger(void *data1, void *data2);
  private:
      aedCallBack m_CallbackFunction;

};

#endif

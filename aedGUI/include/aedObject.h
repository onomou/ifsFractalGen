
/*
 * The aedObject class
 * The alfa, the omega, the start and the end. aedGUI's main base class :)
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

#ifndef AEDOBJECT_H
#define AEDOBJECT_H

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <math.h>
#include <typeinfo>
#include "SDL.h"
#include "dllmacros.h"
#include "aedSignal.h"

// Events
typedef enum
{
    // Mouse events
    MOUSE_LBUTTON_DOWN = 1000,
    MOUSE_LBUTTON_UP,
    MOUSE_RBUTTON_DOWN,
    MOUSE_RBUTTON_UP,
    MOUSE_MBUTTON_DOWN,
    MOUSE_MBUTTON_UP,
    MOUSE_MOVE,

    // Focus events
    GOT_FOCUS = 1500,
    LOST_FOCUS,

    // Keyboard events
    KEY_DOWN = 2000,

    // Text events
    TEXT_CHANGED = 3000,

    // Screen events
    SCREEN_RESOLUTION_CHANGED = 4000,

    // State events
    STATE_CHANGED = 5000,

    // Timer Events
    TIMER_ALARM = 6000,

    // Generic "value changed" event (currently used by
    // sliders and scrollbars)
    VALUE_CHANGED = 7000
} aedEvents;

/** Base class of all widgets, provides signal mechanism. */
class DLLEXPORT aedObject
{
  public:
    aedObject();
    virtual ~ aedObject();

    /** Connect an event to a callback. */
    bool connect(aedEvents evt, aedCallBack cbf);
    bool disconnect(aedEvents evt);
    void triggerEvent(aedEvents evt, void *caller, void *data);

  protected:
      std::multimap < aedEvents, aedSignal > m_Signals;
};

#endif


/*
 * The aedTimer class
 * A _very_ simple timer
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

#ifndef AEDTIMER_H
#define AEDTIMER_H

#include "aedObject.h"

/** This class implements a timer. What else could it be? :)
 * The timer is off by default so you have to explicitly call
 * enable(true) to enable it (assuming you previously set the alarm
 * time interval).
 */
class DLLEXPORT aedTimer:public aedObject
{
  public:
    aedTimer();
    ~aedTimer();

    /** Set timer's interval.
     * @param time time in milliseconds */
    void setAlarm(Uint32 time);

    /** Enable/disable timer. */
    void enable(bool state);

  private:
    /* The timer class uses static member function (timer_cb) as the
     * callback for a SDL timer; since static functions don't have 'this'
     * pointer, timer_cb receives it via its second parameter (void *).
     */
    static Uint32 timer_cb(Uint32 interval, void *p);

    bool m_IsEnabled;
    Uint32 m_AlarmTime, m_CurTicks, m_LastTicks;
    SDL_TimerID m_TID;
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedTimer >;
#endif

#endif

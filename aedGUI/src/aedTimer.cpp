#include "aedTimer.h"
#include "aedLog.h"

aedTimer::aedTimer()
{
    m_IsEnabled = false;
    m_AlarmTime = 0;
    m_CurTicks = 0;
    m_LastTicks = 0;

    if(SDL_WasInit(SDL_INIT_TIMER) == 0)
        SDL_InitSubSystem(SDL_INIT_TIMER);
}

aedTimer::~aedTimer()
{
    if(m_IsEnabled)
        SDL_RemoveTimer(m_TID);
}

void
aedTimer::setAlarm(Uint32 time)
{
    m_AlarmTime = time;
}

void
aedTimer::enable(bool state)
{
    if(state != m_IsEnabled)
    {
        if(m_IsEnabled)
            SDL_RemoveTimer(m_TID);

        if(state && m_AlarmTime > 0)
        {
            m_TID = SDL_AddTimer(m_AlarmTime, timer_cb, this);
            if(m_TID == NULL)
            {
                pLog->error("Unable to add timer (%s)\n", SDL_GetError());
                state = false;
            }
        }
        m_IsEnabled = state;
    }
}

Uint32 aedTimer::timer_cb(Uint32 interval, void *p)
{
    aedTimer *
        me = (aedTimer *) p;

    me->m_CurTicks += interval;
    if((me->m_CurTicks - me->m_LastTicks) >= me->m_AlarmTime)
    {
        me->triggerEvent(TIMER_ALARM, me, NULL);
        me->m_LastTicks = me->m_CurTicks;
    }
    return interval;
}

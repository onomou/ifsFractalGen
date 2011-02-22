#include "aedObject.h"

aedObject::aedObject()
{
}

aedObject::~aedObject()
{
}

bool aedObject::connect(aedEvents evt, aedCallBack cbf)
{
    m_Signals.insert(std::make_pair(evt, aedSignal(cbf)));
    return true;
}

bool aedObject::disconnect(aedEvents evt)
{
    m_Signals.erase(evt);
    return true;
}

void
aedObject::triggerEvent(aedEvents evt, void *caller, void *data)
{
    std::multimap < aedEvents, aedSignal >::iterator pos;

    for(pos = m_Signals.lower_bound(evt); pos != m_Signals.upper_bound(evt);
        ++pos)
        pos->second.trigger(caller, data);
}

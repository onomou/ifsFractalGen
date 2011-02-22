#include "aedSignal.h"

aedSignal::aedSignal()
{
}

aedSignal::aedSignal(aedCallBack & cbf)
{

    connect(cbf);

}

aedSignal::~aedSignal()
{

}

void
aedSignal::connect(aedCallBack & cbf)
{
    m_CallbackFunction = cbf;
}

void
aedSignal::trigger(void *data1, void *data2)
{
    m_CallbackFunction.Call(data1, data2);
}

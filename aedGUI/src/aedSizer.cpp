#include "aedSizer.h"
 aedSizer::aedSizer() 
{
    m_Parent = NULL;
} aedSizer::~aedSizer() 
{
} aedSizer::aedSizer(aedWidget * parent) 
{
    m_Parent = parent;
} void 

aedSizer::
addWidget(aedWidget * widget) 
{
    m_Widgets.push_back(widget);
} 

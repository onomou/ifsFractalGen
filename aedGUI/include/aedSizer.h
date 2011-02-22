#ifndef AEDSIZER_H
#define AEDSIZER_H

#include "aedWidget.h"

class DLLEXPORT aedSizer
{
  public:
    aedSizer();
    aedSizer(aedWidget * parent);
    virtual ~ aedSizer();
    void addWidget(aedWidget * widget);
    virtual void calculateSize(void) = 0;

  protected:
      aedWidget * m_Parent;
      std::vector < aedWidget * >m_Widgets;
};

#endif

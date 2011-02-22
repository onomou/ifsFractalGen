#ifndef AEDBOXSIZER_H
#define AEDBOXSIZER_H

#include "aedSizer.h"

enum boxType
{
    aedVERTICAL,
    aedHORIZONTAL
};

class DLLEXPORT aedBoxSizer:public aedSizer
{
  public:
    aedBoxSizer(aedWidget * parent, boxType type);
    virtual void calculateSize(void);
  private:
      boxType m_BoxType;
};

#endif

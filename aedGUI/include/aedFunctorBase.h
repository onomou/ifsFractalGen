#ifndef AEDFUNCTORBASE_H
#define AEDFUNCTORBASE_H

#include <iostream>
#include "dllmacros.h"

class DLLEXPORT aedFunctorBase
{
  public:
    //aedFunctorBase();
    //virtual ~aedFunctorBase();

    virtual void operator() (void *caller, void *data) = 0;
    virtual void Call(void *caller, void *data) = 0;
    virtual aedFunctorBase *clone(void) = 0;

};

class stub
{
};

#endif

#ifndef AEDFUNCTORNOARGS_H
#define AEDFUNCTORNOARGS_H

#include "aedFunctorBase.h"

template < class T = stub > class DLLEXPORT aedFunctorNoArgs:public
    aedFunctorBase
{
  public:
    aedFunctorNoArgs(T * obj, void (T::*member) (void *, void *))
    {
        m_Object = obj;
        m_MemberFunctionPointer = member;
        m_FunctionPointer = NULL;
    }

    aedFunctorNoArgs(void (*function) (void *, void *))
    {
        m_FunctionPointer = function;
        m_Object = NULL;
        m_MemberFunctionPointer = NULL;
    }

    virtual void operator() (void *caller, void *data)
    {
        this->Call(caller, data);
    }

    void Call(void *caller, void *data)
    {
        if(m_Object != NULL)
            (*m_Object.*m_MemberFunctionPointer) (caller, data);
        else
            m_FunctionPointer(caller, data);
    }

    aedFunctorBase *clone(void)
    {
        aedFunctorNoArgs < T > *clone;

        if(m_Object)
            clone =
                new aedFunctorNoArgs < T > (m_Object, m_MemberFunctionPointer);
        else
            clone = new aedFunctorNoArgs < T > (m_FunctionPointer);

        return clone;
    }

    virtual ~ aedFunctorNoArgs()
    {

    }

  private:
    T * m_Object;
    void (T::*m_MemberFunctionPointer) (void *, void *);
    void (*m_FunctionPointer) (void *, void *);

};

#endif

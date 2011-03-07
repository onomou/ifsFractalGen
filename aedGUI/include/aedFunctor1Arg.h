#ifndef AEDFUNCTOR1ARG_H
#define AEDFUNCTOR1ARG_H

#include "aedFunctorBase.h"

template < class T = stub, class A = stub > class DLLEXPORT aedFunctor1Arg:public
    aedFunctorBase
{
  public:
    aedFunctor1Arg(T * obj, void (T::*member) (void *, void *, A *), A * arg)
    {
        m_Object = obj;
        m_MemberFunctionPointer = member;
        m_Argument = arg;
        m_FunctionPointer = NULL;
    }

    aedFunctor1Arg(void (*function) (void *, void *, A *), A * arg)
    {
        m_FunctionPointer = function;
        m_MemberFunctionPointer = NULL;
        m_Object = NULL;
        m_Argument = arg;
    }

    virtual void operator() (void *caller, void *data)
    {
        this->Call(caller, data);
    }

    void Call(void *caller, void *data)
    {
        if(m_Object)
            (*m_Object.*m_MemberFunctionPointer) (caller, data, m_Argument);
        else
            m_FunctionPointer(caller, data, m_Argument);
    }

    aedFunctorBase *clone(void)
    {
        aedFunctor1Arg < T, A > *clone;

        if(m_Object)
            clone =
                new aedFunctor1Arg < T, A > (m_Object, m_MemberFunctionPointer,
                                             m_Argument);
        else
            clone = new aedFunctor1Arg < T, A > (m_FunctionPointer, m_Argument);

        return clone;
    }

    virtual ~ aedFunctor1Arg()
    {

    }

  private:
    T * m_Object;
    A *m_Argument;
    void (T::*m_MemberFunctionPointer) (void *, void *, A *);
    void (*m_FunctionPointer) (void *, void *, A *);

};

#endif

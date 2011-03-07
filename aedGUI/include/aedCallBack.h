
/*
 * The aedCallBack class
 * This class delivers signals from the GUI library.
 * Initial design by Eduardo B. Fonseca <ebf@aedsol.com>
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

#ifndef AEDCALLBACK_H
#define AEDCALLBACK_H

#include "dllmacros.h"
#include "aedFunctorNoArgs.h"
#include "aedFunctor1Arg.h"

class DLLEXPORT aedCallBack
{
  public:
    aedCallBack()
    {
        m_FuncPtr = NULL;
    }

    aedCallBack(void (*function) (void *, void *)):m_FuncPtr(new
                                                             aedFunctorNoArgs <
                                                             stub > (function))
    {
    }

    template < class A >
        aedCallBack(void (*function) (void *, void *, A *),
                    A * arg):m_FuncPtr(new aedFunctor1Arg < stub, A > (function,
                                                                       arg))
    {
    }

    template < class T >
        aedCallBack(T * obj,
                    void (T::*memberfunction) (void *,
                                               void *)):m_FuncPtr(new
                                                                  aedFunctorNoArgs
                                                                  < T > (obj,
                                                                         memberfunction))
    {
    }

    template < class T, class A >
        aedCallBack(T * obj, void (T::*memberfunction) (void *, void *, A *),
                    A * arg):m_FuncPtr(new aedFunctor1Arg < T, A > (obj,
                                                                    memberfunction,
                                                                    arg))
    {
    }

    aedCallBack(const aedCallBack & obj)
    {
        m_FuncPtr = obj.getFunctor()->clone();
    }

    aedCallBack operator=(const aedCallBack & obj)
    {
        m_FuncPtr = obj.getFunctor()->clone();

        return *this;
    }

    aedFunctorBase *getFunctor(void) const
    {
        return m_FuncPtr;
    }

    void Call(void *caller, void *data)
    {
        if(m_FuncPtr != NULL)
        {
            m_FuncPtr->Call(caller, data);
        }
    }

    ~aedCallBack()
    {
        if(m_FuncPtr)
            delete m_FuncPtr;
    }

  private:
    aedFunctorBase * m_FuncPtr;
};

#endif

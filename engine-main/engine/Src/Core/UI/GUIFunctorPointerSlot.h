/********************************************************************
filename: 	GUIFunctorPointerSlot.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef _CEGUIFunctorPointerSlot_h_
#define _CEGUIFunctorPointerSlot_h_

#include "GUISlotFunctorBase.h"

// Start of CEGUI namespace section
namespace LORD
{
/*!
\brief
    Slot template class that creates a functor that calls back via a functor
    object pointer.
*/
template<typename T>
class FunctorPointerSlot : public SlotFunctorBase
{
public:
    FunctorPointerSlot(T* functor) :
        d_functor(functor)
    {}

    virtual bool operator()(const EventArgs& args)
    {
        return (*d_functor)(args);
    }

private:
    T* d_functor;
};

} // End of  CEGUI namespace section

#endif  // end of guard _CEGUIFunctorPointerSlot_h_

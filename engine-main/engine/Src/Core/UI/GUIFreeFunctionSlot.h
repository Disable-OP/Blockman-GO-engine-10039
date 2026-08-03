/********************************************************************
filename: 	GUIFreeFunctionSlot.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef _CEGUIFreeFunctionSlot_h_
#define _CEGUIFreeFunctionSlot_h_

#include "GUISlotFunctorBase.h"

// Start of CEGUI namespace section
namespace LORD
{
/*!
\brief
    Slot functor class that calls back via a free function pointer.
*/
class FreeFunctionSlot : public SlotFunctorBase
{
public:
    //! Slot function type.
    typedef bool (SlotFunction)(const EventArgs&);

    FreeFunctionSlot(SlotFunction* func) :
        d_function(func)
    {}

    virtual bool operator()(const EventArgs& args)
    {
        return d_function(args);
    }

private:
    SlotFunction* d_function;
};


} // End of  CEGUI namespace section

#endif  // end of guard _CEGUIFreeFunctionSlot_h_

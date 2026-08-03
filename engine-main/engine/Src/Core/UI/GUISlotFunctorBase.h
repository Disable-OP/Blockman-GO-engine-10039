/********************************************************************
filename: 	GUISlotFunctorBase.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef _CEGUISlotFunctorBase_h_
#define _CEGUISlotFunctorBase_h_

// Start of CEGUI namespace section
namespace LORD
{

// forward refs
class EventArgs;

/*!
\brief
    Defines abstract interface which will be used when constructing various
    functor objects that bind slots to signals (or in CEGUI terms, handlers to
    events).
*/
class SlotFunctorBase
{
public:
    virtual ~SlotFunctorBase() {};
    virtual bool operator()(const EventArgs& args) = 0;
};


} // End of  CEGUI namespace section

#endif  // end of guard _CEGUISlotFunctorBase_h_

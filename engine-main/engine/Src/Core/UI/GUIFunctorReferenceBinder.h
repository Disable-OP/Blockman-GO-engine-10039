/********************************************************************
filename: 	GUIFunctorReferenceBinder.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef _CEGUIFunctorReferenceBinder_h_
#define _CEGUIFunctorReferenceBinder_h_

// Start of CEGUI namespace section
namespace LORD
{
/*!
\brief
    Class that enables the creation of a reference binding for a functor object
    to be used as a callback slot.  Wrap your functor with this to enable the
    use of an object reference when subscribing to an event signal (as opposed
    to the functor object being copied, or using a pointer).
*/
template<typename T>
struct FunctorReferenceBinder
{
    FunctorReferenceBinder(T& functor) :
        d_functor(functor)
    {}

    T& d_functor;
};

} // End of  CEGUI namespace section

#endif  // end of guard _CEGUIFunctorReferenceBinder_h_

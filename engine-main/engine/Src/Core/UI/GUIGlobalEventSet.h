/********************************************************************
filename: 	GUIGlobalEventSet.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef _CEGUIGlobalEventSet_h_
#define _CEGUIGlobalEventSet_h_

#include "GUIEventSet.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif

// Start of CEGUI namespace section
namespace LORD
{
/*!
\brief
    The GlobalEventSet singleton allows you to subscribe to an event for all
    instances of a class.  The GlobalEventSet effectively supports "late binding"
    to events; which means you can subscribe to some event that does not actually
    exist (yet).
*/
class LORD_CORE_API GlobalEventSet : public EventSet, public Singleton<GlobalEventSet>
{
public:
	GlobalEventSet();
	~GlobalEventSet();


	/*!
	\brief
		Return singleton System object

	\return
		Singleton System object
	*/
	static	GlobalEventSet*	Instance(void);


	/*!
	\brief
		Fires the named event passing the given EventArgs object.

	\param name
		GUIString object holding the name of the Event that is to be fired (triggered)

	\param args
		The EventArgs (or derived) object that is to be bassed to each subscriber of the Event.  Once all subscribers
		have been called the 'handled' field of the event is updated appropriately.
		
	\param eventNamespace
		GUIString object describing the namespace prefix to use when firing the global event.

	\return
		Nothing.
	*/
	virtual void	fireEvent(const GUIString& name, EventArgs& args, const GUIString& eventNamespace = "");
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIGlobalEventSet_h_

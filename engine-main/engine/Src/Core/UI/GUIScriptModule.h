
/********************************************************************
filename: 	GUIScriptModule.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef _CEGUIScriptModule_h_
#define _CEGUIScriptModule_h_

#include "Core.h"
#include "GUIString.h"
#include "GUIEvent.h"


// Start of CEGUI namespace section
namespace LORD
{
class EventSet;
/*!
\brief
	Abstract interface required for all scripting support modules to be used with
	the CEGUI system.
*/
class LORD_CORE_API ScriptModule : public ObjectAlloc
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for ScriptModule base class
	*/
	ScriptModule(void);


	/*!
	\brief
		Destructor for ScriptModule base class.
	*/
	virtual ~ScriptModule(void) {}


	/*************************************************************************
		Abstract interface
	*************************************************************************/
	/*!
	\brief
		Execute a script file.

	\param filename
		GUIString object holding the filename of the script file that is to be executed
		
	\param resourceGroup
		Resource group idendifier to be passed to the ResourceProvider when loading the script file.
	*/
	virtual	void	executeScriptFile(const GUIString& filename, const GUIString& resourceGroup = "")	= 0;


	/*!
	\brief
		Execute a scripted global function.  The function should not take any parameters and should return an integer.

	\param function_name
		GUIString object holding the name of the function, in the global script environment, that
		is to be executed.

	\return
		The integer value returned from the script function.
	*/
	virtual int	executeScriptGlobal(const GUIString& function_name)	= 0;


	/*!
	\brief
		Execute a scripted global 'event handler' function.  The function should take some kind of EventArgs like parameter
		that the concrete implementation of this function can create from the passed EventArgs based object.  The function
		should not return anything.

	\param handler_name
		GUIString object holding the name of the scripted handler function.

	\param e
		EventArgs based object that should be passed, by any appropriate means, to the scripted function.

	\return
		- true if the event was handled.
		- false if the event was not handled.
	*/
	virtual	bool	executeScriptedEventHandler(const GUIString& handler_name, const EventArgs& e)		= 0;


    /*!
    \brief
        Execute script code contained in the given CEGUI::GUIString object.

    \param str
        GUIString object holding the valid script code that should be executed.

    \return
        Nothing.
    */
    virtual void executeString(const GUIString& str) = 0;


    /*!
    \brief
        Method called during system initialisation, prior to running any scripts via the ScriptModule, to enable the ScriptModule
        to perform any operations required to complete initialisation or binding of the script language to the gui system objects.

    \return
        Nothing.
    */
    virtual void createBindings(void) {}


    /*!
    \brief
        Method called during system destruction, after all scripts have been run via the ScriptModule, to enable the ScriptModule
        to perform any operations required to cleanup bindings of the script language to the gui system objects, as set-up in the
        earlier createBindings call.

    \return
        Nothing.
    */
    virtual void destroyBindings(void) {}

    /*!
    \brief
        Return identification GUIString for the ScriptModule.  If the internal id GUIString has not been
        set by the ScriptModule creator, a generic GUIString of "Unknown scripting module" will be returned.

    \return
        GUIString object holding a GUIString that identifies the ScriptModule in use.
    */
    const GUIString& getIdentifierString() const;

    /*!
    \brief
            Subscribes the named Event to a scripted funtion

    \param target
            The target EventSet for the subscription.

    \param name
            GUIString object containing the name of the Event to subscribe to.

    \param subscriber_name
            GUIString object containing the name of the script funtion that is to be subscribed to the Event.

    \return
            Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.

    \exception UnknownObjectException	Thrown if an Event named \a name is not in the EventSet
    */
    virtual GUIEvent::Connection	subscribeEvent(EventSet* target, const GUIString& name, const GUIString& subscriber_name) = 0;

    /*!
    \brief
            Subscribes the specified group of the named Event to a scripted funtion.

    \param target
            The target EventSet for the subscription.

    \param name
            GUIString object containing the name of the Event to subscribe to.

    \param group
            Group which is to be subscribed to.  Subscription groups are called in ascending order.

    \param subscriber_name
            GUIString object containing the name of the script funtion that is to be subscribed to the Event.

    \return
            Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.

    \exception UnknownObjectException	Thrown if an Event named \a name is not in the EventSet
    */
    virtual GUIEvent::Connection	subscribeEvent(EventSet* target, const GUIString& name, GUIEvent::Group group, const GUIString& subscriber_name) = 0;

    /*!
    \brief
        Sets the default resource group to be used when loading script files.

    \param resourceGroup
        GUIString describing the default resource group identifier to be used.

    \return
        Nothing.
    */
    static void setDefaultResourceGroup(const GUIString& resourceGroup)
        { d_defaultResourceGroup = resourceGroup; }

    /*!
    \brief
        Returns the default resource group used when loading script files.

    \return
        GUIString describing the default resource group identifier..
    */
    static const GUIString& getDefaultResourceGroup()
        { return d_defaultResourceGroup; }

protected:
    //! GUIString that holds some id information about the module.
    GUIString d_identifierString;
    //! holds the default resource group ID for loading script files.
    static GUIString d_defaultResourceGroup;
};


/*!
\brief
	Functor class used for binding named script functions to events
*/
class LORD_CORE_API ScriptFunctor
{
public:
	ScriptFunctor(const GUIString& functionName) : scriptFunctionName(functionName) {}
    ScriptFunctor(const ScriptFunctor& obj) : scriptFunctionName(obj.scriptFunctionName) {}
	bool	operator()(const EventArgs& e) const;

private:
    // no assignment possible
    ScriptFunctor& operator=(const ScriptFunctor& rhs);

	const GUIString	scriptFunctionName;
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIScriptModule_h_

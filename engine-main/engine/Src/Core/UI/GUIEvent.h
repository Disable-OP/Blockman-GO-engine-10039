/********************************************************************
filename: 	GUIEvent.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_EVENT_H__
#define __GUI_EVENT_H__

#include "GUIString.h"
#include "GUIBoundSlot.h"
#include "GUISubscriberSlot.h"
#include "GUIRefCounted.h"

#include <map>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

namespace LORD
{
/*!
\brief
    Defines an 'event' which can be subscribed to by interested parties.

    An Event can be subscribed by a function, a member function, or a function
    object.  Whichever option is taken, the function signature needs to be as
    follows:
    \par
    <em>bool function_name(const EventArgs& args);</em>
    \note
        An Event object may not be copied.
*/
class LORD_CORE_API GUIEvent
{
public:
    /*!
    \brief
        Connection object.  This is a thin 'smart pointer' wrapper around the
        actual BoundSlot that represents the connection.  You can use this
        object to inspect the current connection state and also to disconnect
        from the event.
    */
    typedef RefCounted<BoundSlot> Connection;

    /*!
    \brief
        Subscriber object type.  This is now just a typedef to SubscriberSlot,
        the use of the name Event::Subscriber is maintained for hostorical and
        compatability reasons.
    */
    typedef LORD::SubscriberSlot Subscriber;

    /*!
    \brief
        Type for a subscriber group.  You can use the subscriber group to
        order calls to multiple subscribers.  Groups are called in ascending
        order, with subscribers with no group called last.
    */
    typedef unsigned int Group;

    /*!
    \brief
        GUIEvent::Connection wrapper that automatically disconnects the connection
        when the object is deleted (or goes out of scope).
    */
    class ScopedConnection : public Connection
    {
    public:
        ScopedConnection() {}

        ~ScopedConnection()
            { disconnect(); }

        ScopedConnection(const GUIEvent::Connection& connection) :
            d_connection(connection)
        {}

        ScopedConnection& operator=(const GUIEvent::Connection& connection)
        {
            d_connection = connection;
            return *this;
        }

        bool connected() const
            { return d_connection.isValid() ? d_connection->connected() : false; }

        void disconnect()
            { if (d_connection.isValid()) d_connection->disconnect(); }

    private:
        GUIEvent::Connection d_connection;
    };

    /*!
    \brief
        Constructs a new Event object with the specified name
    */
    GUIEvent(const GUIString& name);

    /*!
    \brief
        Destructor for Event objects.  Note that this is non-virtual and so you
        should not sub-class Event.
    */
    ~GUIEvent();

    /*!
    \brief
        Return the name given to this Event object when it was created.

    \return
        GUIString object containing the name of the Event object.
    */
    const GUIString& getName(void) const
        { return d_name; }

    /*!
    \brief
        Subscribes some function or object to the Event

    \param subscriber
        A function, static member function, or function object, with the
        signature void function_name(const EventArgs& args).  To subscribe a
        member function you should explicitly create an Event::Subscriber as
        this parameter.

    \return
        A Connection object which can be used to disconnect (unsubscribe) from
        the Event, and also to check the connection state.
    */
    Connection subscribe(const Subscriber& slot);

    /*!
    \brief
        Subscribes some function or object to the Event

    \param group
        The Event group to subscribe to, subscription groups are called in
        ascending order, followed by subscriptions with no group.
        Note that calling order of connections to the same group is unspecified.

    \param subscriber
        A function, static member function, or function object, with the
        signature void function_name(const EventArgs& args).  To subscribe a
        member function you should explicitly create an Event::Subscriber as
        this parameter.

    \return
        A Connection object which can be used to disconnect (unsubscribe) from
        the Event, and also to check the connection state.
    */
    Connection subscribe(Group group, const Subscriber& slot);

    /*!
    \brief
        Fires the event.  All event subscribers get called in the appropriate
        sequence.

    \param args
        An object derived from EventArgs to be passed to each event subscriber.
        The 'handled' field will be set to true if any of the called subscribers
        return that they handled the event.

    \return
        Nothing.
    */
    void operator()(EventArgs& args);


private:
    friend void LORD::BoundSlot::disconnect();
    /*!
    \brief
        Disconnects and removes the given BoundSlot from the collection of bound
        slots attached to this Event, thus 'unsubscribing' it.

    \note
        This is an implementation member, and is not available to client code.
        In order to detach / unsubscribe from an Event you should be using the
        Connection object(s) returned when you initially subscribed.
    */
    void unsubscribe(const BoundSlot& slot);

    // Copy constructor and assignment are not allowed for events
    GUIEvent(const GUIEvent&) {}
    GUIEvent& operator=(const GUIEvent&)  {return *this;}

    typedef std::multimap<Group, Connection> SlotContainer;
    SlotContainer d_slots;  //!< Collection holding ref-counted bound slots
    const GUIString d_name;    //!< Name of this event
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _CEGUIEvent_h_

/***********************************************************************
	filename: 	CEGUIEventSet.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the EventSet class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "GUIEventSet.h"
#include "GUIGlobalEventSet.h"
#include "GUIScriptModule.h"
#include "GUISystem.h"

// Start of CEGUI namespace section
namespace LORD
{
/*************************************************************************
	Constructor
*************************************************************************/
EventSet::EventSet() :
	d_muted(false)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
EventSet::~EventSet(void)
{
	removeAllEvents();
}


/*************************************************************************
	Add a new event to the EventSet
*************************************************************************/
void EventSet::addEvent(const GUIString& name)
{
	if (isEventPresent(name))
	{
		LordException("An event named '%s' already exists in the EventSet.", name.c_str());
	}

	d_events[name] = new GUIEvent(name);
}


/*************************************************************************
	Remove an event from the EventSet
*************************************************************************/
void EventSet::removeEvent(const GUIString& name)
{
	EventMap::iterator pos = d_events.find(name);

	if (pos != d_events.end())
	{
		delete pos->second;
		d_events.erase(pos);
	}

}


/*************************************************************************
	Remove all events from the EventSet
*************************************************************************/
void EventSet::removeAllEvents(void)
{
	EventMap::iterator pos = d_events.begin();
	EventMap::iterator end = d_events.end()	;

	for (; pos != end; ++pos)
	{
		delete pos->second;
	}

	d_events.clear();
}


/*************************************************************************
	Check to see if an event is available
*************************************************************************/
bool EventSet::isEventPresent(const GUIString& name)
{
	return (d_events.find(name) != d_events.end());
}


/*************************************************************************
	Subscribe to a scripted event (no group)
*************************************************************************/
GUIEvent::Connection EventSet::subscribeScriptedEvent(const GUIString& name, const GUIString& subscriber_name)
{
	//return subscribeEvent(name, Event::Subscriber(ScriptFunctor(subscriber_name)));
	ScriptModule* sm = GUISystem::Instance()->GetScriptModule();
	if (!sm)
	{
	  LordException("[EventSet::subscribeScriptedEvent] No scripting module is available");
	}
	return sm->subscribeEvent(this, name, subscriber_name);
}


/*************************************************************************
	Subscribe to a scripted event
*************************************************************************/
GUIEvent::Connection EventSet::subscribeScriptedEvent(const GUIString& name, GUIEvent::Group group, const GUIString& subscriber_name)
{
    //return subscribeEvent(name, group, Event::Subscriber(ScriptFunctor(subscriber_name)));
    ScriptModule* sm = GUISystem::Instance()->GetScriptModule();
	if (!sm)
	{
	   LordException("[EventSet::subscribeScriptedEvent] No scripting module is available");
	}
	return sm->subscribeEvent(this, name, group, subscriber_name);
}


/*************************************************************************
	Subscribe to an event (no group)
*************************************************************************/
GUIEvent::Connection EventSet::subscribeEvent(const GUIString& name, GUIEvent::Subscriber subscriber)
{
    // do subscription & return connection
    return getEventObject(name, true)->subscribe(subscriber);
}


/*************************************************************************
	Subscribe to an event group
*************************************************************************/
GUIEvent::Connection EventSet::subscribeEvent(const GUIString& name, GUIEvent::Group group, GUIEvent::Subscriber subscriber)
{
    // do subscription with group & return connection
    return getEventObject(name, true)->subscribe(group, subscriber);
}

/*************************************************************************
	Fire / Trigger an event
*************************************************************************/
void EventSet::fireEvent(const GUIString& name, EventArgs& args, const GUIString& eventNamespace)
{
    // handle global events
	// 暂时注释掉，目前不需要全局的事件，有需要的话，需要初始化globalevent
    //GlobalEventSet::Instance()->fireEvent(name, args, eventNamespace);
    // handle local event
    fireEvent_impl(name, args);
}


/*************************************************************************
	Return whether the EventSet is muted or not.	
*************************************************************************/
bool EventSet::isMuted(void) const
{
	return d_muted;
}


/*************************************************************************
	Set the mute state for this EventSet.	
*************************************************************************/
void EventSet::setMutedState(bool setting)
{
	d_muted = setting;
}


/*************************************************************************
    Return the named Event object, optionally adding it to the set
    if needed
*************************************************************************/
GUIEvent* EventSet::getEventObject(const GUIString& name, bool autoAdd)
{
    EventMap::iterator pos = d_events.find(name);

    // if event did not exist, add it and then find it.
    if (pos == d_events.end())
    {
        if (autoAdd)
        {
            addEvent(name);
            return d_events.find(name)->second;
        }
        else
        {
            return 0;
        }
    }

    return pos->second;
}


/*************************************************************************

*************************************************************************/
void EventSet::fireEvent_impl(const GUIString& name, EventArgs& args)
{
    // find event object
    GUIEvent* ev = getEventObject(name);

    // fire the event if present and set is not muted
    if ((ev != 0) && !d_muted)
        (*ev)(args);
}


/*************************************************************************
	Return a EventSet::EventIterator object to iterate over the available
	events.
*************************************************************************/
EventSet::Iterator EventSet::getIterator(void) const
{
	return Iterator(d_events.begin(), d_events.end());
}


} // End of  CEGUI namespace section

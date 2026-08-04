/*
from Ogre
*/
#include "Core.h"
#include "Resource.h"
#include "Object/Exception.h"

namespace LORD
{

	//-----------------------------------------------------------------------
	Resource::Resource(const String& name, ResourceType rt)
		: mName(name)
		, mLoadingState(LOADSTATE_CREATED)
		, mSize(0)
		, mResourceType(rt)
		, mRefrenceCount(0)
		, mDelayedTime(0)
		, mOldLoadingState(LOADSTATE_CREATED)
	{
	}
	//-----------------------------------------------------------------------
	Resource::~Resource() 
	{ 
	}
	//-----------------------------------------------------------------------
	bool Resource::prepare()
	{
		if (mLoadingState == LOADSTATE_PREPARE)
			return true;

		EE_LOCK_AUTO_MUTEX

			if (mLoadingState == LOADSTATE_PREPARE)
			{
				return true;
			}
			else if (mLoadingState == LOADSTATE_CREATED)
			{
				if (mResourceType == RESOURCE_TYPE_MANUAL)
				{
					LordLogWarning("Instance [%s] was defined as manually loaded, but no manual loader was provided. this Resource will be lost if it has to be reloaded.", mName.c_str());
				}
				else
				{
					if (!prepareImpl())
					{
						unprepareImpl();
						mLoadingState = LOADSTATE_CREATED;
						LordLogError("Resource instance [%s] prepared failed.", mName.c_str());
						return false;
					}
				}
				mLoadingState = LOADSTATE_PREPARE;
			}

		return true;
	}
	//---------------------------------------------------------------------
	bool Resource::load()
	{
		if(mLoadingState == LOADSTATE_LOADED)
			return true;

		// Scope lock for actual loading
		EE_LOCK_AUTO_MUTEX

		if(mLoadingState == LOADSTATE_LOADED)
			return true;

		if(mResourceType == RESOURCE_TYPE_MANUAL)
		{
			LordLogWarning("Instance [%s] was defined as manually loaded, but no manual loader was provided. this Resource will be lost if it has to be reloaded.", mName.c_str());
		}
		else
		{
			if (mLoadingState == LOADSTATE_CREATED)
			{
				if (!prepareImpl())
				{
					unprepareImpl();
					return false;
				}
			}

			mLoadingState = LOADSTATE_PREPARE;

			bool bRet = loadImpl();

			unprepareImpl();

			if(!bRet)
				return false;
		}

		// Calculate resource size
		mSize = calculateSize();

		mLoadingState = LOADSTATE_LOADED;

		return true;
	}
	//-----------------------------------------------------------------------
	void Resource::unload(void) 
	{ 
		// Early-out without lock (mitigate perf cost of ensuring unloaded)
		if (mLoadingState == LOADSTATE_CREATED) return;

		// Scope lock for actual unload
		{
			EE_LOCK_AUTO_MUTEX
			if (mLoadingState==LOADSTATE_PREPARE) 
			{
				unprepareImpl();
			} 
			else
			{
				unloadImpl();
			}
		}

		mLoadingState = LOADSTATE_CREATED;
	}
	//-----------------------------------------------------------------------
	void Resource::reload(void) 
	{ 
		EE_LOCK_AUTO_MUTEX

		if(mLoadingState == LOADSTATE_PREPARE) 
		{
			unprepareImpl();
		} 
		else if(mLoadingState == LOADSTATE_LOADED)
		{
			unloadImpl();
		}
		mLoadingState = LOADSTATE_CREATED;

		load();
	}
	//-----------------------------------------------------------------------
	void Resource::delayedRelease()
	{
		mOldLoadingState = mLoadingState;
		mLoadingState = LOADSTATE_DESTORY;
		mDelayedTime = 0;
	}
	//-----------------------------------------------------------------------
	void Resource::reserveDelayed()
	{
		mLoadingState = mOldLoadingState;
		mDelayedTime = 0;
	}
	//-----------------------------------------------------------------------
	ui32 Resource::addDelayedTime( ui32 deltaTime )
	{
		mDelayedTime += deltaTime;
		return mDelayedTime;
	}
	//-----------------------------------------------------------------------
	void Resource::addRefrenceCount()
	{
		++mRefrenceCount;
	}
	//-----------------------------------------------------------------------
	bool Resource::subRefrenceCount()
	{
		--mRefrenceCount;
		return mRefrenceCount==0;
	}
}
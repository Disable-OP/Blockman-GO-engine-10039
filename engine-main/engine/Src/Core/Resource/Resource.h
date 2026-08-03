/********************************************************************
filename: 	Resource.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RESOURCE_H__
#define __LORD_RESOURCE_H__

#include "Core.h"

namespace LORD
{
	enum ResourceType
	{
		RESOURCE_TYPE_NORMAL,
		RESOURCE_TYPE_MANUAL,
		RESOURCE_TYPE_OUTSIDE
	};

	class LORD_CORE_API Resource : public ObjectAlloc
	{
	public:
		EE_AUTO_MUTEX		
			enum LoadingState
		{
			/// Not loaded
			LOADSTATE_CREATED,
			/// Loading is in progress
			LOADSTATE_PREPARE,
			/// Fully loaded
			LOADSTATE_LOADED,
			/// Destroy
			LOADSTATE_DESTORY
		};

	protected:
		/// Unique name of the resource
		String mName;
		/// Is the resource currently loaded?
		LoadingState mLoadingState;
		/// The size of the resource in bytes
		size_t mSize;
		/// Is this file manually loaded?
		ResourceType mResourceType;

		int mRefrenceCount;

		ui32			mDelayedTime;
		LoadingState	mOldLoadingState;

		/** Protected unnamed constructor to prevent default construction. 
		*/
		Resource()
			: mLoadingState(LOADSTATE_CREATED)
			, mSize(0)
			, mResourceType(RESOURCE_TYPE_NORMAL)
			, mRefrenceCount(0)
		{ 
		}

		virtual bool prepareImpl(void) { return true; }
		virtual void unprepareImpl(void) {}
		virtual bool loadImpl(void) = 0;
		virtual void unloadImpl(void) = 0;
		virtual size_t calculateSize(void) const = 0;

	public:
		/** Standard constructor.
		@param creator Pointer to the ResourceManager that is creating this resource
		@param name The unique name of the resource
		@param isManual Is this resource manually loaded? If so, you should really
		populate the loader parameter in order that the load process
		can call the loader back when loading is required. 
		*/
		Resource(const String& name, ResourceType rt);

		/** Virtual destructor. Shouldn't need to be overloaded, as the resource
		deallocation code should reside in unload()
		@see
		Resource::unload()
		*/
		virtual ~Resource();

		virtual bool prepare();
		virtual bool load();
		virtual void reload(void);

		/** Returns true if the Resource is reloadable, false otherwise.
		*/
		virtual bool isReloadable(void) const
		{
			return mResourceType == RESOURCE_TYPE_NORMAL;
		}

		/** Is this resource manually loaded?
		*/
		virtual bool isManuallyLoaded(void) const
		{
			return mResourceType == RESOURCE_TYPE_MANUAL;
		}

		const int getRefrenceCount() const { return mRefrenceCount; }

		void delayedRelease();
		void reserveDelayed();
		ui32 addDelayedTime(ui32 deltaTime);

		/** Unloads the resource; this is not permanent, the resource can be
		reloaded later if required.
		*/
		virtual void unload(void);

		/** Retrieves info about the size of the resource.
		*/
		virtual size_t getSize(void) const
		{ 
			return mSize; 
		}

		/** Gets resource name.
		*/
		virtual const String& getName(void) const 
		{ 
			return mName; 
		}

		/** Returns true if the Resource has been loaded, false otherwise.
		*/
		virtual bool isLoaded(void) const 
		{ 
			// No lock required to read this state since no modify
			return (mLoadingState == LOADSTATE_LOADED); 
		}

		/** Returns the current loading state.
		*/
		virtual LoadingState getLoadingState() const
		{
			return mLoadingState;
		}

		void addRefrenceCount();
		bool subRefrenceCount();
	};
}
#endif

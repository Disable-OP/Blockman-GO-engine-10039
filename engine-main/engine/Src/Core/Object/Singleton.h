/********************************************************************
filename: 	Singleton.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SINGLETON_H__
#define __LORD_SINGLETON_H__

#include "Util/AssertX.h"

namespace LORD
{
	template<class T>
	class Singleton
	{
	public:
		Singleton()
		{
			LordAssert(!ms_pSingleton);
			ms_pSingleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			LordAssert(ms_pSingleton);
			ms_pSingleton = NULL;
		}

	private:
		/** \brief Explicit private copy constructor. This is a forbidden operation.*/
		Singleton(const Singleton<T>&);

		/** \brief Private operator= . This is a forbidden operation. */
		Singleton& operator = (const Singleton<T>&);

	public:
		static T* Instance()
		{
			return ms_pSingleton;
		}

	protected:
		static T* ms_pSingleton;
	};

	template<class T>
	T* Singleton<T>::ms_pSingleton = NULL;
}

#endif

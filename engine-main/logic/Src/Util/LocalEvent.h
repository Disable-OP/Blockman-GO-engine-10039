#ifndef __LOCAL_EVENT_HEADER__
#define __LOCAL_EVENT_HEADER__

#include "Util/Listenable.h"

namespace BLOCKMAN
{
	class LocalEvent
	{
	private:
		using ListenableType = unsigned char;
		Listenable<ListenableType> m_listenable;

	public:
		using Subscription = decltype(m_listenable)::Subscription;

		Subscription subscribe(std::function<void()> callback)
		{
			return m_listenable.subscribe([callback](const ListenableType&, const ListenableType&) { return callback(), true; });
		}

		void emit()
		{
			++m_listenable;
		}
	};
}

#endif // !__LOCAL_EVENT_HEADER__

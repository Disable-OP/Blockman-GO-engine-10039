#include "Promise.h"
#include <algorithm>

namespace BLOCKMAN
{
	namespace PROMISE
	{
		thread_local CallbackQueue callbackQueue;

		void runCallbacks()
		{
			auto iter = std::remove_if(callbackQueue.begin(), callbackQueue.end(), [](const FutureCallback& callback) {
				return callback();
			});
			callbackQueue.resize(iter - callbackQueue.begin());
		}

		void clearCallbacks()
		{
			callbackQueue.clear();
			callbackQueue.resize(0);
		}
	}
}

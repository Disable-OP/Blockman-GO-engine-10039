#ifndef __HTTP_PROMISE_HEADER__
#define __HTTP_PROMISE_HEADER__

#include <functional>
#include <atomic>
#include <mutex>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class HttpPromise
	{
	private:
		struct Data
		{
			std::mutex m_mutex;
			double m_progress = 0.0;
			float m_totalDownload = 0.0;
			String m_failureMessage;
			std::function<void(double, float)> m_onProgressCallback;
			std::function<void()> m_onSuccessCallback;
			std::function<void(const String&)> m_onFailureCallback;
		};

		static list<HttpPromise>::type allPromises;

		std::shared_ptr<Data> m_data = make_shared<Data>();

	public:
		HttpPromise()
		{
			allPromises.push_back(*this);
		}

		static void tickAll();

		void progress(double value, float totalDownload)
		{
			std::lock_guard<std::mutex> lock(m_data->m_mutex);
			m_data->m_progress = value;
			m_data->m_totalDownload = totalDownload;
		}

		void reject(const String& message)
		{
			std::lock_guard<std::mutex> lock(m_data->m_mutex);
			m_data->m_failureMessage = message;
			m_data->m_progress = -1;
		}

		void fulfill()
		{
			std::lock_guard<std::mutex> lock(m_data->m_mutex);
			m_data->m_progress = 2;
		}

		HttpPromise& onProgress(std::function<void(double, float)> callback)
		{
			std::lock_guard<std::mutex> lock(m_data->m_mutex);
			m_data->m_onProgressCallback = callback;
			return *this;
		}

		HttpPromise& onSuccess(std::function<void()> callback)
		{
			std::lock_guard<std::mutex> lock(m_data->m_mutex);
			m_data->m_onSuccessCallback = callback;
			return *this;
		}

		HttpPromise& onFailure(std::function<void(const String&)> callback)
		{
			std::lock_guard<std::mutex> lock(m_data->m_mutex);
			m_data->m_onFailureCallback = callback;
			return *this;
		}
	};
}

#endif // !__HTTP_PROMISE_HEADER__

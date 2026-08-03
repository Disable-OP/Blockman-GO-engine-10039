#include "HttpPromise.h"

namespace BLOCKMAN
{
	list<HttpPromise>::type HttpPromise::allPromises;

	void HttpPromise::tickAll()
	{
		for (auto iter = allPromises.begin(); iter != allPromises.end(); )
		{
			auto data = iter->m_data;
			double progress;
			float totalDownload;
			std::function<void()> action;
			{
				std::lock_guard<std::mutex> lock(data->m_mutex);
				progress = data->m_progress;
				totalDownload = data->m_totalDownload;
				if (progress > 1)
				{
					const auto& onSuccessCallback = data->m_onSuccessCallback;
					if (onSuccessCallback)
					{
						action = [onSuccessCallback] { onSuccessCallback(); };
					}
				}
				else if (progress < 0)
				{
					const auto& onFailureCallback = data->m_onFailureCallback;
					if (onFailureCallback)
					{
						const auto& failureMessage = data->m_failureMessage;
						action = [onFailureCallback, failureMessage] { onFailureCallback(failureMessage); };
					}
				}
				else
				{
					const auto& onProgressCallback = data->m_onProgressCallback;
					if (onProgressCallback)
					{
						action = [onProgressCallback, progress, totalDownload] { onProgressCallback(progress, totalDownload); };
					}
				}
			}
			if (action)
			{
				action();
			}

			if (progress < 0 || progress > 1)
			{
				iter = allPromises.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
}

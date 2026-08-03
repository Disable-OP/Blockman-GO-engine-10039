#ifndef CPR_CONNECT_TIMEOUT_H
#define CPR_CONNECT_TIMEOUT_H

#include <chrono>
#include "cpr/defines.h"

namespace cpr {

	class ConnectTimeout {
	public:
		template<typename Rep, typename Period>
		explicit ConnectTimeout(const std::chrono::duration<Rep, Period>& timeout)
			: timeout(std::chrono::duration_cast<std::chrono::milliseconds>(timeout)) {}
		ConnectTimeout(const ConnectTimeout& rhs) = default;
		ConnectTimeout(ConnectTimeout&& rhs) = default;
		ConnectTimeout& operator=(const ConnectTimeout& rhs) = default;
		ConnectTimeout& operator=(ConnectTimeout&& rhs) = default;

		long Milliseconds() const;

		std::chrono::milliseconds timeout;
	};

} // namespace cpr


#endif // !CPR_CONNECT_TIMEOUT_H


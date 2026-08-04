#include "cpr/connect_timeout.h"

#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace cpr {

	long ConnectTimeout::Milliseconds() const {
		static_assert(std::is_same<std::chrono::milliseconds, decltype(timeout)>::value,
			"Following casting expects milliseconds.");

		if (timeout.count() > std::numeric_limits<long>::max()) {
			throw std::overflow_error("cpr::Timeout: timeout value overflow: " +
				std::to_string(timeout.count()) + " ms.");
		}
		if (timeout.count() < std::numeric_limits<long>::min()) {
			throw std::underflow_error("cpr::Timeout: timeout value underflow: " +
				std::to_string(timeout.count()) + " ms.");
		}

		return static_cast<long>(timeout.count());
	}

} // namespace cpr
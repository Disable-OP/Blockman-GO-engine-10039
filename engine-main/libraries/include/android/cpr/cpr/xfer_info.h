#ifndef CPR_XFER_INFO_H
#define CPR_XFER_INFO_H

#include <cstdint>
#include <functional>
#include "cpr/defines.h"

namespace cpr {

    struct TransferProgress
    {
        std::uint64_t downloadTotal;
        std::uint64_t downloadNow;
        std::uint64_t uploadTotal;
        std::uint64_t uploadNow;
    };

    class XferInfo {
    public:
        static int progress_callback(void *clientp, std::uint64_t dltotal, std::uint64_t dlnow, std::uint64_t ultotal, std::uint64_t ulnow);

        XferInfo() = default;
        XferInfo(const XferInfo& rhs) = default;
        XferInfo(XferInfo&& rhs) = default;
        XferInfo& operator=(const XferInfo& rhs) = default;
        XferInfo& operator=(XferInfo&& rhs) = default;
        explicit XferInfo(std::function<int(const TransferProgress&)> callback) : callback(callback) {}

        std::function<int(const TransferProgress&)> callback;
    };

} // namespace cpr


#endif // !CPR_XFERINFO_H


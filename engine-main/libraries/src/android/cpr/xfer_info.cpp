#include "cpr/xfer_info.h"

namespace cpr {

    int XferInfo::progress_callback(void * clientp, std::uint64_t dltotal, std::uint64_t dlnow, std::uint64_t ultotal, std::uint64_t ulnow)
    {
        return static_cast<XferInfo*>(clientp)->callback({ dltotal, dlnow, ultotal, ulnow });
    }

} // namespace cpr

// curl_ssl_stubs.c — stub implementations for curl's TLS backend functions
// that are referenced by curl's core (url.c, http.c, etc.) but not
// available because we excluded the TLS backends (openssl, gtls, nss).
//
// In standalone server mode the HTTP calls all go to 127.0.0.1:1
// (connection refused), so TLS is never actually used. These stubs
// satisfy the linker; if TLS is ever needed, a real TLS backend must
// be added to the curl build.
//
// This file is compiled as part of the curl static library (via
// Android.mk LOCAL_SRC_FILES).

#include "curl_setup.h"
#include <curl/curl.h>

// Forward-declare the structs these functions take (defined in urldata.h).
struct ssl_primary_config;
struct ssl_config_data;

// Stubs for ssl_config functions (normally in vtls/vtls.c).
void Curl_free_primary_ssl_config(struct ssl_primary_config* sslc)
{
    (void)sslc;
}

CURLcode Curl_clone_primary_ssl_config(struct ssl_primary_config* source,
                                        struct ssl_primary_config* dest)
{
    (void)source;
    (void)dest;
    return CURLE_OK;
}

bool Curl_ssl_config_matches(struct ssl_primary_config* data,
                              struct ssl_primary_config* probe)
{
    (void)data;
    (void)probe;
    return false;
}

// Stub for Curl_auth_is_digest_supported (normally in vtls/vtls.c or http_digest.c).
bool Curl_auth_is_digest_supported(const struct ssl_primary_config* sslc)
{
    (void)sslc;
    return false;
}

// Stub for Curl_initinfo (normally in getinfo.c — but if it's still
// missing at link time, provide it here too).
// Note: getinfo.c is now included in the build, so this should not be
// needed. Left as a safety net.
#if 0
void Curl_initinfo(struct Curl_easy* data)
{
    (void)data;
}
#endif

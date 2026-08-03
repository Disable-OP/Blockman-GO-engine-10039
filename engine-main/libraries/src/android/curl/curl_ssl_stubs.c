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
struct Curl_easy;

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

// Stub for Curl_ssl_backend (normally in vtls/vtls.c).
const struct Curl_ssl_backend* Curl_ssl_backend = (const void*)0;

// Stubs for digest auth functions (normally in vtls/vtls.c).
CURLcode Curl_auth_decode_digest_http_message(const char* data,
                                                struct Curl_easy* easy)
{
    (void)data;
    (void)easy;
    return CURLE_OK;
}

CURLcode Curl_auth_create_digest_http_message(struct Curl_easy* data,
                                                 const char* userp,
                                                 const char* passwdp,
                                                 const unsigned char* request,
                                                 unsigned char* outptr,
                                                 size_t* outlen,
                                                 void* context)
{
    (void)data; (void)userp; (void)passwdp; (void)request;
    (void)outptr; (void)outlen; (void)context;
    return CURLE_OK;
}

void Curl_auth_digest_cleanup(void* context)
{
    (void)context;
}

// Stubs for getifaddrs/freeifaddrs (Android doesn't have these).
int getifaddrs(void** ifap)
{
    (void)ifap;
    return -1;
}

void freeifaddrs(void* ifa)
{
    (void)ifa;
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

// gnustl_compat.h — compatibility shims for gnustl_static (NDK r17c).
//
// gnustl_static's <string> header does not define std::to_string even
// when __GXX_EXPERIMENTAL_CXX0X__ is defined. This header provides
// fallback implementations using snprintf.
//
// This file is force-included via -include gnustl_compat.h in the
// Android server build (server/CMakeLists.txt).
#pragma once

#include <string>
#include <cstdio>
#include <cstdint>

#if defined(__ANDROID__) && !defined(_LIBCPP_VERSION) && !defined(BLOCKMAN_GNUSTL_COMPAT)
#define BLOCKMAN_GNUSTL_COMPAT

namespace std {

// std::stoi/stol/stoll/stoul/stoull — gnustl_static doesn't define these.
inline int stoi(const string& str, size_t* pos = 0, int base = 10) {
    return static_cast<int>(strtol(str.c_str(), (char**)pos, base));
}

inline long stol(const string& str, size_t* pos = 0, int base = 10) {
    return strtol(str.c_str(), (char**)pos, base);
}

inline long long stoll(const string& str, size_t* pos = 0, int base = 10) {
    return strtoll(str.c_str(), (char**)pos, base);
}

inline unsigned long stoul(const string& str, size_t* pos = 0, int base = 10) {
    return strtoul(str.c_str(), (char**)pos, base);
}

inline unsigned long long stoull(const string& str, size_t* pos = 0, int base = 10) {
    return strtoull(str.c_str(), (char**)pos, base);
}

inline float stof(const string& str, size_t* pos = 0) {
    return strtof(str.c_str(), (char**)pos);
}

inline double stod(const string& str, size_t* pos = 0) {
    return strtod(str.c_str(), (char**)pos);
}

// std::to_string — gnustl_static doesn't define these either.
inline string to_string(int value) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", value);
    return string(buf);
}

inline string to_string(long value) {
    char buf[24];
    snprintf(buf, sizeof(buf), "%ld", value);
    return string(buf);
}

inline string to_string(long long value) {
    char buf[24];
    snprintf(buf, sizeof(buf), "%lld", value);
    return string(buf);
}

inline string to_string(unsigned value) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%u", value);
    return string(buf);
}

inline string to_string(unsigned long value) {
    char buf[24];
    snprintf(buf, sizeof(buf), "%lu", value);
    return string(buf);
}

inline string to_string(unsigned long long value) {
    char buf[24];
    snprintf(buf, sizeof(buf), "%llu", value);
    return string(buf);
}

inline string to_string(float value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%f", value);
    return string(buf);
}

inline string to_string(double value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%f", value);
    return string(buf);
}

inline string to_string(long double value) {
    char buf[48];
    snprintf(buf, sizeof(buf), "%Lf", value);
    return string(buf);
}

} // namespace std

#endif // __ANDROID__ && !_LIBCPP_VERSION

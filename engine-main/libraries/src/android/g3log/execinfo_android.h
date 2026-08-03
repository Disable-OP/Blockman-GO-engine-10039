#pragma once

#ifndef __ANDROID__
#error "execinfo_android.h used but is not under android system"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    int backtrace(void **buffer, int size);

    char **backtrace_symbols(void *const *buffer, int size);

#ifdef __cplusplus
}
#endif

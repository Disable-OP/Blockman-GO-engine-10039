#include "execinfo_android.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unwind.h>
#include <dlfcn.h>

#ifndef __ANDROID__
#error "execinfo_android.c used but is not under android system"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct backtrace_buffer_t
    {
        void **current;
        void **end;
    } backtrace_buffer;

    static _Unwind_Reason_Code unwind_callback(struct _Unwind_Context* context, void* arg)
    {
        backtrace_buffer* bbuf = (backtrace_buffer *)arg;
        if (bbuf->current == bbuf->end) 
        {
            return _URC_END_OF_STACK;
        } 
        uintptr_t pc = _Unwind_GetIP(context);
        if (pc) 
        {
            *bbuf->current++ = (void*) pc;
        }
        return _URC_NO_REASON;
    }

    int backtrace(void **buffer, int size)
    {
        backtrace_buffer bbuf;
        bbuf.current = buffer;
        bbuf.end = buffer + size;
        _Unwind_Backtrace(unwind_callback, &bbuf);
        return bbuf.current - buffer;
    }

    static size_t msg_buf_size(Dl_info* pdli)
    {
        if (pdli->dli_fname && pdli->dli_sname && pdli->dli_saddr)
        {
            return (strlen(pdli->dli_fname) + strlen(pdli->dli_sname) + 43) * sizeof(char);
        }
        else if (pdli->dli_fname)
        {
            return (strlen(pdli->dli_fname) + 22) * sizeof(char);
        }
    }

    char **backtrace_symbols(void *const *buffer, int size)
    {
        int *ptr_offsets = (int *) malloc(size * sizeof(int));
        if (!ptr_offsets)
        {
            return 0;
        }
        memset(ptr_offsets, 0, size * sizeof(int));
        int i;
        size_t total_buf_size = size * sizeof(char **);
        for (i=0; i<size; i++)
        {
            Dl_info dli;
            if (!dladdr(buffer[i], &dli))
            {
                continue;
            }
            ptr_offsets[i] = total_buf_size;
            total_buf_size += msg_buf_size(&dli);
        }

        char ** msg = (char **) malloc(total_buf_size);
        if (!msg)
        {
            free(ptr_offsets);
            return 0;
        }
        memset(msg, 0, total_buf_size);
        for (i=0; i<size; i++)
        {
            msg[i] = msg + ptr_offsets[i];
        }
        free(ptr_offsets);
        for (i=0; i<size; i++)
        {
            Dl_info dli;
            if (!dladdr(buffer[i], &dli))
            {
                continue;
            }
            if (dli.dli_fname && dli.dli_sname && dli.dli_saddr)
            {
                snprintf(msg[i], msg_buf_size(&dli), "%s(%s+%#x) [%#llx]", dli.dli_fname, dli.dli_sname, buffer[i] - dli.dli_saddr, (unsigned long long) buffer[i]);
            }
            else if (dli.dli_fname)
            {
                snprintf(msg[i], msg_buf_size(&dli), "%s [%#llx]", dli.dli_fname, (unsigned long long) buffer[i]);
            }
        }
        return msg;
    }

#ifdef __cplusplus__
}
#endif

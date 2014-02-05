

#ifndef spj_debug_h
#define spj_debug_h

#include "types.h"


#ifdef __APPLE__
#include <execinfo.h>

// https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/backtrace.3.html
#define print_callstack() { \
    void *callstack[128]; \
    int i, frames = backtrace(callstack, 128); \
    char **strs = backtrace_symbols(callstack, frames); \
    printf("Callstack:\n"); \
    for (i = 0; i < frames; ++i) { \
        printf("%s\n", strs[i]); \
    } \
    free(strs); \
}
#else
#define print_callstack() ((void)0)
#endif


void spj_jsonvalue_debug(SpjJSONValue *jsonvalue);

#endif

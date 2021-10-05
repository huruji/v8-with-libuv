#ifndef V8_WITH_LIBUV_UTIL_H
#define V8_WITH_LIBUV_UTIL_H

#include "../deps/v8/include/v8.h"
using namespace v8;

static inline const char* ToCString(const String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}


#endif //V8_WITH_LIBUV_UTIL_H

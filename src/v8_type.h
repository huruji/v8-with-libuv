#ifndef V8_WITH_LIBUV_V8_TYPE_H
#define V8_WITH_LIBUV_V8_TYPE_H

#include "../deps/v8/include/v8.h"
#include "util.h"
using namespace v8;


static inline v8::Local<v8::String> v8_str(const char* x) {
    return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), x).ToLocalChecked();
}

static inline v8::Local<v8::Boolean> v8_bool(bool val) {
    return v8::Boolean::New(v8::Isolate::GetCurrent(), val);
}
static inline v8::Local<v8::Value> v8_num(double x) {
    return v8::Number::New(v8::Isolate::GetCurrent(), x);
}
static inline v8::Local<v8::Integer> v8_int(int32_t x) {
    return v8::Integer::New(v8::Isolate::GetCurrent(), x);
}
static inline v8::Local<v8::BigInt> v8_bigint(int64_t x) {
    return v8::BigInt::New(v8::Isolate::GetCurrent(), x);
}

static inline v8::Local<v8::Symbol> v8_symbol(const char* name) {
    return v8::Symbol::New(v8::Isolate::GetCurrent(), v8_str(name));
}



#endif //V8_WITH_LIBUV_V8_TYPE_H

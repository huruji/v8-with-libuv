//
// Created by huruji on 2021/10/7.
//

#ifndef V8_WITH_LIBUV_TIMER_H
#define V8_WITH_LIBUV_TIMER_H
#include "../deps/libuv/include/uv.h"
#include "v8_type.h"
#include "./app.h"
#include "./common.h"

class Timer : public common::ObjectWrap {
public:
    uv_timer_t *handle;
    static void New(const v8::FunctionCallbackInfo<v8::Value> &args);

public:
    Timer(){}

    void init(Local<Object> target);
//    void Wrap(v8::Local<v8::Object> handle);
};

#endif //V8_WITH_LIBUV_TIMER_H

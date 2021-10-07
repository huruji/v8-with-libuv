//
// Created by huruji on 2021/10/7.
//

#ifndef V8_WITH_LIBUV_TIMER_H
#define V8_WITH_LIBUV_TIMER_H
#include "../deps/libuv/include/uv.h"
#include "v8_type.h"

class Timer {
public:
    Local<Object> target;
    v8::Local<v8::Context> context;
    Isolate* isolate;

public:
    Timer(v8::Local<v8::Context> context, Isolate *isolate, Local<Object> target){
        this->target = target;
        this->context = context;
        this->isolate = isolate;
    }

    void init();
};

#endif //V8_WITH_LIBUV_TIMER_H

//
// Created by huruji on 2021/10/6.
//

#ifndef V8_WITH_LIBUV_PROCESS_H
#define V8_WITH_LIBUV_PROCESS_H
#include "v8_type.h"
#include "../deps/libuv/include/uv.h"

class Process {
public:
    v8::Local<v8::Context> context;
    Isolate* isolate;
public:
    void init(int argc, char* argv[]);

    Process(v8::Local<v8::Context> context, Isolate* isolate) {
        this->context = context;
        this->isolate = isolate;
    }
};


#endif //V8_WITH_LIBUV_PROCESS_H

//
// Created by huruji on 2021/10/6.
//

#ifndef V8_WITH_LIBUV_NODE_H
#define V8_WITH_LIBUV_NODE_H
#include "v8_type.h"
#include "../deps/libuv/include/uv.h"
#include "./timer.h"

class Node {
public:
    v8::Local<v8::Context> context;
    Isolate* isolate;
public:
    void init(int argc, char* argv[]);

    Node(v8::Local<v8::Context> context, Isolate* isolate) {
        this->context = context;
        this->isolate = isolate;
    }
};


#endif //V8_WITH_LIBUV_NODE_H

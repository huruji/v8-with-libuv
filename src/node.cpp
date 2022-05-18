//
// Created by huruji on 2021/10/6.
//
#include "node.h"

void Node::init(int argc, char* argv[], uv_loop_t* loop) {
    this->loop = loop;
    Local<Object> globalInstance = this->context->Global();

    Local<Object> node = Object::New(this->isolate);
    Timer timer;
    timer.init(node);

    globalInstance->Set(this->context, v8_str("node"), node);
}
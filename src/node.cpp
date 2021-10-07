//
// Created by huruji on 2021/10/6.
//
#include "node.h"

void Node::init(int argc, char* argv[]) {
    Local<Object> globalInstance = this->context->Global();

    Local<Object> node = Object::New(this->isolate);
    Timer timer(this->context, this->isolate, node);
    timer.init();

    globalInstance->Set(this->context, v8_str("node"), node);
}
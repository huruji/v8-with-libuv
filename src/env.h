//
// Created by huruji on 2021/10/29.
//

#ifndef V8_WITH_LIBUV_ENV_H
#define V8_WITH_LIBUV_ENV_H
#include "../deps/libuv/include/uv.h"
#include "../deps/v8/include/libplatform/libplatform.h"
#include "../deps/v8/include/v8.h"

const int kModuleEmbedderDataIndex = 32;
const int kInspectorClientIndex = 33;

class Environment {
public:
public:
    uv_loop_t *loop;
    v8::Persistent<v8::Function> onExit;
    v8::Persistent<v8::Function> onUnhandledRejection;
    v8::Persistent<v8::Object> err;
//    js_error *error;

    inline void AssignToContext(v8::Local<v8::Context> context)
    {
        context->SetAlignedPointerInEmbedderData(kModuleEmbedderDataIndex, this);
    }
    Environment()
    {
    }

private:
    ~Environment()
    {
    }
};

#endif //V8_WITH_LIBUV_ENV_H

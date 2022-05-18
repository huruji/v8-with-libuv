#ifndef V8_WITH_LIBUV_APP_H
#define V8_WITH_LIBUV_APP_H
#include "../deps/v8/include/libplatform/libplatform.h"
#include "v8_type.h"
#include "process.h"
#include "node.h"
#include "env.h"
using namespace v8;


class App {
public:
    std::unique_ptr<v8::Platform> platform;
    Isolate::CreateParams create_params;
    Isolate* isolate;
    v8::Local<v8::Context> context;
    Handle<ObjectTemplate> global;
    uv_loop_t* loop;

public:
    void createPlatform( char* argv[]);
    void createVM();
    void ShutdownVM();
    void initGlobal(int argc, char* argv[]);
    void setupGlobal(int argc, char* argv[]);

    char* readFile(const char* filename);

    void runScriptString(int argc, char* argv[], const char* scriptString);

    void runJSFile(int argc, char* argv[], const char* filename);

    inline void AssignToContext() {
        this->context->SetAlignedPointerInEmbedderData(32, this);
    }
};

#endif //V8_WITH_LIBUV_APP_H

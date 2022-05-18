#include "app.h"


inline bool ShouldAbortOnUncaughtException(v8::Isolate *isolate) {
    fprintf(stderr, "ShouldAbortOnUncaughtException\n");
    return true;
}
inline void OnFatalError(const char *location, const char *message) {
    fprintf(stderr, "FATAL ERROR: %s %s\n", location, message);
    fflush(stderr);
}

inline void OOMErrorHandler(const char *location, bool is_heap_oom) {
    fprintf(stderr, "OOM ERROR: %s %i\n", location, is_heap_oom);
    fflush(stderr);
}

void App::createPlatform(char *argv[]) {
    // v8 初始化
    V8::InitializeICUDefaultLocation(argv[0]);
    V8::InitializeExternalStartupData(argv[0]);
    this->platform = v8::platform::NewDefaultPlatform();
    V8::InitializePlatform(this->platform.get());
    V8::Initialize();
}

void App::createVM() {
    // 创建虚拟机
    this->create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();;
    this->isolate = v8::Isolate::New(this->create_params);
}

void App::ShutdownVM() {
    // 关闭虚拟机
    this->isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete this->create_params.array_buffer_allocator;
}

void App::initGlobal(int argc, char *argv[]) {
    this->global = v8::ObjectTemplate::New(this->isolate);
}

void App::setupGlobal(int argc, char *argv[]) {
    Local<Object> globalInstance = this->context->Global();
    Node node(this->context, this->isolate);
    node.init(argc, argv, this->loop);

    Process process(this->context, this->isolate);
    process.init(argc, argv);
}

char *App::readFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return new char[0];
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char *chars = new char[size + 1];
    chars[size] = '\0';

    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return new char[0];
        }
    }

    fclose(file);
    return chars;
}

void App::runScriptString(int argc, char *argv[], const char *scriptString) {
    this->isolate->SetAbortOnUncaughtExceptionCallback(ShouldAbortOnUncaughtException);
    this->isolate->SetFatalErrorHandler(OnFatalError);
    this->isolate->SetOOMErrorHandler(OOMErrorHandler);
    // 进入 scope
    v8::Isolate::Scope isolate_scope(this->isolate);
    v8::HandleScope handle_scope(this->isolate);
    this->initGlobal(argc, argv);
    this->context = v8::Context::New(this->isolate, NULL, this->global);
    Environment *env = new Environment();
    env->AssignToContext(this->context);
    env->loop = uv_default_loop();
    v8::Context::Scope context_scope(this->context);
    this->setupGlobal(argc, argv);
    v8::Local<v8::String> source = v8_str(scriptString);
    // 编译运行
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();
    script->Run(context);
}

void App::runJSFile(int argc, char *argv[], const char *filename) {
    const char *scriptStr = this->readFile(filename);
    this->runScriptString(argc, argv, scriptStr);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "../deps/v8/include/libplatform/libplatform.h"
#include "../deps/v8/include/v8.h"
#include "../deps/libuv/include/uv.h"

using namespace v8;
using namespace std;

const char* version = "1.0.0";

v8::Local<v8::String> v8_str(const char* x) {
    return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), x).ToLocalChecked();
}

void getVersion(const FunctionCallbackInfo<Value>& args) {
    args.GetReturnValue().Set(v8_str(version));
}

void log(const FunctionCallbackInfo<Value>& args) {
    for(int i = 0; i < args.Length(); i++) {
        if(i > 0) {
            fprintf(stdout, "");
        }
        String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = *str;
        fprintf(stdout, "%s", cstr);
    }
    fprintf(stdout, "\n");
}

const char* ToCString(const String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void getFileContent(const FunctionCallbackInfo<Value>& args) {
    String::Utf8Value str(args.GetIsolate(), args[0]);
    const char* filename = ToCString(str);
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        args.GetReturnValue().Set(v8_str(""));
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* chars = new char[size + 1];
    chars[size] = '\0';

    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            args.GetReturnValue().Set(v8_str(""));
        }
    }

    fclose(file);
    args.GetReturnValue().Set(v8_str(chars));
}

void XGetter(Local<String> property, const PropertyCallbackInfo<Value>& info) {
    info.GetReturnValue().Set(v8_str(version));
}

void XSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
}


void executeString(const FunctionCallbackInfo<Value>& args) {
    HandleScope handleScope(v8::Isolate::GetCurrent());
    Local<Context> context = args.GetIsolate()->GetCurrentContext();
    Local<Script> script = Script::Compile(context, args[0]->ToString(context).ToLocalChecked()).ToLocalChecked();
    script->Run(context);
}

void compile(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = v8::Isolate::GetCurrent();
    Local<Context> context = isolate->GetCurrentContext();
    ScriptCompiler::Source script_source(args[0]->ToString(context).ToLocalChecked());
    Local<String> params[] = {
            v8_str("require"),
            v8_str("exports"),
            v8_str("module"),
    };
    MaybeLocal<Function> fun = ScriptCompiler::CompileFunctionInContext(context, &script_source, 3, params,0, nullptr);
    if (fun.IsEmpty()) {
        args.GetReturnValue().Set(Undefined(isolate));
    } else {
        args.GetReturnValue().Set(fun.ToLocalChecked());
    }
}

class App {
public:
    std::unique_ptr<v8::Platform> platform;
    Isolate::CreateParams create_params;
    Isolate* isolate;
    v8::Local<v8::Context> context;
    Handle<ObjectTemplate> global;

public:
    void createPlatform( char* argv[]) {
        // v8 初始化
        V8::InitializeICUDefaultLocation(argv[0]);
        V8::InitializeExternalStartupData(argv[0]);
        this->platform = v8::platform::NewDefaultPlatform();
        V8::InitializePlatform(this->platform.get());
        V8::Initialize();
    }

    void createVM() {
        // 创建虚拟机
        this->create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();;
        this->isolate = v8::Isolate::New(this->create_params);
    }

    void ShutdownVM() {
        // 关闭虚拟机
        this->isolate->Dispose();
        V8::Dispose();
        V8::ShutdownPlatform();
        delete this->create_params.array_buffer_allocator;
    }

    Isolate* getIsolate() {
        return this->isolate;
    }

    void initGlobal(int argc, char* argv[]) {
        this->global = v8::ObjectTemplate::New(this->isolate);
        this->global->Set(this->isolate, "log", FunctionTemplate::New(this->isolate, log));

    }
    void setupGlobal(int argc, char* argv[]) {
        Local<Object> globalInstance = this->context->Global();
        Local<Object> node = Object::New(this->isolate);
        node->Set(this->context, v8_str("version"), v8_str("1.0.0"));
        Local<Array> arguments = Array::New(this->isolate, argc);
        for (int i = 0; i < argc; i++) {
            arguments->Set(this->context, Number::New(this->isolate, i), v8_str(argv[i]));
        }
        node->Set(this->context, v8_str("ARGV"), arguments);
        node->Set(this->context, v8_str("ReadFile"), Function::New(this->context,getFileContent).ToLocalChecked());
        node->Set(this->context, v8_str("Compile"), Function::New(this->context, compile).ToLocalChecked());
        globalInstance->Set(this->context, v8_str("NODE"), node);
    }

    char* readFile(const char* filename) {
        FILE* file = fopen(filename, "rb");
        if (file == NULL) {
            return new char[0];
        }
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);
        char* chars = new char[size + 1];
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

    void runScriptString(int argc, char* argv[], const char* scriptString) {
        // 进入 scope
        v8::Isolate::Scope isolate_scope(this->isolate);
        v8::HandleScope handle_scope(this->isolate);
        this->initGlobal(argc, argv);
        this->context = v8::Context::New(this->isolate, NULL, this->global);
        v8::Context::Scope context_scope(this->context);
        this->setupGlobal(argc, argv);
        v8::Local<v8::String> source = v8_str(scriptString);
        // 编译运行
        v8::Local<v8::Script> script =
                v8::Script::Compile(context, source).ToLocalChecked();
        script->Run(context);
    }

    void runJSFile(int argc, char* argv[], const char* filename) {
        const char* scriptStr = this->readFile(filename);
        this->runScriptString(argc, argv, scriptStr);
    }

};

int64_t num = 0;

void my_idle_cb(uv_idle_t* handle)
{
    num++;
    if (num >= 10e6) {
        printf("idle stop, num = %ld\n", num);
        uv_idle_stop(handle);
    }
}


int main(int argc, char* argv[]) {
    App app;
    app.createPlatform(argv);
    app.createVM();

    if (argc < 2) {
        app.ShutdownVM();
        return 0;
    }

    const char* filename = "./src/node.js";

    app.runJSFile(argc, argv, filename);

    uv_idle_t idler;

    uv_idle_init(uv_default_loop(), &idler);

    std::cout << "dle start" << std::endl;

    uv_idle_start(&idler, my_idle_cb);

    std::cout << "next" << std::endl;


    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    app.ShutdownVM();
    return 0;
}
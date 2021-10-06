#include "process.h"


void versionsCb(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = v8::Isolate::GetCurrent();
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> versions = Object::New(isolate);
    versions->Set(context, v8_str("node"), v8_str("1.0.0"));
    versions->Set(context, v8_str("v8"), v8_str(v8::V8::GetVersion()));
    versions->Set(context, v8_str("uv"), v8_str(uv_version_string()));
    args.GetReturnValue().Set(versions);
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


void print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            printf(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}

void print_error(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    printf("Error: ");
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            printf(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stderr);
}


void Process::init(int argc, char* argv[]) {
    Local<Object> globalInstance = this->context->Global();


    Local<Object> process = Object::New(this->isolate);
    process->Set(this->context, v8_str("versions"), Function::New(this->context, versionsCb).ToLocalChecked());
    Local<Array> arguments = Array::New(this->isolate, argc);
    for (int i = 0; i < argc; i++) {
        arguments->Set(this->context, Number::New(this->isolate, i), v8_str(argv[i]));
    }
    process->Set(this->context, v8_str("ARGV"), arguments);
    process->Set(this->context, v8_str("Compile"), Function::New(this->context, compile).ToLocalChecked());
    process->Set(this->context, v8_str("ReadFile"), Function::New(this->context,getFileContent).ToLocalChecked());
    process->Set(this->context, v8_str("Print"), Function::New(this->context, print).ToLocalChecked());
    process->Set(this->context, v8_str("PrintError"), Function::New(this->context, print_error).ToLocalChecked());
    globalInstance->Set(this->context, v8_str("process"), process);
}
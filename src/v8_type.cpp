#include "v8_type.h"

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
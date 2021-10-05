#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "../deps/v8/include/libplatform/libplatform.h"
#include "../deps/v8/include/v8.h"
#include "../deps/libuv/include/uv.h"
#include "./app.h"

using namespace v8;
using namespace std;

const char* version = "1.0.0";

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
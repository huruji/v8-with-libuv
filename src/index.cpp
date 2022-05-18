#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "../deps/v8/include/libplatform/libplatform.h"
#include "../deps/v8/include/v8.h"
#include "../deps/libuv/include/uv.h"
#include "./app.h"
#include "./process.h"

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

static void once_cb(uv_timer_t* handle) {
    printf("data");
    int a = *(int *)handle->data;
    cout << a << endl;
    printf("ONCE_CB %d\n");
}

int main(int argc, char* argv[]) {
    App app;
    app.createPlatform(argv);
    app.createVM();
    app.loop = uv_default_loop();

    const char* filename = "./src/node.js";

//    uv_idle_t idler;
//
//    uv_idle_init(uv_default_loop(), &idler);
    app.runJSFile(argc, argv, filename);
    bool more;
    do {
        v8::platform::PumpMessageLoop(app.platform.get(), app.isolate);
        more = uv_run(app.loop, UV_RUN_DEFAULT);
        if (more == false) {
            v8::platform::PumpMessageLoop(app.platform.get(), app.isolate);
            more = uv_loop_alive(app.loop);
            int isRun = uv_run(app.loop, UV_RUN_NOWAIT);
            if (uv_run(app.loop, UV_RUN_NOWAIT) != 0) {
                more = true;
            }
        }

    } while (more == true);
//    app.ShutdownVM();
    return 0;
}
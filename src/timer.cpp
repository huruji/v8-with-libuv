#include "./timer.h"
#include <iostream>


class TimerWrap {
public:
    uv_timer_t timer_req;
    FunctionCallbackInfo<Value> pargs;
public:
    void init(){
      timer_req.data = this;
    }

    TimerWrap(FunctionCallbackInfo<Value> pargs) : pargs(pargs) {
        this->pargs = pargs;
    }
public:

};

void timerCb(uv_timer_t* handle) {
    TimerWrap* timerWrap = (TimerWrap *)handle->data;
    FunctionCallbackInfo<Value> args = timerWrap->pargs;

    int64_t delay = args[0]->IntegerValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
    Local<Function> callback = Local<Function>::Cast(args[2]);
    const unsigned argc = 0;
    Local<Value> argv[] = {};
    callback->Call(args.GetIsolate()->GetCurrentContext(), Null(args.GetIsolate()) , 0, argv);
}


static void timerStartCallback(const FunctionCallbackInfo<Value>& args) {
    uv_timer_t timer_req;
    int64_t delay = args[0]->IntegerValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
    int64_t repeat = args[1]->IntegerValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
    TimerWrap timerWrap(args);
    timerWrap.init();
    uv_timer_init(uv_default_loop(), &timerWrap.timer_req);
    uv_timer_start(&timerWrap.timer_req, timerCb, delay, repeat);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

void timerCb(const FunctionCallbackInfo<Value>& args) {
   if(args.IsConstructCall()) {
       args.GetReturnValue().Set(args.This());
   }
   args.GetReturnValue().Set(Undefined(args.GetIsolate()));
}


void Timer::init() {
    uv_loop_t* loop = uv_default_loop();
    Local<FunctionTemplate> timerFn = FunctionTemplate::New(this->isolate, timerCb);
    timerFn->PrototypeTemplate()->Set(this->isolate, "start", FunctionTemplate::New(this->isolate, timerStartCallback));
    this->target->Set(this->context, v8_str("Timer"), timerFn->GetFunction(this->context).ToLocalChecked());
}

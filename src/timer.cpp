#include "./timer.h"
#include <iostream>


uv_timer_t *global_timer_req;

class TimerWrap {
public:
//    uv_timer_t timer_req;
    FunctionCallbackInfo<Value> pargs;
    Local<Function> cb;
    Local<Context> context;
    Isolate* isolate;
public:
    void init(){
//        global_timer_req->data = this;
//      this->timer_req = *globalTimer->handle;
    }

    TimerWrap(Isolate* isolate, FunctionCallbackInfo<Value> pargs) : pargs(pargs) {
        this->isolate = isolate;
        this->pargs = pargs;
    }
public:

};

void timerCb(uv_timer_t* handle) {
    TimerWrap* timerWrap = (TimerWrap *)handle->data;
//    Local<Function> callback = Local<Function>::Cast(*args2);
    Local<Value> argv[] = {};

    Isolate* isolate = timerWrap->isolate;
    std::cout << "isolate" << std::endl;
    if (isolate->IsDead()){
        printf("isolate is dead\n");
    }
    Local<Context> context = timerWrap->context;
    Local<Function> callback = Local<Function>::Cast(timerWrap->pargs[2]);
    if (context.IsEmpty()) {
        printf("context is null\n");
    }
    if (timerWrap->context.IsEmpty()) {
        printf("timer wrap context is null\n");
    }
    std::cout << "callback" << sizeof callback << std::endl;
    callback->Call(context, Null(isolate), 0, argv);
    std::cout << "timerwrap" << sizeof timerWrap->pargs[2] << std::endl;
//    args2->As<Function>()->Call()
//    Local<Context>
//    args2->As<Function>()->Call(context, Null(isolate) , 0, argv);

}


static void timerStartCallback(const FunctionCallbackInfo<Value>& args) {
    int64_t delay = args[0]->IntegerValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
    int64_t repeat = args[1]->IntegerValue(args.GetIsolate()->GetCurrentContext()).ToChecked();

    Local<Function> callback = Local<Function>::Cast(args[2]);
    TimerWrap timerWrap(args.GetIsolate(), args);
    timerWrap.context = args.GetIsolate()->GetCurrentContext();
    timerWrap.init();
    global_timer_req->data = &timerWrap;
    uv_timer_start(global_timer_req, timerCb, delay, repeat);
}

void timerCb(const FunctionCallbackInfo<Value>& args) {
   if(args.IsConstructCall()) {
       args.GetReturnValue().Set(args.This());
   }
   args.GetReturnValue().Set(Undefined(args.GetIsolate()));
}

void Timer::New(const v8::FunctionCallbackInfo<v8::Value> &args){
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    if(args.IsConstructCall()) {
        Timer *timer = new Timer();
        App *app = static_cast<App *>(context->GetAlignedPointerFromEmbedderData(32));
        timer->handle = (uv_timer_t *)calloc(1, sizeof(uv_timer_t));
        int r = uv_timer_init(app->loop, timer->handle);
        global_timer_req = timer->handle;
        args.GetReturnValue().Set(args.This());
    }
}


void Timer::init(Local<Object> target) {
    Isolate* isolate = v8::Isolate::GetCurrent();
    Local<Context> context = isolate->GetCurrentContext();
    Local<FunctionTemplate> timerFn = FunctionTemplate::New(isolate, New);
    timerFn->PrototypeTemplate()->Set(isolate, "start", FunctionTemplate::New(isolate, timerStartCallback));
    target->Set(context, v8_str("Timer"), timerFn->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

//void Timer::Wrap(v8::Local<v8::Object> handle)
//{
//    handle->SetAlignedPointerInInternalField(0, this);
//}
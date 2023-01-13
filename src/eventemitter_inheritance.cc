#include "eventemitter_inheritance.h"

Napi::FunctionReference *MyEmitter::EventEmitter = nullptr;

MyEmitter::MyEmitter(const Napi::CallbackInfo &info) : ObjectWrap(info) {
  // Call the super class constructor
  MyEmitter::EventEmitter->Call(this->Value(), {});
}

void MyEmitter::Ping(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Object self = this->Value().ToObject();
  Napi::Function emit = self.Get("emit").As<Napi::Function>();
  emit.Call(this->Value(),
            {Napi::String::New(env, "data"), Napi::String::New(env, "pong")});
}

Napi::Function MyEmitter::GetClass(Napi::Env env) {
  Napi::Function self =
      DefineClass(env, "MyEmitter",
                  {
                      InstanceMethod("ping", &MyEmitter::Ping),
                  });

  // require from C++ needs some acrobatics (the ugly part is in the JS file)
  Napi::Function require = env.Global().Get("require").As<Napi::Function>();
  Napi::Function ee = require.Call({Napi::String::New(env, "events")})
                          .ToObject()
                          .Get("EventEmitter")
                          .As<Napi::Function>();

  // This is the visible inheritance
  Napi::Function setProto = env.Global()
                                .Get("Object")
                                .ToObject()
                                .Get("setPrototypeOf")
                                .As<Napi::Function>();
  setProto.Call({self, ee});
  setProto.Call({self.Get("prototype"), ee.Get("prototype")});

  // Keep a reference to the constructor
  MyEmitter::EventEmitter = new Napi::FunctionReference();
  *MyEmitter::EventEmitter = Napi::Persistent(ee);
  return self;
}

Napi::Value SetupEEInheritance(Napi::Env env, Napi::Object exports) {
  exports.Set("MyEmitter", MyEmitter::GetClass(env));
  return exports;
}

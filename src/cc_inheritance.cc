#include "cc_inheritance.h"
#include <string>

using namespace std::string_literals;
using namespace Napi;

template <class T>
Base<T>::Base(const Napi::CallbackInfo &info) : ObjectWrap<T>(info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "You need to name yourself")
        .ThrowAsJavaScriptException();
    return;
  }

  this->name = info[0].As<Napi::String>().Utf8Value();
}

template <class T> Napi::Value Base<T>::Greet(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "Hello "s + this->name);
}

template <class T>
Napi::Value Base<T>::Mention(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, this->name);
}

template <class T> Napi::Function Base<T>::GetClass(Napi::Env env) {
  return DefineClass(env, "Base",
                     {
                         InstanceMethod("greet", &Base::Greet),
                         InstanceMethod("mention", &Base::Mention),
                     });
}

Extended::Extended(const Napi::CallbackInfo &info) : Base(info) {
  Napi::Env env = info.Env();

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "You need to selected a honorific")
        .ThrowAsJavaScriptException();
    return;
  }

  this->honorific = info[1].As<Napi::String>().Utf8Value();
}

Napi::Value Extended::Greet(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env,
                           "Hello "s + this->honorific + " "s + this->name);
}

Napi::Value Extended::Title(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, this->honorific);
}

Napi::Function Extended::GetClass(Napi::Env env) {
  return ObjectWrap<Extended>::DefineClass(
      env, "Extended",
      {
          // This is how we signal to JS that we want to override a method
          // (quirk 1)
          ObjectWrap<Extended>::InstanceMethod("greet", &Extended::Greet),
          ObjectWrap<Extended>::InstanceMethod("title", &Extended::Title),
          // This is how we signal to JS that we want to inherit a method
          // (quirk 2)
          ObjectWrap<Extended>::InstanceMethod("mention", &Base::Mention),
      });
}

// The magic happens here
Napi::Value SetupCCInheritance(Napi::Env env, Napi::Object exports) {
  Napi::Function ClassBase = BaseInst::GetClass(env);
  Napi::Function ClassExtended = Extended::GetClass(env);
  exports.Set("Base", ClassBase);
  exports.Set("Extended", ClassExtended);
  Napi::Function setProto = env.Global()
                                .Get("Object")
                                .ToObject()
                                .Get("setPrototypeOf")
                                .As<Napi::Function>();
  setProto.Call({ClassExtended, ClassBase});
  setProto.Call({ClassExtended.Get("prototype"), ClassBase.Get("prototype")});
  return exports;
}
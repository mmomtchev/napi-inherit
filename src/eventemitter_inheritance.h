#include <napi.h>

class MyEmitter : public Napi::ObjectWrap<MyEmitter> {
public:
  MyEmitter(const Napi::CallbackInfo &);

  void Ping(const Napi::CallbackInfo &);

  static Napi::Function GetClass(Napi::Env);

private:
  // This is the quirk
  static Napi::FunctionReference *EventEmitter;
};

Napi::Value SetupEEInheritance(Napi::Env, Napi::Object);

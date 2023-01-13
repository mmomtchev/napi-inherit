#include "cc_inheritance.h"
#include "eventemitter_inheritance.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  SetupCCInheritance(env, exports);
  SetupEEInheritance(env, exports);
  return exports;
}

NODE_API_MODULE(addon, Init)

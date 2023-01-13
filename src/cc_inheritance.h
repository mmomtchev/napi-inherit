#pragma once

#include <napi.h>

// This will be the base class, it is a CRTP class just like ObjectWrap
template <class T> class Base : public Napi::ObjectWrap<T> {
public:
  Base(const Napi::CallbackInfo &);
  virtual ~Base(){};

  // This method will be overriden in the subclass
  // The C++ virtual is not needed/enforced when calling from JS
  // (quirk 1)
  virtual Napi::Value Greet(const Napi::CallbackInfo &);

  // This method will be inherited by the subclass
  // But we will have to do it explicitly
  // (quirk 2)
  Napi::Value Mention(const Napi::CallbackInfo &);

  static Napi::Function GetClass(Napi::Env);

protected:
  std::string name;
};

// This will be the class that will instantiate when we need Base
// (Base itself is not instantiable)
class BaseInst : public Base<BaseInst> {
public:
  using Base::Base;
  virtual ~BaseInst(){};
};

// This is the extended class
class Extended : public Base<Extended> {
public:
  Extended(const Napi::CallbackInfo &);
  virtual ~Extended(){};

  // This method should override the one from Base
  virtual Napi::Value Greet(const Napi::CallbackInfo &);

  // This is a method present only in the extended class
  Napi::Value Title(const Napi::CallbackInfo &);

  static Napi::Function GetClass(Napi::Env);

protected:
  std::string honorific;
};

Napi::Value SetupCCInheritance(Napi::Env, Napi::Object);

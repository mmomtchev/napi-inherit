# Inheritance in both C++ and JavaScript with N-API

Currently inheritance of C++ classes exported to JavaScript is not supported officially in N-API / `node-addon-api`.

It is the last major show-stopper for porting complex `Nan` projects to `node-addon-api`.

There are a number of issues with heated debate about this problem (this is the first one: [`node-addon-api#229`](https://github.com/nodejs/node-addon-api/issues/229)) and various proposals for each one of which at least one party with veto powers does not agree to at least one point. The result is that this issue remains stalled for almost 5 years now.

There is no need to restart the discussions - if there was a perfect solution it would have been adopted by now - but I will just summarize the problem:

V8 has a specific object model where there are `Object`s and `ObjectTemplate`s. This object model does not apply to other JS engines. In order to create a `Function` one must first create a `FunctionTemplate` and then instantiate the template. `ObjectWrap::DefineClass` which relies on `napi_define_class` does this behind the scenes and returns only the `Function` object. In order to inherit it from another class, one needs the `FunctionTemplate` which is not returned by `napi_define_class`. V8 does not allow to retrieve the `FunctionTemplate` from a `Function` (in fact it does not store it anywhere - which means that this would be a substantial modification).

The methods shown here are not perfect but they get the job done satisfying all the requirements:

- They use only N-API and they are independent of the Node.js / V8 version used
- The classes appear to be inherited to the JS end-user (`instanceof` works as expected)
- The C++ implementations can share method code in a base class
- They cover the two most common use-cases: C++ inheritance and `EventEmitter` inheritance

# C++

The drawbacks of the C++ inheritance are:

- Duplication of declaration between C++ and JS - but this has always been the case:
  - C++ `virtual` in particular must be declared twice (see quirk 1)
  - All inheritance must be manually mentioned (see quirk 2)
- All classes that are going to serve as a base class must be a CRTP class
  - Special _leaf_ classes must be used to instantiate these classes (see `BaseInst` in the C++ example).

# EventEmitter

The drawbacks of the `EventEmitter` inheritance are:

- `require()` from C++ is ugly
- One must keep a reference to the super-class and to manually call it in the constructor

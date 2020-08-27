#pragma once

#include <memory>
#include <napi.h>

// liboqs-cpp
#include "oqs_cpp.h"

namespace KeyEncapsulation {

  class KeyEncapsulation : public Napi::ObjectWrap<KeyEncapsulation> {
    private:
      std::unique_ptr<oqs::KeyEncapsulation> oqsKE;

    public:
      explicit KeyEncapsulation(const Napi::CallbackInfo& info);
      Napi::Value getDetails(const Napi::CallbackInfo& info);
      Napi::Value generateKeypair(const Napi::CallbackInfo& info);
      Napi::Value exportSecretKey(const Napi::CallbackInfo& info);
      Napi::Value encapsulateSecret(const Napi::CallbackInfo& info);
      Napi::Value decapsulateSecret(const Napi::CallbackInfo& info);

      static void Init(Napi::Env env, Napi::Object exports);
  };

  void Init(Napi::Env env, Napi::Object exports);

}

#pragma once

#include <memory>
#include <napi.h>

// liboqs-cpp
#include "oqs_cpp.h"

namespace Signature {

  class Signature : public Napi::ObjectWrap<Signature> {
    private:
      std::unique_ptr<oqs::Signature> oqsSig;

    public:
      explicit Signature(const Napi::CallbackInfo& info);
      Napi::Value getDetails(const Napi::CallbackInfo& info);
      Napi::Value generateKeypair(const Napi::CallbackInfo& info);
      Napi::Value exportSecretKey(const Napi::CallbackInfo& info);
      Napi::Value sign(const Napi::CallbackInfo& info);
      Napi::Value verify(const Napi::CallbackInfo& info);

      static void Init(Napi::Env env, Napi::Object exports);
  };

  void Init(Napi::Env env, Napi::Object exports);

}

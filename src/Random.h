#pragma once

#include <napi.h>

namespace Random {

  Napi::Value switchAlgorithm(const Napi::CallbackInfo& info);
  Napi::Value randomBytes(const Napi::CallbackInfo& info);
  Napi::Value initNistKat(const Napi::CallbackInfo& info);

  void Init(Napi::Env env, Napi::Object exports);

}

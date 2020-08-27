#pragma once

#include <napi.h>

namespace KEMs {

  Napi::Value getEnabledAlgorithms(const Napi::CallbackInfo& info);
  Napi::Value isAlgorithmEnabled(const Napi::CallbackInfo& info);

  void Init(Napi::Env env, Napi::Object exports);

}

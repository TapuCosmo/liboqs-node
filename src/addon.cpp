#include <napi.h>

#include "KEMs.h"
#include "KeyEncapsulation.h"
#include "Random.h"
#include "Signature.h"
#include "Sigs.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  KEMs::Init(env, exports);
  KeyEncapsulation::Init(env, exports);
  Random::Init(env, exports);
  Signature::Init(env, exports);
  Sigs::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)

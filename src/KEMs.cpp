// exports.KEMs

#include "KEMs.h"

#include <cstdint>
#include <vector>
#include <napi.h>

// liboqs-cpp
#include "oqs_cpp.h"

/** @namespace KEMs */
namespace KEMs {

  /**
   * The different KEM algorithms that can be used.
   * Use {@link KEMs.getEnabledAlgorithms} for an array of available algorithms.
   * @memberof KEMs
   * @typedef {string} Algorithm
   */

  /**
   * Gets an array of KEM algorithms that were enabled at compile-time and are available for use.
   * @memberof KEMs
   * @name getEnabledAlgorithms
   * @static
   * @method
   * @returns {KEMs.Algorithm[]} - A list of enabled KEM algorithms.
   */
  Napi::Value getEnabledAlgorithms(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const std::vector<std::string> enabledKEMs = oqs::KEMs::get_enabled_KEMs();
    std::size_t numDefaultKEMs = 0;
    for (auto kem : enabledKEMs) {
      // Not expecting any other default algorithms to be created
      if (kem == "DEFAULT") {
        numDefaultKEMs++;
      }
    }
    auto enabledKEMsArray = Napi::Array::New(env, enabledKEMs.size() - numDefaultKEMs);
    for (std::size_t i = 0; auto kem : enabledKEMs) {
      if (kem != "DEFAULT") {
        enabledKEMsArray[i++] = kem;
      }
    }
    return enabledKEMsArray;
  }

  /**
   * Checks if an algorithm was enabled at compile-time and is available for use.
   * @memberof KEMs
   * @name isAlgorithmEnabled
   * @static
   * @method
   * @param {KEMs.Algorithm} algorithm - The algorithm to check.
   * @returns {boolean} - Whether the algorithm is enabled.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   */
  Napi::Value isAlgorithmEnabled(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::TypeError::New(env, "Algorithm must be a string");
    }
    if (!info[0].IsString()) {
      throw Napi::TypeError::New(env, "Algorithm must be a string");
    }
    const auto algorithm = info[0].As<Napi::String>().Utf8Value();
    auto isEnabled = Napi::Boolean::New(
      env,
      oqs::KEMs::is_KEM_enabled(algorithm)
    );
    return isEnabled;
  }

  void Init(Napi::Env env, Napi::Object exports) {
    auto KEMsExports = Napi::Object::New(env);
    KEMsExports.Set(
      Napi::String::New(env, "getEnabledAlgorithms"),
      Napi::Function::New(env, getEnabledAlgorithms)
    );
    KEMsExports.Set(
      Napi::String::New(env, "isAlgorithmEnabled"),
      Napi::Function::New(env, isAlgorithmEnabled)
    );
    exports.Set(
      Napi::String::New(env, "KEMs"),
      KEMsExports
    );
  }

} // namespace KEMs

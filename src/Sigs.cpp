// exports.Sigs

#include "Sigs.h"

#include <cstdint>
#include <vector>
#include <napi.h>

// liboqs-cpp
#include "oqs_cpp.h"

/** @namespace Sigs */
namespace Sigs {

  /**
   * The different signature algorithms that can be used.
   * Use {@link Sigs.getEnabledAlgorithms} for an array of available algorithms.
   * @memberof Sigs
   * @typedef {string} Algorithm
   */

  /**
   * Gets an array of signature algorithms that were enabled at compile-time and are available for use.
   * @memberof Sigs
   * @name getEnabledAlgorithms
   * @static
   * @method
   * @returns {Sigs.Algorithm[]} - A list of enabled signature algorithms.
   */
  Napi::Value getEnabledAlgorithms(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const std::vector<std::string> enabledSigs = oqs::Sigs::get_enabled_sigs();
    std::size_t numDefaultSigs = 0;
    for (auto sig : enabledSigs) {
      // Not expecting any other default algorithms to be created
      if (sig == "DEFAULT") {
        numDefaultSigs++;
      }
    }
    auto enabledSigsArray = Napi::Array::New(env, enabledSigs.size() - numDefaultSigs);
    for (std::size_t i = 0; auto sig : enabledSigs) {
      if (sig != "DEFAULT") {
        enabledSigsArray[i++] = sig;
      }
    }
    return enabledSigsArray;
  }

  /**
   * Checks if an algorithm was enabled at compile-time and is available for use.
   * @memberof Sigs
   * @name isAlgorithmEnabled
   * @static
   * @method
   * @param {Sigs.Algorithm} algorithm - The algorithm to check.
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
      oqs::Sigs::is_sig_enabled(algorithm)
    );
    return isEnabled;
  }

  void Init(Napi::Env env, Napi::Object exports) {
    auto SigsExports = Napi::Object::New(env);
    SigsExports.Set(
      Napi::String::New(env, "getEnabledAlgorithms"),
      Napi::Function::New(env, getEnabledAlgorithms)
    );
    SigsExports.Set(
      Napi::String::New(env, "isAlgorithmEnabled"),
      Napi::Function::New(env, isAlgorithmEnabled)
    );
    exports.Set(
      Napi::String::New(env, "Sigs"),
      SigsExports
    );
  }

} // namespace Sigs

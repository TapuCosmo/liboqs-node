// exports.Signature

#include "Signature.h"

#include <cstdint>
#include <memory>
#include <new>
#include <vector>
#include <napi.h>

// liboqs-cpp
#include "oqs_cpp.h"
#include "common.h"

namespace Signature {

  using oqs::byte;
  using oqs::bytes;

  /**
   * Constructs an instance of Signature.
   * @name Signature
   * @class
   * @constructs Signature
   * @param {Sigs.Algorithm} algorithm - The signature algorithm to use.
   * @param {Buffer} [secretKey] - An optional secret key. If not specified, use Signature#generateKeypair later to create a secret key.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   */
  Signature::Signature(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Signature>(info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::TypeError::New(env, "Algorithm must be a string");
    }
    if (!info[0].IsString()) {
      throw Napi::TypeError::New(env, "Algorithm must be a string");
    }
    const auto algorithm = info[0].As<Napi::String>().Utf8Value();
    if (info.Length() >= 2) {
      if (!info[1].IsBuffer()) {
        throw Napi::TypeError::New(env, "Secret key must be a buffer");
      }
      const auto secretKeyBuffer = info[1].As<Napi::Buffer<byte>>();
      const auto secretKeyData = secretKeyBuffer.Data();
      const bytes secretKeyVec(secretKeyData, secretKeyData + secretKeyBuffer.Length());
      try {
        oqsSig = std::make_unique<oqs::Signature>(algorithm, secretKeyVec);
      } catch (const std::exception& ex) {
        throw Napi::TypeError::New(env, ex.what());
      }
    } else {
      try {
        oqsSig = std::make_unique<oqs::Signature>(algorithm);
      } catch (const std::exception& ex) {
        throw Napi::TypeError::New(env, ex.what());
      }
    }
  }

  /**
   * Gets the details for the signature algorithm that the instance was constructed with.
   * @memberof Signature
   * @instance
   * @method
   * @name getDetails
   * @returns {Object} - An object containing the details of the signature algorithm.
   */
  Napi::Value Signature::getDetails(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const oqs::Signature::SignatureDetails details = oqsSig->get_details();
    auto detailsObj = Napi::Object::New(env);
    detailsObj.Set(
      Napi::String::New(env, "name"),
      Napi::String::New(env, details.name)
    );
    detailsObj.Set(
      Napi::String::New(env, "version"),
      Napi::String::New(env, details.version)
    );
    detailsObj.Set(
      Napi::String::New(env, "claimedNistLevel"),
      Napi::Number::New(env, details.claimed_nist_level)
    );
    detailsObj.Set(
      Napi::String::New(env, "isEUFCMA"),
      Napi::Boolean::New(env, details.is_euf_cma)
    );
    detailsObj.Set(
      Napi::String::New(env, "publicKeyLength"),
      Napi::Number::New(env, details.length_public_key)
    );
    detailsObj.Set(
      Napi::String::New(env, "secretKeyLength"),
      Napi::Number::New(env, details.length_secret_key)
    );
    detailsObj.Set(
      Napi::String::New(env, "maxSignatureLength"),
      Napi::Number::New(env, details.max_length_signature)
    );
    return detailsObj;
  }

  /**
   * Generates a keypair. Overwrites any existing secret key on the instance with the generated secret key.
   * @memberof Signature
   * @instance
   * @method
   * @name generateKeypair
   * @returns {Buffer} - A Buffer containing the public key.
   * @throws {Error} Will throw an error if memory cannot be allocated.
   */
  Napi::Value Signature::generateKeypair(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
      const bytes publicKeyVec = oqsSig->generate_keypair();
      bytes* publicKeyVecCopy = new bytes(publicKeyVec);
      if (publicKeyVecCopy == nullptr) {
        throw Napi::Error::New(env, "Failed to allocate memory");
      }
      Napi::MemoryManagement::AdjustExternalMemory(env, publicKeyVecCopy->size());
      return Napi::Buffer<byte>::New(
        env,
        publicKeyVecCopy->data(),
        publicKeyVecCopy->size(),
        [](Napi::Env cbEnv, byte* /* unused */, bytes* vec) -> void {
          if (vec != nullptr) {
            Napi::MemoryManagement::AdjustExternalMemory(cbEnv, -vec->size());
            // No need to secure free public key
          }
          delete vec;
        },
        publicKeyVecCopy
      );
    } catch (const std::exception& ex) {
      throw Napi::Error::New(env, ex.what());
    }
  }

  /**
   * Exports the secret key.
   * @memberof Signature
   * @instance
   * @method
   * @name exportSecretKey
   * @returns {Buffer} - A Buffer containing the secret key.
   * @throws {Error} Will throw an error if memory cannot be allocated.
   */
  Napi::Value Signature::exportSecretKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bytes secretKeyVec = oqsSig->export_secret_key();
    bytes* secretKeyVecCopy = new (std::nothrow) bytes(secretKeyVec);
    oqs::mem_cleanse(secretKeyVec);
    if (secretKeyVecCopy == nullptr) {
      throw Napi::Error::New(env, "Failed to allocate memory");
    }
    Napi::MemoryManagement::AdjustExternalMemory(env, secretKeyVecCopy->size());
    return Napi::Buffer<byte>::New(
      env,
      secretKeyVecCopy->data(),
      secretKeyVecCopy->size(),
      [](Napi::Env cbEnv, byte* /* unused */, bytes* vec) -> void {
        if (vec != nullptr) {
          Napi::MemoryManagement::AdjustExternalMemory(cbEnv, -vec->size());
          oqs::mem_cleanse(*vec);
        }
        delete vec;
      },
      secretKeyVecCopy
    );
  }

  /**
   * Signs a message.
   * @memberof Signature
   * @instance
   * @method
   * @name sign
   * @param {Buffer} message - The message to sign.
   * @returns {Buffer} - The signature for the message.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   * @throws {Error} Will throw an error if memory cannot be allocated.
   */
  Napi::Value Signature::sign(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::TypeError::New(env, "Message must be a buffer");
    }
    if (!info[0].IsBuffer()) {
      throw Napi::TypeError::New(env, "Message must be a buffer");
    }
    const auto messageBuffer = info[0].As<Napi::Buffer<byte>>();
    const auto messageData = messageBuffer.Data();
    const bytes messageVec(messageData, messageData + messageBuffer.Length());
    try {
      bytes signatureVec = oqsSig->sign(messageVec);
      bytes* signatureVecCopy = new (std::nothrow) bytes(signatureVec);
      if (signatureVecCopy == nullptr) {
        throw Napi::Error::New(env, "Failed to allocate memory");
      }
      Napi::MemoryManagement::AdjustExternalMemory(env, signatureVecCopy->size());
      return Napi::Buffer<byte>::New(
        env,
        signatureVecCopy->data(),
        signatureVecCopy->size(),
        [](Napi::Env cbEnv, byte* /* unused */, bytes* vec) -> void {
          if (vec != nullptr) {
            Napi::MemoryManagement::AdjustExternalMemory(cbEnv, -vec->size());
            // No need to secure free signature
          }
          delete vec;
        },
        signatureVecCopy
      );
    } catch (const std::exception& ex) {
      throw Napi::TypeError::New(env, ex.what());
    }
  }

  /**
   * Verifies the signature belonging to a message using a public key.
   * @memberof Signature
   * @instance
   * @method
   * @name verify
   * @param {Buffer} message - The message that was signed to produce the signature.
   * @param {Buffer} signature - The signature to verify.
   * @param {Buffer} publicKey - The public key to verify the signature against.
   * @returns {boolean} - Whether the message has a valid signature from the owner of the public key.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   */
  Napi::Value Signature::verify(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
      throw Napi::TypeError::New(env, "Message, signature, and publicKey must be buffers");
    }
    if (!info[0].IsBuffer() || !info[1].IsBuffer() || !info[2].IsBuffer()) {
      throw Napi::TypeError::New(env, "Message, signature, and publicKey must be buffers");
    }

    const auto messageBuffer = info[0].As<Napi::Buffer<byte>>();
    const auto messageData = messageBuffer.Data();
    const bytes messageVec(messageData, messageData + messageBuffer.Length());

    const auto signatureBuffer = info[1].As<Napi::Buffer<byte>>();
    const auto signatureData = signatureBuffer.Data();
    const bytes signatureVec(signatureData, signatureData + signatureBuffer.Length());

    const auto publicKeyBuffer = info[2].As<Napi::Buffer<byte>>();
    const auto publicKeyData = publicKeyBuffer.Data();
    const bytes publicKeyVec(publicKeyData, publicKeyData + publicKeyBuffer.Length());

    try {
      bool valid = oqsSig->verify(messageVec, signatureVec, publicKeyVec);
      return Napi::Boolean::New(env, valid);
    } catch (const std::exception& ex) {
      throw Napi::TypeError::New(env, ex.what());
    }
  }

  void Signature::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Signature", {
      InstanceMethod<&Signature::getDetails>("getDetails"),
      InstanceMethod<&Signature::generateKeypair>("generateKeypair"),
      InstanceMethod<&Signature::exportSecretKey>("exportSecretKey"),
      InstanceMethod<&Signature::sign>("sign"),
      InstanceMethod<&Signature::verify>("verify")
    });
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set(
      Napi::String::New(env, "Signature"),
      func
    );
    env.SetInstanceData<Napi::FunctionReference>(constructor);
  }

  void Init(Napi::Env env, Napi::Object exports) {
    Signature::Init(env, exports);
  }

} // namespace Signature

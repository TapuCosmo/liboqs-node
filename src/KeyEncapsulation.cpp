// exports.KeyEncapsulation

#include "KeyEncapsulation.h"

#include <cstdint>
#include <memory>
#include <new>
#include <vector>
#include <napi.h>

// liboqs-cpp
#include "oqs_cpp.h"
#include "common.h"

namespace KeyEncapsulation {

  using oqs::byte;
  using oqs::bytes;

  /**
   * Constructs an instance of KeyEncapsulation.
   * @name KeyEncapsulation
   * @class
   * @constructs KeyEncapsulation
   * @param {KEMs.Algorithm} algorithm - The KEM algorithm to use.
   * @param {Buffer} [secretKey] - An optional secret key. If not specified, use KeyEncapsulation#generateKeypair later to create a secret key.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   */
  KeyEncapsulation::KeyEncapsulation(const Napi::CallbackInfo& info) : Napi::ObjectWrap<KeyEncapsulation>(info) {
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
        oqsKE = std::make_unique<oqs::KeyEncapsulation>(algorithm, secretKeyVec);
      } catch (const std::exception& ex) {
        throw Napi::TypeError::New(env, ex.what());
      }
    } else {
      try {
        oqsKE = std::make_unique<oqs::KeyEncapsulation>(algorithm);
      } catch (const std::exception& ex) {
        throw Napi::TypeError::New(env, ex.what());
      }
    }
  }

  /**
   * Gets the details for the KEM algorithm that the instance was constructed with.
   * @memberof KeyEncapsulation
   * @instance
   * @method
   * @name getDetails
   * @returns {Object} - An object containing the details of the KEM algorithm.
   */
  Napi::Value KeyEncapsulation::getDetails(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const oqs::KeyEncapsulation::KeyEncapsulationDetails details = oqsKE->get_details();
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
      Napi::String::New(env, "isINDCCA"),
      Napi::Boolean::New(env, details.is_ind_cca)
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
      Napi::String::New(env, "ciphertextLength"),
      Napi::Number::New(env, details.length_ciphertext)
    );
    detailsObj.Set(
      Napi::String::New(env, "sharedSecretLength"),
      Napi::Number::New(env, details.length_shared_secret)
    );
    return detailsObj;
  }

  /**
   * Generates a keypair. Overwrites any existing secret key on the instance with the generated secret key.
   * @memberof KeyEncapsulation
   * @instance
   * @method
   * @name generateKeypair
   * @returns {Buffer} - A Buffer containing the public key.
   * @throws {Error} Will throw an error if memory cannot be allocated..
   */
  Napi::Value KeyEncapsulation::generateKeypair(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
      const bytes publicKeyVec = oqsKE->generate_keypair();
      bytes* publicKeyVecCopy = new (std::nothrow) bytes(publicKeyVec);
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
   * @memberof KeyEncapsulation
   * @instance
   * @method
   * @name exportSecretKey
   * @returns {Buffer} - A Buffer containing the secret key.
   * @throws {Error} Will throw an error if memory cannot be allocated.
   */
  Napi::Value KeyEncapsulation::exportSecretKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bytes secretKeyVec = oqsKE->export_secret_key();
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
   * An object with the following properties:
   * * `ciphertext`: The ciphertext to be given to the owner of the public key.
   * * `sharedSecret`: The shared secret.
   * @memberof KeyEncapsulation
   * @typedef {Object} CiphertextSharedSecretPair
   */

  /**
   * Encapsulates the shared secret using a provided public key.
   * @memberof KeyEncapsulation
   * @instance
   * @method
   * @name encapsulateSecret
   * @param {Buffer} publicKey - The public key belonging to the intended recipient of the shared secret.
   * @returns {KeyEncapsulation.CiphertextSharedSecretPair} - The ciphertext and shared secret.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   * @throws {Error} Will throw an error if memory cannot be allocated.
   */
  Napi::Value KeyEncapsulation::encapsulateSecret(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::TypeError::New(env, "Public key must be a buffer");
    }
    if (!info[0].IsBuffer()) {
      throw Napi::TypeError::New(env, "Public key must be a buffer");
    }
    const auto publicKeyBuffer = info[0].As<Napi::Buffer<byte>>();
    const auto publicKeyData = publicKeyBuffer.Data();
    const bytes publicKeyVec(publicKeyData, publicKeyData + publicKeyBuffer.Length());
    try {
      std::pair<bytes, bytes> encapPair = oqsKE->encap_secret(publicKeyVec);
      bytes* ciphertextVec = new (std::nothrow) bytes(encapPair.first);
      bytes* sharedSecretVec = new (std::nothrow) bytes(encapPair.second);
      // Secure free shared secret returned by OQS
      oqs::mem_cleanse(encapPair.second);
      if (ciphertextVec == nullptr || sharedSecretVec == nullptr) {
        throw Napi::Error::New(env, "Failed to allocate memory");
      }
      Napi::MemoryManagement::AdjustExternalMemory(env, ciphertextVec->size());
      Napi::MemoryManagement::AdjustExternalMemory(env, sharedSecretVec->size());
      auto ciphertextSharedSecretPair = Napi::Object::New(env);
      ciphertextSharedSecretPair.Set(
        Napi::String::New(env, "ciphertext"),
        Napi::Buffer<byte>::New(
          env,
          ciphertextVec->data(),
          ciphertextVec->size(),
          [](Napi::Env cbEnv, byte* /* unused */, bytes* vec) -> void {
            if (vec != nullptr) {
              Napi::MemoryManagement::AdjustExternalMemory(cbEnv, -vec->size());
              // No need to secure free ciphertext
            }
            delete vec;
          },
          ciphertextVec
        )
      );
      ciphertextSharedSecretPair.Set(
        Napi::String::New(env, "sharedSecret"),
        Napi::Buffer<byte>::New(
          env,
          sharedSecretVec->data(),
          sharedSecretVec->size(),
          [](Napi::Env cbEnv, byte* /* unused */, bytes* vec) -> void {
            if (vec != nullptr) {
              Napi::MemoryManagement::AdjustExternalMemory(cbEnv, -vec->size());
              oqs::mem_cleanse(*vec);
            }
            delete vec;
          },
          sharedSecretVec
        )
      );
      return ciphertextSharedSecretPair;
    } catch (const std::exception& ex) {
      throw Napi::TypeError::New(env, ex.what());
    }
  }

  /**
   * Decapsulates the shared secret using a provided public key.
   * @memberof KeyEncapsulation
   * @instance
   * @method
   * @name decapsulateSecret
   * @param {Buffer} ciphertext - The ciphertext that was encrypted using the instance's public key.
   * @returns {Buffer} - The shared secret.
   * @throws {TypeError} Will throw an error if any argument is invalid.
   * @throws {Error} Will throw an error if memory cannot be allocated.
   */
  Napi::Value KeyEncapsulation::decapsulateSecret(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::TypeError::New(env, "Ciphertext must be a buffer");
    }
    if (!info[0].IsBuffer()) {
      throw Napi::TypeError::New(env, "Ciphertext must be a buffer");
    }
    const auto ciphertextBuffer = info[0].As<Napi::Buffer<byte>>();
    const auto ciphertextData = ciphertextBuffer.Data();
    const bytes ciphertextVec(ciphertextData, ciphertextData + ciphertextBuffer.Length());
    try {
      bytes sharedSecretVec = oqsKE->decap_secret(ciphertextVec);
      bytes* sharedSecretVecCopy = new (std::nothrow) bytes(sharedSecretVec);
      // Secure free shared secret returned by OQS
      oqs::mem_cleanse(sharedSecretVec);
      if (sharedSecretVecCopy == nullptr) {
        throw Napi::Error::New(env, "Failed to allocate memory");
      }
      Napi::MemoryManagement::AdjustExternalMemory(env, sharedSecretVecCopy->size());
      return Napi::Buffer<byte>::New(
        env,
        sharedSecretVecCopy->data(),
        sharedSecretVecCopy->size(),
        [](Napi::Env cbEnv, byte* /* unused */, bytes* vec) -> void {
          if (vec != nullptr) {
            Napi::MemoryManagement::AdjustExternalMemory(cbEnv, -vec->size());
            oqs::mem_cleanse(*vec);
          }
          delete vec;
        },
        sharedSecretVecCopy
      );
    } catch (const std::exception& ex) {
      throw Napi::TypeError::New(env, ex.what());
    }
  }

  void KeyEncapsulation::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "KeyEncapsulation", {
      InstanceMethod<&KeyEncapsulation::getDetails>("getDetails"),
      InstanceMethod<&KeyEncapsulation::generateKeypair>("generateKeypair"),
      InstanceMethod<&KeyEncapsulation::exportSecretKey>("exportSecretKey"),
      InstanceMethod<&KeyEncapsulation::encapsulateSecret>("encapsulateSecret"),
      InstanceMethod<&KeyEncapsulation::decapsulateSecret>("decapsulateSecret")
    });
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set(
      Napi::String::New(env, "KeyEncapsulation"),
      func
    );
    env.SetInstanceData<Napi::FunctionReference>(constructor);
  }

  void Init(Napi::Env env, Napi::Object exports) {
    KeyEncapsulation::Init(env, exports);
  }

} // namespace KeyEncapsulation

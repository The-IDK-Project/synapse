#pragma once

#include "../../core/matrix_types.hpp"
#include <vector>
#include <array>
#include <cstdint>

namespace matrix::crypto {

enum class KeyAlgorithm {
    ED25519,
    CURVE25519,
    RSA_2048,
    RSA_4096,
    AES_256,
    HMAC_SHA256
};

enum class HashAlgorithm {
    SHA256,
    SHA512,
    SHA3_256,
    SHA3_512,
    BLAKE2B,
    BLAKE2S
};

enum class SignatureAlgorithm {
    ED25519,
    RSA_PSS_SHA256,
    RSA_PSS_SHA512,
    ECDSA_SECP256K1
};

enum class EncryptionMode {
    CBC,
    GCM,
    CTR,
    OFB
};

struct KeyPair {
    std::vector<uint8_t> public_key;
    std::vector<uint8_t> private_key;
    KeyAlgorithm algorithm;
    std::string key_id;
    Timestamp created_ts;
    Timestamp expires_ts;
};

struct Signature {
    std::vector<uint8_t> data;
    SignatureAlgorithm algorithm;
    std::string key_id;
    Timestamp timestamp;
};

struct EncryptedData {
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> iv;
    std::vector<uint8_t> auth_tag;
    EncryptionMode mode;
    KeyAlgorithm algorithm;
    std::string key_id;
};

struct HashResult {
    std::vector<uint8_t> digest;
    HashAlgorithm algorithm;
    size_t bit_length;
};

struct CryptographicContext {
    std::string context_id;
    std::vector<uint8_t> associated_data;
    std::vector<uint8_t> nonce;
    Timestamp created_ts;
    int usage_count;
};

struct KeyMetadata {
    std::string key_id;
    KeyAlgorithm algorithm;
    std::string purpose;
    Timestamp created_ts;
    Timestamp expires_ts;
    bool enabled;
    bool compromised;
    std::vector<std::string> allowed_operations;
    std::string owner;
    int usage_count;
};

struct KeyRotationPolicy {
    int rotation_interval_days;
    int max_usage_count;
    bool auto_rotate;
    bool archive_old_keys;
    int archive_retention_days;
};

struct CryptoConfig {
    HashAlgorithm default_hash_algorithm = HashAlgorithm::SHA256;
    SignatureAlgorithm default_signature_algorithm = SignatureAlgorithm::ED25519;
    EncryptionMode default_encryption_mode = EncryptionMode::GCM;
    int key_size_bits = 256;
    int iv_size_bytes = 16;
    int auth_tag_size_bytes = 16;
    int salt_size_bytes = 32;
    int key_derivation_iterations = 100000;
    bool enable_secure_erasure = true;
    bool enable_side_channel_protection = false;
};

class CryptoError : public std::exception {
public:
    CryptoError(const std::string& message, int error_code = 0)
        : message_(message), error_code_(error_code) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    int error_code() const { return error_code_; }

private:
    std::string message_;
    int error_code_;
};

template<typename T>
class CryptoResult {
public:
    CryptoResult(const T& value) : value_(value), error_() {}
    CryptoResult(const CryptoError& error) : value_(), error_(error) {}

    bool success() const { return !error_.what(); }
    const T& value() const { return value_; }
    const CryptoError& error() const { return error_; }

    explicit operator bool() const { return success(); }

private:
    T value_;
    CryptoError error_;
};

}
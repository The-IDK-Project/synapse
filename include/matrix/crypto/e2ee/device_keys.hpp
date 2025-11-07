#pragma once

#include "../common/crypto_types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace matrix::crypto::e2ee {

struct DeviceKeys {
    std::string user_id;
    std::string device_id;
    std::vector<std::string> algorithms;
    std::unordered_map<std::string, std::string> keys;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> signatures;
    Timestamp created_ts;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct OneTimeKey {
    std::string key_id;
    std::string algorithm;
    std::string key;
    std::unordered_map<std::string, std::string> signatures;
    Timestamp created_ts;
    bool used;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct KeyQueryResponse {
    std::unordered_map<std::string, std::unordered_map<std::string, DeviceKeys>> device_keys;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, OneTimeKey>>> one_time_keys;
    std::string next_batch_token;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct KeyClaimResponse {
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, OneTimeKey>>> one_time_keys;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct CrossSigningKeys {
    std::string user_id;
    std::unordered_map<std::string, std::string> master_key;
    std::unordered_map<std::string, std::string> self_signing_key;
    std::unordered_map<std::string, std::string> user_signing_key;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> signatures;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

class DeviceKeyManager {
public:
    DeviceKeyManager();
    ~DeviceKeyManager();

    bool initialize(const std::string& user_id, const std::string& device_id);
    bool shutdown();

    CryptoResult<DeviceKeys> generate_device_keys();
    CryptoResult<bool> sign_device_keys(DeviceKeys& device_keys);
    CryptoResult<bool> verify_device_keys(const DeviceKeys& device_keys);

    CryptoResult<std::vector<OneTimeKey>> generate_one_time_keys(int count = 50);
    CryptoResult<bool> sign_one_time_keys(std::vector<OneTimeKey>& one_time_keys);
    CryptoResult<bool> verify_one_time_key(const OneTimeKey& one_time_key);

    CryptoResult<CrossSigningKeys> generate_cross_signing_keys();
    CryptoResult<bool> sign_cross_signing_keys(CrossSigningKeys& cross_signing_keys);
    CryptoResult<bool> verify_cross_signing_keys(const CrossSigningKeys& cross_signing_keys);

    CryptoResult<bool> upload_keys(const DeviceKeys& device_keys, const std::vector<OneTimeKey>& one_time_keys = {});
    CryptoResult<KeyQueryResponse> query_keys(const std::vector<std::string>& user_ids);
    CryptoResult<KeyClaimResponse> claim_keys(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& one_time_keys);

    CryptoResult<bool> mark_one_time_key_used(const std::string& key_id);
    CryptoResult<int> get_available_one_time_keys_count();

    CryptoResult<bool> verify_device_signature(const std::string& user_id,
                                              const std::string& device_id,
                                              const std::vector<uint8_t>& message,
                                              const std::vector<uint8_t>& signature);

    CryptoResult<bool> trust_device(const std::string& user_id, const std::string& device_id);
    CryptoResult<bool> untrust_device(const std::string& user_id, const std::string& device_id);
    CryptoResult<bool> is_device_trusted(const std::string& user_id, const std::string& device_id);

    std::vector<std::string> get_trusted_devices(const std::string& user_id);
    std::vector<std::string> get_untrusted_devices(const std::string& user_id);

    struct DeviceKeyStats {
        size_t total_devices;
        size_t trusted_devices;
        size_t one_time_keys_available;
        size_t key_queries_performed;
        size_t key_claims_performed;
    };

    DeviceKeyStats get_stats() const;

private:
    std::string user_id_;
    std::string device_id_;
    bool initialized_ = false;

    std::unordered_map<std::string, DeviceKeys> device_keys_cache_;
    std::unordered_map<std::string, std::vector<OneTimeKey>> one_time_keys_cache_;
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> device_trust_status_;

    mutable std::shared_mutex cache_mutex_;

    DeviceKeyStats stats_;

    CryptoResult<std::string> generate_key_id(const std::string& algorithm) const;
    CryptoResult<bool> validate_device_keys_structure(const DeviceKeys& device_keys) const;
    CryptoResult<bool> validate_one_time_key_structure(const OneTimeKey& one_time_key) const;

    CryptoResult<std::vector<uint8_t>> calculate_device_keys_fingerprint(const DeviceKeys& device_keys) const;
    CryptoResult<std::vector<uint8_t>> calculate_one_time_key_fingerprint(const OneTimeKey& one_time_key) const;

    void update_trust_status(const std::string& user_id, const std::string& device_id, bool trusted);
    void cleanup_expired_one_time_keys();
};

}
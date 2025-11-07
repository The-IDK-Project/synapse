#pragma once

#include "crypto_types.hpp"
#include <memory>
#include <unordered_map>
#include <shared_mutex>

namespace matrix::crypto {

class KeyManager {
public:
    KeyManager();
    ~KeyManager();

    bool initialize(const CryptoConfig& config = CryptoConfig());
    bool shutdown();

    CryptoResult<KeyPair> generate_key_pair(KeyAlgorithm algorithm, const std::string& key_id = "");
    CryptoResult<KeyPair> import_key_pair(const std::vector<uint8_t>& public_key,
                                         const std::vector<uint8_t>& private_key,
                                         KeyAlgorithm algorithm,
                                         const std::string& key_id = "");

    CryptoResult<std::vector<uint8_t>> derive_key(const std::string& password,
                                                 const std::vector<uint8_t>& salt,
                                                 KeyAlgorithm algorithm,
                                                 size_t key_size_bits);

    CryptoResult<Signature> sign_data(const std::vector<uint8_t>& data,
                                     const std::string& key_id,
                                     SignatureAlgorithm algorithm = SignatureAlgorithm::ED25519);

    CryptoResult<bool> verify_signature(const std::vector<uint8_t>& data,
                                       const Signature& signature,
                                       const std::vector<uint8_t>& public_key);

    CryptoResult<EncryptedData> encrypt_data(const std::vector<uint8_t>& data,
                                            const std::string& key_id,
                                            EncryptionMode mode = EncryptionMode::GCM);

    CryptoResult<std::vector<uint8_t>> decrypt_data(const EncryptedData& encrypted_data,
                                                   const std::string& key_id);

    CryptoResult<std::vector<uint8_t>> export_public_key(const std::string& key_id);
    CryptoResult<std::vector<uint8_t>> export_private_key(const std::string& key_id);

    bool delete_key(const std::string& key_id);
    bool disable_key(const std::string& key_id);
    bool enable_key(const std::string& key_id);

    bool set_key_metadata(const std::string& key_id, const KeyMetadata& metadata);
    CryptoResult<KeyMetadata> get_key_metadata(const std::string& key_id);

    bool set_rotation_policy(const std::string& key_type, const KeyRotationPolicy& policy);
    CryptoResult<KeyRotationPolicy> get_rotation_policy(const std::string& key_type);

    bool rotate_key(const std::string& key_id);
    bool auto_rotate_keys();

    std::vector<std::string> list_keys(KeyAlgorithm algorithm = KeyAlgorithm::ED25519);
    std::vector<std::string> list_keys_by_purpose(const std::string& purpose);

    bool key_exists(const std::string& key_id) const;
    bool is_key_enabled(const std::string& key_id) const;
    bool is_key_compromised(const std::string& key_id) const;

    CryptoResult<std::string> get_default_key_id(KeyAlgorithm algorithm) const;
    bool set_default_key_id(const std::string& key_id, KeyAlgorithm algorithm);

    bool backup_keys(const std::string& backup_path, const std::string& encryption_key = "");
    bool restore_keys(const std::string& backup_path, const std::string& decryption_key = "");

    struct KeyManagerStats {
        size_t total_keys;
        size_t enabled_keys;
        size_t compromised_keys;
        size_t key_generations;
        size_t key_rotations;
        size_t failed_operations;
    };

    KeyManagerStats get_stats() const;
    nlohmann::json get_detailed_stats() const;

private:
    struct KeyStorage {
        KeyPair key_pair;
        KeyMetadata metadata;
        std::vector<uint8_t> encrypted_private_key;
        Timestamp last_used;
        int usage_count;
    };

    CryptoConfig config_;
    std::unordered_map<std::string, KeyStorage> keys_;
    std::unordered_map<KeyAlgorithm, std::string> default_keys_;
    std::unordered_map<std::string, KeyRotationPolicy> rotation_policies_;

    mutable std::shared_mutex keys_mutex_;

    KeyManagerStats stats_;

    std::string generate_key_id(KeyAlgorithm algorithm) const;
    bool validate_key_algorithm(KeyAlgorithm algorithm) const;
    bool validate_key_size(size_t key_size_bits, KeyAlgorithm algorithm) const;

    CryptoResult<std::vector<uint8_t>> encrypt_private_key(const std::vector<uint8_t>& private_key);
    CryptoResult<std::vector<uint8_t>> decrypt_private_key(const std::vector<uint8_t>& encrypted_private_key);

    bool store_key_in_memory(const std::string& key_id, const KeyStorage& storage);
    bool remove_key_from_memory(const std::string& key_id);
    CryptoResult<KeyStorage> load_key_from_memory(const std::string& key_id);

    bool update_key_usage(const std::string& key_id);
    bool check_key_expiry(const std::string& key_id);
    bool archive_key(const std::string& key_id);

    void cleanup_expired_keys();
    void cleanup_archived_keys();

    std::vector<uint8_t> generate_secure_random(size_t size) const;
    bool secure_erase(std::vector<uint8_t>& data) const;
};

class KeyStore {
public:
    KeyStore();
    ~KeyStore();

    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;

    virtual bool store_key(const std::string& key_id, const KeyPair& key_pair, const KeyMetadata& metadata) = 0;
    virtual CryptoResult<KeyPair> load_key(const std::string& key_id) = 0;
    virtual CryptoResult<KeyMetadata> load_key_metadata(const std::string& key_id) = 0;
    virtual bool delete_key(const std::string& key_id) = 0;
    virtual bool key_exists(const std::string& key_id) = 0;

    virtual std::vector<std::string> list_keys() = 0;
    virtual std::vector<std::string> list_keys_by_algorithm(KeyAlgorithm algorithm) = 0;

    virtual bool backup(const std::string& backup_path) = 0;
    virtual bool restore(const std::string& backup_path) = 0;

protected:
    virtual std::vector<uint8_t> encrypt_key_data(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> decrypt_key_data(const std::vector<uint8_t>& data) = 0;
};

class MemoryKeyStore : public KeyStore {
public:
    MemoryKeyStore();
    ~MemoryKeyStore();

    bool initialize() override;
    bool shutdown() override;

    bool store_key(const std::string& key_id, const KeyPair& key_pair, const KeyMetadata& metadata) override;
    CryptoResult<KeyPair> load_key(const std::string& key_id) override;
    CryptoResult<KeyMetadata> load_key_metadata(const std::string& key_id) override;
    bool delete_key(const std::string& key_id) override;
    bool key_exists(const std::string& key_id) override;

    std::vector<std::string> list_keys() override;
    std::vector<std::string> list_keys_by_algorithm(KeyAlgorithm algorithm) override;

    bool backup(const std::string& backup_path) override;
    bool restore(const std::string& backup_path) override;

protected:
    std::vector<uint8_t> encrypt_key_data(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decrypt_key_data(const std::vector<uint8_t>& data) override;

private:
    std::unordered_map<std::string, std::pair<KeyPair, KeyMetadata>> keys_;
    mutable std::shared_mutex keys_mutex_;
    std::vector<uint8_t> encryption_key_;
};

class FileKeyStore : public KeyStore {
public:
    FileKeyStore(const std::string& base_path);
    ~FileKeyStore();

    bool initialize() override;
    bool shutdown() override;

    bool store_key(const std::string& key_id, const KeyPair& key_pair, const KeyMetadata& metadata) override;
    CryptoResult<KeyPair> load_key(const std::string& key_id) override;
    CryptoResult<KeyMetadata> load_key_metadata(const std::string& key_id) override;
    bool delete_key(const std::string& key_id) override;
    bool key_exists(const std::string& key_id) override;

    std::vector<std::string> list_keys() override;
    std::vector<std::string> list_keys_by_algorithm(KeyAlgorithm algorithm) override;

    bool backup(const std::string& backup_path) override;
    bool restore(const std::string& backup_path) override;

protected:
    std::vector<uint8_t> encrypt_key_data(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decrypt_key_data(const std::vector<uint8_t>& data) override;

private:
    std::string base_path_;
    std::vector<uint8_t> encryption_key_;

    std::string get_key_path(const std::string& key_id) const;
    std::string get_metadata_path(const std::string& key_id) const;
    bool ensure_directory_exists(const std::string& path) const;
};

}
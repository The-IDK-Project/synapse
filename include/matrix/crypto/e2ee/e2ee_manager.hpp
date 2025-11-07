#pragma once

#include "../common/crypto_types.hpp"
#include "olm_session.hpp"
#include "megolm_session.hpp"
#include "device_keys.hpp"
#include "key_verification.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::crypto::e2ee {

class E2EEManager {
public:
    E2EEManager();
    ~E2EEManager();

    bool initialize(const std::string& user_id, const std::string& device_id);
    bool shutdown();

    // Device keys management
    CryptoResult<DeviceKeys> generate_device_keys();
    CryptoResult<bool> upload_device_keys(const DeviceKeys& device_keys);
    CryptoResult<bool> upload_one_time_keys(int count = 50);
    CryptoResult<std::unordered_map<std::string, int>> get_one_time_keys_count();

    // Key query and claim
    CryptoResult<KeyQueryResponse> query_keys(const std::vector<std::string>& user_ids);
    CryptoResult<KeyClaimResponse> claim_keys(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& one_time_keys);

    // Olm sessions
    CryptoResult<std::string> create_outbound_olm_session(const std::string& user_id, const std::string& device_id, const std::vector<uint8_t>& identity_key);
    CryptoResult<std::string> create_inbound_olm_session(const std::vector<uint8_t>& message, const std::vector<uint8_t>& identity_key);
    CryptoResult<OlmSession> get_olm_session(const std::string& session_id);
    CryptoResult<bool> save_olm_session(const std::string& session_id, const OlmSession& session);

    // Megolm sessions
    CryptoResult<std::string> create_outbound_megolm_session(const std::string& room_id);
    CryptoResult<std::string> create_inbound_megolm_session(const std::string& room_id, const std::vector<uint8_t>& session_key);
    CryptoResult<MegolmSession> get_megolm_session(const std::string& session_id);
    CryptoResult<bool> share_megolm_session(const std::string& session_id, const std::vector<std::string>& user_ids);

    // Encryption
    CryptoResult<EncryptedData> encrypt_olm_message(const std::string& session_id, const std::vector<uint8_t>& plaintext);
    CryptoResult<EncryptedData> encrypt_megolm_message(const std::string& session_id, const std::vector<uint8_t>& plaintext);
    CryptoResult<std::vector<uint8_t>> decrypt_olm_message(const std::string& session_id, const EncryptedData& encrypted_data);
    CryptoResult<std::vector<uint8_t>> decrypt_megolm_message(const std::string& session_id, const EncryptedData& encrypted_data);

    // Key verification
    CryptoResult<std::string> start_key_verification(const std::string& user_id, const std::string& device_id);
    CryptoResult<bool> accept_key_verification(const std::string& verification_id);
    CryptoResult<bool> cancel_key_verification(const std::string& verification_id, const std::string& reason);
    CryptoResult<bool> perform_key_verification(const std::string& verification_id, const std::vector<uint8_t>& secret);

    // Cross-signing
    CryptoResult<CrossSigningKeys> generate_cross_signing_keys();
    CryptoResult<bool> upload_cross_signing_keys(const CrossSigningKeys& keys);
    CryptoResult<bool> sign_user_keys(const std::string& target_user_id, const CrossSigningKeys& target_keys);
    CryptoResult<bool> verify_user_keys(const std::string& user_id);

    // Key backup
    CryptoResult<std::string> create_key_backup_version(const std::vector<uint8_t>& recovery_key);
    CryptoResult<bool> update_key_backup(const std::string& version, const std::vector<uint8_t>& recovery_key);
    CryptoResult<bool> backup_room_keys(const std::string& version, const std::vector<RoomKeyBackup>& room_keys);
    CryptoResult<std::vector<RoomKeyBackup>> restore_room_keys(const std::string& version, const std::vector<uint8_t>& recovery_key);

    // Session management
    CryptoResult<bool> rotate_megolm_session(const std::string& session_id);
    CryptoResult<bool> invalidate_olm_session(const std::string& session_id);
    CryptoResult<bool> cleanup_expired_sessions();

    // Status and info
    bool is_initialized() const;
    std::string get_user_id() const;
    std::string get_device_id() const;
    CryptoResult<E2EEStatus> get_status() const;

    struct E2EEStats {
        size_t olm_sessions_count;
        size_t megolm_sessions_count;
        size_t one_time_keys_count;
        size_t verified_devices_count;
        size_t encrypted_messages_count;
        size_t decrypted_messages_count;
    };

    E2EEStats get_stats() const;

private:
    std::string user_id_;
    std::string device_id_;
    bool initialized_ = false;

    std::unique_ptr<KeyManager> key_manager_;
    std::unique_ptr<DeviceKeyManager> device_key_manager_;
    std::unique_ptr<KeyVerificationManager> verification_manager_;

    std::unordered_map<std::string, OlmSession> olm_sessions_;
    std::unordered_map<std::string, MegolmSession> megolm_sessions_;
    std::unordered_map<std::string, KeyVerificationSession> verification_sessions_;

    mutable std::shared_mutex sessions_mutex_;

    E2EEStats stats_;

    CryptoResult<std::vector<uint8_t>> generate_olm_session_key();
    CryptoResult<std::vector<uint8_t>> generate_megolm_session_key();
    CryptoResult<bool> validate_device_keys(const DeviceKeys& device_keys);
    CryptoResult<bool> verify_device_signature(const DeviceKeys& device_keys);

    void update_session_stats(bool encrypted, bool decrypted);
    void cleanup_old_sessions();
};

class E2EEProvider {
public:
    E2EEProvider();
    ~E2EEProvider();

    static std::unique_ptr<E2EEManager> create_manager(const std::string& user_id, const std::string& device_id);
    static bool is_e2ee_supported();
    static std::vector<std::string> get_supported_algorithms();

    struct E2EECapabilities {
        bool olm_support;
        bool megolm_support;
        bool cross_signing_support;
        bool key_backup_support;
        bool key_verification_support;
        std::vector<std::string> supported_algorithms;
    };

    static E2EECapabilities get_capabilities();

private:
    static bool check_olm_library();
    static bool check_megolm_library();
};

}
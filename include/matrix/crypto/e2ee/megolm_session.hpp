#pragma once

#include "../common/crypto_types.hpp"
#include <vector>
#include <memory>

namespace matrix::crypto::e2ee {

struct MegolmSession {
    std::string session_id;
    std::string room_id;
    std::vector<uint8_t> session_key;
    std::vector<uint8_t> signing_key;
    uint32_t message_index;
    uint32_t max_messages;
    Timestamp created_ts;
    Timestamp last_used_ts;
    Timestamp expiration_ts;
    bool is_shared;
    bool is_verified;
    std::vector<std::string> shared_with_users;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct RoomKeyBackup {
    std::string room_id;
    std::string session_id;
    std::vector<uint8_t> session_key;
    uint32_t first_message_index;
    uint32_t forwarded_count;
    bool is_verified;
    Timestamp backed_up_ts;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

class MegolmSessionManager {
public:
    MegolmSessionManager();
    ~MegolmSessionManager();

    CryptoResult<std::string> create_outbound_session(const std::string& room_id,
                                                     uint32_t max_messages = 1000,
                                                     uint32_t max_age_ms = 604800000); // 7 days

    CryptoResult<std::string> create_inbound_session(const std::string& room_id,
                                                    const std::vector<uint8_t>& session_key);

    CryptoResult<MegolmSession> get_session(const std::string& session_id);
    CryptoResult<bool> save_session(const std::string& session_id, const MegolmSession& session);
    CryptoResult<bool> delete_session(const std::string& session_id);

    CryptoResult<EncryptedData> encrypt_message(const std::string& session_id,
                                               const std::vector<uint8_t>& plaintext);

    CryptoResult<std::vector<uint8_t>> decrypt_message(const std::string& session_id,
                                                      const EncryptedData& encrypted_data);

    CryptoResult<bool> share_session(const std::string& session_id,
                                    const std::vector<std::string>& user_ids);

    CryptoResult<bool> import_session(const std::string& room_id,
                                     const std::vector<uint8_t>& session_key,
                                     uint32_t message_index = 0);

    CryptoResult<bool> rotate_session(const std::string& session_id);
    CryptoResult<bool> invalidate_session(const std::string& session_id);

    CryptoResult<std::vector<RoomKeyBackup>> export_sessions_for_backup();
    CryptoResult<bool> import_sessions_from_backup(const std::vector<RoomKeyBackup>& backups);

    std::vector<std::string> get_sessions_for_room(const std::string& room_id);
    std::vector<std::string> get_sessions_shared_with_user(const std::string& user_id);

    bool session_exists(const std::string& session_id) const;
    bool is_session_expired(const std::string& session_id) const;
    bool is_session_usable(const std::string& session_id) const;

    struct MegolmSessionStats {
        size_t total_sessions;
        size_t active_sessions;
        size_t expired_sessions;
        size_t shared_sessions;
        size_t messages_encrypted;
        size_t messages_decrypted;
    };

    MegolmSessionStats get_stats() const;

private:
    std::unordered_map<std::string, MegolmSession> sessions_;
    mutable std::shared_mutex sessions_mutex_;

    MegolmSessionStats stats_;

    std::string generate_session_id() const;
    CryptoResult<std::vector<uint8_t>> generate_session_key() const;
    CryptoResult<std::vector<uint8_t>> generate_signing_key() const;

    CryptoResult<std::vector<uint8_t>> perform_megolm_encryption(const MegolmSession& session,
                                                                const std::vector<uint8_t>& plaintext);
    CryptoResult<std::vector<uint8_t>> perform_megolm_decryption(const MegolmSession& session,
                                                                const EncryptedData& encrypted_data);

    bool validate_session_key(const std::vector<uint8_t>& session_key) const;
    bool validate_signing_key(const std::vector<uint8_t>& signing_key) const;

    void cleanup_expired_sessions();
};

class MegolmKeyDerivation {
public:
    MegolmKeyDerivation();
    ~MegolmKeyDerivation();

    CryptoResult<std::vector<uint8_t>> derive_message_key(const std::vector<uint8_t>& session_key,
                                                         uint32_t message_index);

    CryptoResult<std::vector<uint8_t>> derive_ratchet_key(const std::vector<uint8_t>& session_key,
                                                         uint32_t ratchet_index);

    CryptoResult<std::vector<uint8_t>> derive_signing_key(const std::vector<uint8_t>& session_key);

    CryptoResult<std::vector<uint8_t>> perform_ratchet(const std::vector<uint8_t>& current_key);

    CryptoResult<bool> verify_message_signature(const std::vector<uint8_t>& message,
                                               const std::vector<uint8_t>& signature,
                                               const std::vector<uint8_t>& signing_key);

    static size_t get_megolm_message_overhead();
    static size_t get_max_megolm_message_size();
    static uint32_t get_default_max_messages();
    static uint32_t get_default_max_age_ms();

private:
    CryptoResult<std::vector<uint8_t>> hkdf_derive(const std::vector<uint8_t>& input_key,
                                                  const std::vector<uint8_t>& salt,
                                                  const std::vector<uint8_t>& info,
                                                  size_t output_length);
};

}
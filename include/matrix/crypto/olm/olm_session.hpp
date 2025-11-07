#pragma once

#include "../common/crypto_types.hpp"
#include "olm_wrapper.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::crypto::olm {

class OlmSessionManager {
public:
    OlmSessionManager();
    ~OlmSessionManager();

    bool initialize();
    bool shutdown();

    CryptoResult<std::string> create_outbound_session(const std::string& session_id,
                                                     OlmAccount& account,
                                                     const std::vector<uint8_t>& their_identity_key,
                                                     const std::vector<uint8_t>& their_one_time_key);

    CryptoResult<std::string> create_inbound_session(const std::string& session_id,
                                                    OlmAccount& account,
                                                    const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<std::string> create_inbound_session_from(const std::string& session_id,
                                                         OlmAccount& account,
                                                         const std::vector<uint8_t>& their_identity_key,
                                                         const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<std::shared_ptr<OlmSession>> get_session(const std::string& session_id);
    CryptoResult<bool> save_session(const std::string& session_id, std::shared_ptr<OlmSession> session);
    CryptoResult<bool> delete_session(const std::string& session_id);

    CryptoResult<EncryptedData> encrypt_message(const std::string& session_id,
                                               const std::vector<uint8_t>& plaintext);

    CryptoResult<std::vector<uint8_t>> decrypt_message(const std::string& session_id,
                                                      const EncryptedData& encrypted_data);

    CryptoResult<std::string> find_matching_session(OlmAccount& account,
                                                   const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<std::string> find_matching_session_from(OlmAccount& account,
                                                        const std::vector<uint8_t>& their_identity_key,
                                                        const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<bool> session_exists(const std::string& session_id) const;
    CryptoResult<bool> session_has_received_message(const std::string& session_id);
    CryptoResult<std::vector<uint8_t>> get_session_description(const std::string& session_id);

    std::vector<std::string> get_all_sessions() const;
    std::vector<std::string> get_sessions_for_account(const std::vector<uint8_t>& account_id);

    CryptoResult<bool> save_sessions_to_file(const std::string& file_path);
    CryptoResult<bool> load_sessions_from_file(const std::string& file_path);

    CryptoResult<nlohmann::json> export_sessions();
    CryptoResult<bool> import_sessions(const nlohmann::json& sessions_data);

    struct SessionStats {
        size_t total_sessions;
        size_t outbound_sessions;
        size_t inbound_sessions;
        size_t active_sessions;
        size_t messages_encrypted;
        size_t messages_decrypted;
        size_t session_matches;
    };

    SessionStats get_stats() const;

private:
    std::unordered_map<std::string, std::shared_ptr<OlmSession>> sessions_;
    std::unordered_map<std::vector<uint8_t>, std::vector<std::string>> account_sessions_;

    mutable std::shared_mutex sessions_mutex_;

    SessionStats stats_;

    std::string generate_session_id() const;
    CryptoResult<bool> validate_session_parameters(const std::vector<uint8_t>& their_identity_key,
                                                  const std::vector<uint8_t>& their_one_time_key) const;

    void update_session_stats(bool encrypted, bool decrypted, bool matched);
    void cleanup_inactive_sessions();
};

class OlmMessage {
public:
    enum class Type {
        PRE_KEY = 0,
        MESSAGE = 1
    };

    struct EncryptedMessage {
        Type type;
        std::vector<uint8_t> ciphertext;
        std::vector<uint8_t> mac;
        std::vector<uint8_t> ephemeral_key;

        nlohmann::json to_json() const;
        void from_json(const nlohmann::json& j);
    };

    OlmMessage();
    ~OlmMessage();

    static CryptoResult<EncryptedMessage> create_pre_key_message(OlmSession& session,
                                                                const std::vector<uint8_t>& plaintext);

    static CryptoResult<EncryptedMessage> create_message(OlmSession& session,
                                                        const std::vector<uint8_t>& plaintext);

    static CryptoResult<std::vector<uint8_t>> decrypt_message(OlmSession& session,
                                                             const EncryptedMessage& encrypted_message);

    static size_t get_message_overhead(Type type);
    static size_t get_max_message_size(Type type);

private:
    static CryptoResult<EncryptedMessage> encrypt_internal(OlmSession& session,
                                                          const std::vector<uint8_t>& plaintext,
                                                          Type message_type);
};

class OlmSessionStorage {
public:
    OlmSessionStorage();
    ~OlmSessionStorage();

    CryptoResult<bool> store_session(const std::string& session_id,
                                    const std::vector<uint8_t>& session_data,
                                    const std::vector<uint8_t>& account_id);

    CryptoResult<std::vector<uint8_t>> load_session(const std::string& session_id);
    CryptoResult<bool> delete_session(const std::string& session_id);
    CryptoResult<bool> session_exists(const std::string& session_id);

    CryptoResult<std::vector<std::string>> list_sessions_for_account(const std::vector<uint8_t>& account_id);
    CryptoResult<std::vector<std::string>> list_all_sessions();

    CryptoResult<bool> cleanup_orphaned_sessions();
    CryptoResult<bool> backup_sessions(const std::string& backup_path);
    CryptoResult<bool> restore_sessions(const std::string& backup_path);

    struct StorageStats {
        size_t total_sessions;
        size_t storage_size;
        size_t backup_count;
    };

    StorageStats get_stats() const;

private:
    std::unordered_map<std::string, std::vector<uint8_t>> session_storage_;
    std::unordered_map<std::vector<uint8_t>, std::vector<std::string>> account_index_;

    mutable std::shared_mutex storage_mutex_;

    StorageStats stats_;

    std::string generate_storage_key(const std::string& session_id) const;
    CryptoResult<bool> validate_session_data(const std::vector<uint8_t>& session_data) const;
};

}
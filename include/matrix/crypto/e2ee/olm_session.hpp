#pragma once

#include "../common/crypto_types.hpp"
#include <vector>
#include <memory>

namespace matrix::crypto::e2ee {

struct OlmSession {
    std::string session_id;
    std::string user_id;
    std::string device_id;
    std::vector<uint8_t> session_key;
    std::vector<uint8_t> identity_key;
    std::vector<uint8_t> one_time_key;
    Timestamp created_ts;
    Timestamp last_used_ts;
    int message_count;
    bool is_verified;
    bool is_active;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

class OlmSessionManager {
public:
    OlmSessionManager();
    ~OlmSessionManager();

    CryptoResult<std::string> create_outbound_session(const std::string& user_id,
                                                     const std::string& device_id,
                                                     const std::vector<uint8_t>& identity_key,
                                                     const std::vector<uint8_t>& one_time_key);

    CryptoResult<std::string> create_inbound_session(const std::vector<uint8_t>& message,
                                                    const std::vector<uint8_t>& identity_key,
                                                    const std::vector<uint8_t>& one_time_key);

    CryptoResult<OlmSession> get_session(const std::string& session_id);
    CryptoResult<bool> save_session(const std::string& session_id, const OlmSession& session);
    CryptoResult<bool> delete_session(const std::string& session_id);

    CryptoResult<EncryptedData> encrypt_message(const std::string& session_id,
                                               const std::vector<uint8_t>& plaintext);

    CryptoResult<std::vector<uint8_t>> decrypt_message(const std::string& session_id,
                                                      const EncryptedData& encrypted_data);

    CryptoResult<bool> verify_session(const std::string& session_id);
    CryptoResult<bool> rotate_session(const std::string& session_id);

    std::vector<std::string> get_sessions_for_user(const std::string& user_id);
    std::vector<std::string> get_sessions_for_device(const std::string& user_id, const std::string& device_id);

    bool session_exists(const std::string& session_id) const;
    bool is_session_verified(const std::string& session_id) const;

    struct OlmSessionStats {
        size_t total_sessions;
        size_t active_sessions;
        size_t verified_sessions;
        size_t messages_encrypted;
        size_t messages_decrypted;
    };

    OlmSessionStats get_stats() const;

private:
    std::unordered_map<std::string, OlmSession> sessions_;
    mutable std::shared_mutex sessions_mutex_;

    OlmSessionStats stats_;

    std::string generate_session_id() const;
    CryptoResult<std::vector<uint8_t>> perform_olm_encryption(const OlmSession& session, const std::vector<uint8_t>& plaintext);
    CryptoResult<std::vector<uint8_t>> perform_olm_decryption(const OlmSession& session, const EncryptedData& encrypted_data);
    bool validate_session_state(const OlmSession& session) const;
};

class OlmUtility {
public:
    OlmUtility();
    ~OlmUtility();

    CryptoResult<std::vector<uint8_t>> calculate_shared_secret(const std::vector<uint8_t>& our_private_key,
                                                              const std::vector<uint8_t>& their_public_key);

    CryptoResult<std::vector<uint8_t>> derive_ratchet_key(const std::vector<uint8_t>& shared_secret,
                                                         const std::vector<uint8_t>& ratchet_info);

    CryptoResult<std::vector<uint8_t>> perform_ratchet_step(const std::vector<uint8_t>& current_key,
                                                           const std::vector<uint8_t>& ratchet_data);

    CryptoResult<bool> verify_identity_key_signature(const std::vector<uint8_t>& identity_key,
                                                    const std::vector<uint8_t>& signature,
                                                    const std::vector<uint8_t>& signing_key);

    CryptoResult<std::vector<uint8_t>> generate_olm_message_key(const std::vector<uint8_t>& ratchet_key,
                                                               uint32_t message_index);

    static size_t get_olm_message_overhead();
    static size_t get_max_olm_message_size();

private:
    CryptoResult<std::vector<uint8_t>> perform_key_derivation(const std::vector<uint8_t>& input_key,
                                                             const std::vector<uint8_t>& salt,
                                                             const std::string& context);
};

}
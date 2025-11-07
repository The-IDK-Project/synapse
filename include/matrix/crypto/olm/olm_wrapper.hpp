#pragma once

#include "../common/crypto_types.hpp"
#include <vector>
#include <memory>

extern "C" {
#include <olm/olm.h>
}

namespace matrix::crypto::olm {

class OlmWrapper {
public:
    OlmWrapper();
    ~OlmWrapper();

    bool initialize();
    bool shutdown();

    CryptoResult<size_t> create_account_size();
    CryptoResult<std::vector<uint8_t>> create_account();
    CryptoResult<std::vector<uint8_t>> generate_identity_keys(const std::vector<uint8_t>& account_buffer);
    CryptoResult<std::vector<uint8_t>> generate_one_time_keys(const std::vector<uint8_t>& account_buffer, size_t count);
    CryptoResult<std::vector<uint8_t>> get_identity_keys(const std::vector<uint8_t>& account_buffer);
    CryptoResult<std::vector<uint8_t>> get_one_time_keys(const std::vector<uint8_t>& account_buffer);

    CryptoResult<size_t> create_outbound_session_size();
    CryptoResult<std::vector<uint8_t>> create_outbound_session(const std::vector<uint8_t>& account_buffer,
                                                              const std::vector<uint8_t>& their_identity_key,
                                                              const std::vector<uint8_t>& their_one_time_key);

    CryptoResult<size_t> create_inbound_session_size();
    CryptoResult<std::vector<uint8_t>> create_inbound_session(const std::vector<uint8_t>& account_buffer,
                                                             const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<size_t> create_inbound_session_from_size();
    CryptoResult<std::vector<uint8_t>> create_inbound_session_from(const std::vector<uint8_t>& account_buffer,
                                                                  const std::vector<uint8_t>& their_identity_key,
                                                                  const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<size_t> encrypt_message_size(const std::vector<uint8_t>& session_buffer,
                                             size_t plaintext_length,
                                             int message_type);
    CryptoResult<std::vector<uint8_t>> encrypt_message(const std::vector<uint8_t>& session_buffer,
                                                      const std::vector<uint8_t>& plaintext,
                                                      int message_type);

    CryptoResult<size_t> decrypt_message_size(const std::vector<uint8_t>& session_buffer,
                                             int message_type,
                                             const std::vector<uint8_t>& message);
    CryptoResult<std::vector<uint8_t>> decrypt_message(const std::vector<uint8_t>& session_buffer,
                                                      int message_type,
                                                      const std::vector<uint8_t>& message);

    CryptoResult<bool> session_matches_inbound(const std::vector<uint8_t>& session_buffer,
                                              const std::vector<uint8_t>& one_time_key_message);
    CryptoResult<bool> session_matches_inbound_from(const std::vector<uint8_t>& session_buffer,
                                                   const std::vector<uint8_t>& their_identity_key,
                                                   const std::vector<uint8_t>& one_time_key_message);
    CryptoResult<std::vector<uint8_t>> get_session_id(const std::vector<uint8_t>& session_buffer);
    CryptoResult<bool> session_has_received_message(const std::vector<uint8_t>& session_buffer);
    CryptoResult<size_t> get_library_version();

    CryptoResult<bool> verify_identity_key_signature(const std::vector<uint8_t>& account_buffer,
                                                    const std::vector<uint8_t>& signature);
    std::string get_last_error() const;
    int get_last_error_code() const;

    struct OlmStats {
        size_t accounts_created;
        size_t sessions_created;
        size_t messages_encrypted;
        size_t messages_decrypted;
        size_t errors_occurred;
    };

    OlmStats get_stats() const;

private:
    mutable OlmStats stats_;
    mutable std::string last_error_;
    mutable int last_error_code_ = 0;

    void update_error() const;
    bool check_buffer_size(const std::vector<uint8_t>& buffer, size_t expected_size) const;
};

class OlmAccount {
public:
    OlmAccount();
    ~OlmAccount();

    CryptoResult<bool> create();
    CryptoResult<bool> load(const std::vector<uint8_t>& account_data);
    CryptoResult<std::vector<uint8_t>> save();

    CryptoResult<std::vector<uint8_t>> get_identity_keys();
    CryptoResult<std::vector<uint8_t>> generate_one_time_keys(size_t count);
    CryptoResult<std::vector<uint8_t>> get_one_time_keys();
    CryptoResult<bool> mark_keys_as_published();

    CryptoResult<std::vector<uint8_t>> sign_message(const std::vector<uint8_t>& message);
    CryptoResult<size_t> get_max_one_time_keys();

    std::string get_identity_key_fingerprint() const;
    size_t get_one_time_keys_count() const;

private:
    std::vector<uint8_t> account_buffer_;
    OlmAccount* account_ = nullptr;
    bool initialized_ = false;

    CryptoResult<bool> initialize_account();
    void cleanup_account();
};

class OlmSession {
public:
    OlmSession();
    ~OlmSession();

    CryptoResult<bool> create_outbound(OlmAccount& account,
                                      const std::vector<uint8_t>& their_identity_key,
                                      const std::vector<uint8_t>& their_one_time_key);

    CryptoResult<bool> create_inbound(OlmAccount& account,
                                     const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<bool> create_inbound_from(OlmAccount& account,
                                          const std::vector<uint8_t>& their_identity_key,
                                          const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<bool> load(const std::vector<uint8_t>& session_data);
    CryptoResult<std::vector<uint8_t>> save();

    CryptoResult<std::vector<uint8_t>> encrypt(const std::vector<uint8_t>& plaintext);
    CryptoResult<std::vector<uint8_t>> decrypt(int message_type, const std::vector<uint8_t>& message);

    CryptoResult<bool> matches_inbound(const std::vector<uint8_t>& one_time_key_message);
    CryptoResult<bool> matches_inbound_from(const std::vector<uint8_t>& their_identity_key,
                                           const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<std::vector<uint8_t>> get_session_id();
    CryptoResult<bool> has_received_message();

    std::string get_session_description() const;

private:
    std::vector<uint8_t> session_buffer_;
    OlmSession* session_ = nullptr;
    bool initialized_ = false;

    CryptoResult<bool> initialize_session();
    void cleanup_session();
};

}
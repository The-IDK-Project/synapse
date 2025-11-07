#pragma once

#include "../common/crypto_types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace matrix::crypto::e2ee {

enum class VerificationMethod {
    SAS, // short auth string
    QR_CODE,
    RECIPROCATE
};

enum class VerificationState {
    REQUESTED,
    READY,
    STARTED,
    ACCEPTED,
    CANCELLED,
    DONE
};

struct KeyVerificationRequest {
    std::string verification_id;
    std::string from_user_id;
    std::string from_device_id;
    std::string to_user_id;
    std::string to_device_id;
    std::vector<VerificationMethod> methods;
    Timestamp created_ts;
    Timestamp expires_ts;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct KeyVerificationSession {
    std::string verification_id;
    std::string from_user_id;
    std::string from_device_id;
    std::string to_user_id;
    std::string to_device_id;
    VerificationMethod method;
    VerificationState state;
    std::vector<uint8_t> shared_secret;
    std::vector<uint8_t> commitment;
    std::vector<std::string> sas_codes;
    bool we_started;
    Timestamp created_ts;
    Timestamp last_activity_ts;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

struct SASVerificationData {
    std::vector<uint8_t> public_key;
    std::vector<uint8_t> commitment;
    std::vector<std::string> sas_codes;
    std::vector<uint8_t> mac;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};

class KeyVerificationManager {
public:
    KeyVerificationManager();
    ~KeyVerificationManager();

    bool initialize();
    bool shutdown();

    CryptoResult<std::string> start_verification(const std::string& to_user_id,
                                                const std::string& to_device_id,
                                                VerificationMethod method = VerificationMethod::SAS);

    CryptoResult<bool> accept_verification_request(const std::string& verification_id);
    CryptoResult<bool> cancel_verification(const std::string& verification_id, const std::string& reason = "");
    CryptoResult<bool> confirm_verification(const std::string& verification_id);

    CryptoResult<KeyVerificationRequest> get_verification_request(const std::string& verification_id);
    CryptoResult<KeyVerificationSession> get_verification_session(const std::string& verification_id);

    CryptoResult<SASVerificationData> generate_sas_data(const std::string& verification_id);
    CryptoResult<bool> verify_sas_data(const std::string& verification_id, const SASVerificationData& their_sas_data);

    CryptoResult<std::vector<uint8_t>> generate_qr_code_data(const std::string& verification_id);
    CryptoResult<bool> verify_qr_code_data(const std::string& verification_id, const std::vector<uint8_t>& qr_code_data);

    CryptoResult<bool> mark_device_verified(const std::string& user_id, const std::string& device_id);
    CryptoResult<bool> is_device_verified(const std::string& user_id, const std::string& device_id);

    std::vector<std::string> get_pending_verifications();
    std::vector<std::string> get_active_verifications();
    std::vector<std::string> get_completed_verifications();

    bool is_verification_active(const std::string& verification_id) const;
    bool is_verification_completed(const std::string& verification_id) const;

    struct KeyVerificationStats {
        size_t total_verifications;
        size_t pending_verifications;
        size_t active_verifications;
        size_t completed_verifications;
        size_t cancelled_verifications;
        size_t sas_verifications;
        size_t qr_code_verifications;
    };

    KeyVerificationStats get_stats() const;

private:
    std::unordered_map<std::string, KeyVerificationRequest> verification_requests_;
    std::unordered_map<std::string, KeyVerificationSession> verification_sessions_;
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> verified_devices_;

    mutable std::shared_mutex verification_mutex_;

    KeyVerificationStats stats_;

    std::string generate_verification_id() const;
    CryptoResult<std::vector<uint8_t>> generate_shared_secret() const;
    CryptoResult<std::vector<std::string>> generate_sas_codes(const std::vector<uint8_t>& shared_secret) const;

    CryptoResult<bool> validate_verification_state_transition(VerificationState from, VerificationState to) const;
    CryptoResult<bool> update_verification_state(const std::string& verification_id, VerificationState new_state);

    void cleanup_expired_verifications();
    void cleanup_completed_verifications();
};

class SASVerification {
public:
    SASVerification();
    ~SASVerification();

    CryptoResult<SASVerificationData> generate_verification_data(const std::vector<uint8_t>& our_identity_key,
                                                                const std::vector<uint8_t>& their_identity_key,
                                                                const std::vector<uint8_t>& our_ephemeral_key,
                                                                const std::vector<uint8_t>& their_ephemeral_key);

    CryptoResult<bool> verify_mac(const SASVerificationData& our_data,
                                 const SASVerificationData& their_data,
                                 const std::vector<uint8_t>& their_identity_key,
                                 const std::vector<uint8_t>& our_ephemeral_key,
                                 const std::vector<uint8_t>& their_ephemeral_key);

    static std::vector<std::string> generate_sas_codes_from_bytes(const std::vector<uint8_t>& bytes);
    static std::vector<uint8_t> calculate_commitment(const std::vector<uint8_t>& public_key);
    static std::vector<uint8_t> calculate_mac(const std::vector<uint8_t>& key,
                                             const std::vector<uint8_t>& data);

private:
    CryptoResult<std::vector<uint8_t>> perform_sas_key_agreement(const std::vector<uint8_t>& our_private_key,
                                                                const std::vector<uint8_t>& their_public_key);
};

}
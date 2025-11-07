#pragma once

#include "../common/crypto_types.hpp"
#include "olm_wrapper.hpp"
#include <vector>
#include <memory>

namespace matrix::crypto::olm {

class OlmUtility {
public:
    OlmUtility();
    ~OlmUtility();

    bool initialize();
    bool shutdown();

    CryptoResult<std::vector<uint8_t>> calculate_sha256(const std::vector<uint8_t>& input);
    CryptoResult<std::vector<uint8_t>> calculate_hmac_sha256(const std::vector<uint8_t>& key,
                                                            const std::vector<uint8_t>& message);

    CryptoResult<std::vector<uint8_t>> generate_random_bytes(size_t length);
    CryptoResult<uint32_t> generate_random_uint32();

    CryptoResult<std::vector<uint8_t>> derive_hkdf(const std::vector<uint8_t>& input_key,
                                                  const std::vector<uint8_t>& salt,
                                                  const std::vector<uint8_t>& info,
                                                  size_t output_length);

    CryptoResult<std::vector<uint8_t>> perform_ratchet(const std::vector<uint8_t>& current_key,
                                                      const std::vector<uint8_t>& ratchet_data);

    CryptoResult<bool> verify_ed25519_signature(const std::vector<uint8_t>& message,
                                               const std::vector<uint8_t>& signature,
                                               const std::vector<uint8_t>& public_key);

    CryptoResult<std::vector<uint8_t>> create_ed25519_signature(const std::vector<uint8_t>& message,
                                                               const std::vector<uint8_t>& private_key);

    CryptoResult<std::vector<uint8_t>> convert_curve25519_to_ed25519(const std::vector<uint8_t>& curve25519_key);
    CryptoResult<std::vector<uint8_t>> convert_ed25519_to_curve25519(const std::vector<uint8_t>& ed25519_key);

    CryptoResult<std::string> calculate_key_fingerprint(const std::vector<uint8_t>& public_key);
    CryptoResult<std::string> calculate_session_fingerprint(const std::vector<uint8_t>& session_data);

    CryptoResult<bool> validate_pre_key_message_format(const std::vector<uint8_t>& message);
    CryptoResult<bool> validate_message_format(const std::vector<uint8_t>& message);

    CryptoResult<std::string> base64_encode(const std::vector<uint8_t>& data);
    CryptoResult<std::vector<uint8_t>> base64_decode(const std::string& encoded_data);

    CryptoResult<std::vector<uint8_t>> calculate_olm_session_id(const std::vector<uint8_t>& session_data);
    CryptoResult<bool> verify_olm_account_signature(const std::vector<uint8_t>& account_data,
                                                   const std::vector<uint8_t>& signature);

    struct UtilityStats {
        size_t sha256_operations;
        size_t hmac_operations;
        size_t random_generations;
        size_t signature_verifications;
        size_t key_conversions;
        size_t base64_operations;
    };

    UtilityStats get_stats() const;

private:
    mutable UtilityStats stats_;

    void update_stats_sha256();
    void update_stats_hmac();
    void update_stats_random();
    void update_stats_signature();
    void update_stats_key_conversion();
    void update_stats_base64();
};

class OlmKeyDerivation {
public:
    OlmKeyDerivation();
    ~OlmKeyDerivation();

    CryptoResult<std::vector<uint8_t>> derive_olm_ratchet_key(const std::vector<uint8_t>& shared_secret,
                                                             const std::vector<uint8_t>& ratchet_info);

    CryptoResult<std::vector<uint8_t>> derive_chain_key(const std::vector<uint8_t>& root_key,
                                                       const std::vector<uint8_t>& ratchet_data);

    CryptoResult<std::vector<uint8_t>> derive_message_key(const std::vector<uint8_t>& chain_key);

    CryptoResult<std::vector<uint8_t>> perform_double_ratchet(const std::vector<uint8_t>& current_root_key,
                                                             const std::vector<uint8_t>& ratchet_data);

    static size_t get_olm_shared_secret_size();
    static size_t get_olm_root_key_size();
    static size_t get_olm_chain_key_size();
    static size_t get_olm_message_key_size();

private:
    CryptoResult<std::vector<uint8_t>> hkdf_expand(const std::vector<uint8_t>& prk,
                                                  const std::vector<uint8_t>& info,
                                                  size_t output_length);

    CryptoResult<std::vector<uint8_t>> hkdf_extract(const std::vector<uint8_t>& salt,
                                                   const std::vector<uint8_t>& input_key);
};

class OlmSessionValidator {
public:
    OlmSessionValidator();
    ~OlmSessionValidator();

    CryptoResult<bool> validate_session_creation(const std::vector<uint8_t>& account_data,
                                                const std::vector<uint8_t>& their_identity_key,
                                                const std::vector<uint8_t>& their_one_time_key);

    CryptoResult<bool> validate_inbound_session(const std::vector<uint8_t>& account_data,
                                               const std::vector<uint8_t>& one_time_key_message);

    CryptoResult<bool> validate_session_data(const std::vector<uint8_t>& session_data);

    CryptoResult<bool> verify_session_consistency(const std::vector<uint8_t>& session_data1,
                                                 const std::vector<uint8_t>& session_data2);

    CryptoResult<bool> check_session_expiry(const std::vector<uint8_t>& session_data,
                                           uint64_t max_age_seconds);

    static CryptoResult<bool> validate_identity_key_format(const std::vector<uint8_t>& identity_key);
    static CryptoResult<bool> validate_one_time_key_format(const std::vector<uint8_t>& one_time_key);
    static CryptoResult<bool> validate_pre_key_message_format(const std::vector<uint8_t>& message);

private:
    bool check_key_curve(const std::vector<uint8_t>& key) const;
    bool check_message_structure(const std::vector<uint8_t>& message) const;
};

class OlmPerformanceMonitor {
public:
    OlmPerformanceMonitor();
    ~OlmPerformanceMonitor();

    struct PerformanceMetrics {
        int64_t session_creation_time_ns;
        int64_t message_encryption_time_ns;
        int64_t message_decryption_time_ns;
        int64_t key_generation_time_ns;
        int64_t signature_verification_time_ns;
        size_t memory_usage_bytes;
    };

    CryptoResult<PerformanceMetrics> measure_session_creation();
    CryptoResult<PerformanceMetrics> measure_message_encryption(size_t message_size);
    CryptoResult<PerformanceMetrics> measure_message_decryption(size_t message_size);
    CryptoResult<PerformanceMetrics> measure_key_generation();
    CryptoResult<PerformanceMetrics> measure_signature_verification();

    CryptoResult<PerformanceMetrics> run_comprehensive_benchmark(size_t iterations = 1000);

    static PerformanceMetrics get_current_metrics();
    static void reset_metrics();

private:
    static PerformanceMetrics current_metrics_;

    int64_t measure_operation(std::function<void()> operation);
    size_t measure_memory_usage();
};

}
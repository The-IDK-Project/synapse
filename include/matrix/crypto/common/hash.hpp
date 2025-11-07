#pragma once

#include "crypto_types.hpp"
#include <memory>
#include <vector>

namespace matrix::crypto {

class HashFunction {
public:
    virtual ~HashFunction() = default;

    virtual bool initialize(HashAlgorithm algorithm) = 0;
    virtual bool update(const std::vector<uint8_t>& data) = 0;
    virtual CryptoResult<HashResult> finalize() = 0;
    virtual bool reset() = 0;

    virtual size_t get_digest_size(HashAlgorithm algorithm) const = 0;
    virtual size_t get_block_size(HashAlgorithm algorithm) const = 0;
};

class Hash {
public:
    Hash();
    ~Hash();

    CryptoResult<HashResult> compute_hash(const std::vector<uint8_t>& data,
                                         HashAlgorithm algorithm = HashAlgorithm::SHA256);

    CryptoResult<HashResult> compute_hash(const std::string& data,
                                         HashAlgorithm algorithm = HashAlgorithm::SHA256);

    CryptoResult<std::vector<uint8_t>> compute_hmac(const std::vector<uint8_t>& data,
                                                   const std::vector<uint8_t>& key,
                                                   HashAlgorithm algorithm = HashAlgorithm::SHA256);

    CryptoResult<std::vector<uint8_t>> compute_pbkdf2(const std::string& password,
                                                     const std::vector<uint8_t>& salt,
                                                     int iterations,
                                                     size_t key_length,
                                                     HashAlgorithm algorithm = HashAlgorithm::SHA256);

    CryptoResult<std::vector<uint8_t>> compute_argon2id(const std::string& password,
                                                       const std::vector<uint8_t>& salt,
                                                       int time_cost,
                                                       int memory_cost,
                                                       int parallelism,
                                                       size_t key_length);

    CryptoResult<bool> verify_hash(const std::vector<uint8_t>& data,
                                  const HashResult& expected_hash);

    CryptoResult<bool> verify_hmac(const std::vector<uint8_t>& data,
                                  const std::vector<uint8_t>& key,
                                  const std::vector<uint8_t>& expected_hmac,
                                  HashAlgorithm algorithm = HashAlgorithm::SHA256);

    static size_t get_digest_size(HashAlgorithm algorithm);
    static size_t get_block_size(HashAlgorithm algorithm);
    static bool is_algorithm_supported(HashAlgorithm algorithm);

private:
    std::unique_ptr<HashFunction> create_hash_function(HashAlgorithm algorithm);
};

class StreamingHash {
public:
    StreamingHash(HashAlgorithm algorithm = HashAlgorithm::SHA256);
    ~StreamingHash();

    bool initialize();
    bool update(const std::vector<uint8_t>& data);
    bool update(const std::string& data);
    CryptoResult<HashResult> finalize();
    bool reset();

    HashAlgorithm get_algorithm() const;
    size_t get_bytes_processed() const;

private:
    HashAlgorithm algorithm_;
    std::unique_ptr<HashFunction> hash_function_;
    size_t bytes_processed_;
};

class HMAC {
public:
    HMAC(const std::vector<uint8_t>& key, HashAlgorithm algorithm = HashAlgorithm::SHA256);
    ~HMAC();

    bool initialize();
    bool update(const std::vector<uint8_t>& data);
    bool update(const std::string& data);
    CryptoResult<std::vector<uint8_t>> finalize();
    bool reset();

    CryptoResult<std::vector<uint8_t>> compute(const std::vector<uint8_t>& data);
    CryptoResult<bool> verify(const std::vector<uint8_t>& data, const std::vector<uint8_t>& expected_hmac);

private:
    std::vector<uint8_t> key_;
    HashAlgorithm algorithm_;
    std::unique_ptr<HashFunction> hash_function_;
};

class PasswordHasher {
public:
    struct PasswordHash {
        std::vector<uint8_t> hash;
        std::vector<uint8_t> salt;
        HashAlgorithm algorithm;
        int iterations;
        int time_cost;
        int memory_cost;
        int parallelism;
        Timestamp created_ts;
    };

    PasswordHasher();
    ~PasswordHasher();

    CryptoResult<PasswordHash> hash_password(const std::string& password,
                                            HashAlgorithm algorithm = HashAlgorithm::SHA256);

    CryptoResult<PasswordHash> hash_password_argon2id(const std::string& password,
                                                     int time_cost = 3,
                                                     int memory_cost = 65536,
                                                     int parallelism = 4);

    CryptoResult<bool> verify_password(const std::string& password, const PasswordHash& password_hash);

    CryptoResult<bool> needs_rehash(const PasswordHash& password_hash);

    bool set_default_algorithm(HashAlgorithm algorithm);
    bool set_default_work_factors(int iterations, int time_cost, int memory_cost, int parallelism);

private:
    HashAlgorithm default_algorithm_;
    int default_iterations_;
    int default_time_cost_;
    int default_memory_cost_;
    int default_parallelism_;

    std::vector<uint8_t> generate_salt(size_t size) const;
};

class HashVerifier {
public:
    HashVerifier();
    ~HashVerifier();

    CryptoResult<bool> verify_hash(const std::vector<uint8_t>& data,
                                  const HashResult& expected_hash);

    CryptoResult<bool> verify_hmac(const std::vector<uint8_t>& data,
                                  const std::vector<uint8_t>& key,
                                  const std::vector<uint8_t>& expected_hmac,
                                  HashAlgorithm algorithm = HashAlgorithm::SHA256);

    CryptoResult<bool> verify_password(const std::string& password,
                                      const PasswordHasher::PasswordHash& password_hash);

    CryptoResult<bool> verify_timing_safe(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

    static bool constant_time_compare(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

private:
    PasswordHasher password_hasher_;
};

class HashBenchmark {
public:
    struct BenchmarkResult {
        HashAlgorithm algorithm;
        size_t data_size;
        int64_t time_ns;
        double speed_mbps;
        int iterations;
    };

    HashBenchmark();
    ~HashBenchmark();

    CryptoResult<BenchmarkResult> benchmark_algorithm(HashAlgorithm algorithm,
                                                     size_t data_size = 1024 * 1024,
                                                     int iterations = 1000);

    std::vector<BenchmarkResult> benchmark_all_algorithms(size_t data_size = 1024 * 1024,
                                                         int iterations = 1000);

    CryptoResult<BenchmarkResult> benchmark_hmac(HashAlgorithm algorithm,
                                                size_t data_size = 1024 * 1024,
                                                int iterations = 1000);

    nlohmann::json get_benchmark_results() const;

private:
    std::vector<uint8_t> generate_test_data(size_t size) const;
    std::vector<uint8_t> generate_test_key(size_t size) const;
};

}
#pragma once

#include "cache.hpp"
#include <hiredis/hiredis.h>
#include <memory>
#include <vector>

namespace matrix::storage::cache {

class RedisCache : public Cache {
public:
    struct RedisConfig {
        std::string host = "localhost";
        int port = 6379;
        std::string password;
        int database = 0;
        int timeout_ms = 5000;
        int max_connections = 10;
        bool use_ssl = false;
        std::string ssl_cert;
        std::string ssl_key;
        std::string ssl_ca;
    };

    RedisCache(const RedisConfig& config);
    ~RedisCache();

    bool set(const std::string& key, const std::string& value, int ttl_seconds = 0) override;
    std::string get(const std::string& key) override;
    bool exists(const std::string& key) override;
    bool remove(const std::string& key) override;
    bool clear() override;

    bool set_serialized(const std::string& key, const nlohmann::json& value, int ttl_seconds = 0) override;
    nlohmann::json get_serialized(const std::string& key) override;

    bool set_binary(const std::string& key, const std::vector<uint8_t>& value, int ttl_seconds = 0) override;
    std::vector<uint8_t> get_binary(const std::string& key) override;

    int64_t get_size() const override;
    int64_t get_max_size() const override;
    int64_t get_item_count() const override;

    bool initialize() override;
    bool shutdown() override;

    CacheStats get_stats() const override;


    bool set_multiple(const std::vector<std::pair<std::string, std::string>>& key_values, int ttl_seconds = 0);
    std::vector<std::string> get_multiple(const std::vector<std::string>& keys);
    std::vector<std::string> get_keys(const std::string& pattern = "*");

    bool set_hash(const std::string& key, const std::unordered_map<std::string, std::string>& hash);
    std::unordered_map<std::string, std::string> get_hash(const std::string& key);
    bool set_hash_field(const std::string& key, const std::string& field, const std::string& value);
    std::string get_hash_field(const std::string& key, const std::string& field);

    bool add_to_set(const std::string& key, const std::vector<std::string>& members);
    std::vector<std::string> get_set_members(const std::string& key);
    bool is_set_member(const std::string& key, const std::string& member);

    bool push_to_list(const std::string& key, const std::vector<std::string>& values, bool left = false);
    std::vector<std::string> get_list_range(const std::string& key, int start = 0, int stop = -1);

    bool increment_counter(const std::string& key, int64_t increment = 1);
    int64_t get_counter(const std::string& key);

    bool acquire_lock(const std::string& lock_key, const std::string& lock_value, int ttl_seconds = 30);
    bool release_lock(const std::string& lock_key, const std::string& lock_value);
    bool extend_lock(const std::string& lock_key, const std::string& lock_value, int ttl_seconds);

    bool publish_message(const std::string& channel, const std::string& message);
    bool subscribe_to_channel(const std::string& channel, std::function<void(const std::string&)> callback);
    bool unsubscribe_from_channel(const std::string& channel);

    bool execute_script(const std::string& script, const std::vector<std::string>& keys, const std::vector<std::string>& args);
    bool pipeline_operations(const std::vector<std::string>& commands);

private:
    RedisConfig config_;
    redisContext* context_ = nullptr;
    mutable std::mutex connection_mutex_;

    struct RedisStats {
        int64_t commands_processed;
        int64_t connections_created;
        int64_t errors_occurred;
        Timestamp last_connection_time;
    };

    mutable RedisStats stats_;

    bool connect();
    bool disconnect();
    bool check_connection();
    bool reconnect_if_needed();

    redisReply* execute_command(const std::string& command);
    redisReply* execute_command(const std::vector<std::string>& command_args);
    bool handle_redis_reply(redisReply* reply);

    std::string build_command(const std::vector<std::string>& args) const;
    bool validate_key(const std::string& key) const;

    static void free_reply(redisReply* reply);
};

class RedisPool {
public:
    RedisPool(const RedisCache::RedisConfig& config, size_t pool_size = 10);
    ~RedisPool();

    std::unique_ptr<RedisCache> acquire();
    bool release(std::unique_ptr<RedisCache> cache);

    bool initialize();
    bool shutdown();

    struct PoolStats {
        size_t total_connections;
        size_t active_connections;
        size_t idle_connections;
        size_t connection_errors;
    };

    PoolStats get_stats() const;

private:
    RedisCache::RedisConfig config_;
    size_t pool_size_;

    mutable std::mutex pool_mutex_;
    std::condition_variable pool_condition_;
    std::vector<std::unique_ptr<RedisCache>> available_caches_;
    std::vector<std::unique_ptr<RedisCache>> in_use_caches_;

    std::atomic<size_t> total_connections_{0};
    std::atomic<size_t> connection_errors_{0};
};

class RedisClusterCache : public Cache {
public:
    struct RedisClusterConfig {
        std::vector<std::pair<std::string, int>> nodes;
        std::string password;
        int timeout_ms = 5000;
        int max_redirects = 3;
    };

    RedisClusterCache(const RedisClusterConfig& config);
    ~RedisClusterCache();

    bool set(const std::string& key, const std::string& value, int ttl_seconds = 0) override;
    std::string get(const std::string& key) override;
    bool exists(const std::string& key) override;
    bool remove(const std::string& key) override;
    bool clear() override;

    int64_t get_size() const override;
    int64_t get_max_size() const override;
    int64_t get_item_count() const override;

    bool initialize() override;
    bool shutdown() override;

    CacheStats get_stats() const override;

private:
    RedisClusterConfig config_;
    void* cluster_context_ = nullptr;
    mutable std::mutex cluster_mutex_;

    bool initialize_cluster();
    bool execute_cluster_command(const std::string& command, const std::vector<std::string>& args);
};

}
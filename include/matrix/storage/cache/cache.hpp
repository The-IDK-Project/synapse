#pragma once

#include "../../core/matrix_types.hpp"
#include <memory>
#include <string>
#include <functional>

namespace matrix::storage::cache {

class Cache {
public:
    virtual ~Cache() = default;

    virtual bool set(const std::string& key, const std::string& value, int ttl_seconds = 0) = 0;
    virtual std::string get(const std::string& key) = 0;
    virtual bool exists(const std::string& key) = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual bool clear() = 0;

    virtual bool set_serialized(const std::string& key, const nlohmann::json& value, int ttl_seconds = 0);
    virtual nlohmann::json get_serialized(const std::string& key);

    virtual bool set_binary(const std::string& key, const std::vector<uint8_t>& value, int ttl_seconds = 0);
    virtual std::vector<uint8_t> get_binary(const std::string& key);

    virtual int64_t get_size() const = 0;
    virtual int64_t get_max_size() const = 0;
    virtual int64_t get_item_count() const = 0;

    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;

    struct CacheStats {
        int64_t total_items;
        int64_t total_size;
        int64_t hits;
        int64_t misses;
        int64_t evictions;
        double hit_rate;
        int64_t memory_usage;
    };

    virtual CacheStats get_stats() const = 0;
};

template<typename T>
class TypedCache {
public:
    virtual ~TypedCache() = default;

    virtual bool set(const std::string& key, const T& value, int ttl_seconds = 0) = 0;
    virtual std::unique_ptr<T> get(const std::string& key) = 0;
    virtual bool exists(const std::string& key) = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual bool clear() = 0;

    virtual T get_or_default(const std::string& key, const T& default_value);
    virtual T get_or_compute(const std::string& key, std::function<T()> computer, int ttl_seconds = 0);
};

class CacheManager {
public:
    CacheManager();
    ~CacheManager();

    bool initialize();
    bool shutdown();

    std::shared_ptr<Cache> get_cache(const std::string& name);
    bool register_cache(const std::string& name, std::shared_ptr<Cache> cache);
    bool unregister_cache(const std::string& name);

    bool clear_all_caches();
    nlohmann::json get_all_stats() const;

    struct CacheConfig {
        std::string type;
        size_t max_size;
        int default_ttl;
        bool compression;
        std::string backend_config;
    };

    bool configure_cache(const std::string& name, const CacheConfig& config);

private:
    std::unordered_map<std::string, std::shared_ptr<Cache>> caches_;
    mutable std::shared_mutex caches_mutex_;
};

}
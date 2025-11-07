#pragma once

#include "cache.hpp"
#include <list>
#include <unordered_map>
#include <shared_mutex>

namespace matrix::storage::cache {

template<typename K, typename V>
class LRUCache {
public:
    LRUCache(size_t max_size);
    ~LRUCache();

    bool set(const K& key, const V& value);
    std::unique_ptr<V> get(const K& key);
    bool exists(const K& key) const;
    bool remove(const K& key);
    bool clear();

    size_t size() const;
    size_t max_size() const;
    void set_max_size(size_t max_size);

    struct CacheStats {
        size_t hits;
        size_t misses;
        size_t evictions;
        size_t current_size;
        size_t max_size;
    };

    CacheStats get_stats() const;

private:
    struct CacheNode {
        K key;
        V value;
        Timestamp timestamp;
    };

    using ListType = std::list<CacheNode>;
    using MapType = std::unordered_map<K, typename ListType::iterator>;

    mutable std::shared_mutex mutex_;
    ListType lru_list_;
    MapType cache_map_;
    size_t max_size_;
    mutable CacheStats stats_;

    void evict_if_needed();
    void touch(typename ListType::iterator it);
};

class LRUMemoryCache : public Cache {
public:
    LRUMemoryCache(size_t max_size = 1024 * 1024 * 100);
    ~LRUMemoryCache();

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

    bool set_compression(bool enable);
    bool is_compression_enabled() const;

private:
    struct CacheItem {
        std::string value;
        Timestamp expires_at;
        size_t size;
        bool compressed;
    };

    LRUCache<std::string, CacheItem> cache_;
    bool compression_enabled_ = false;
    std::atomic<int64_t> total_size_{0};

    std::string compress_value(const std::string& value) const;
    std::string decompress_value(const std::string& value) const;
    size_t calculate_item_size(const std::string& key, const std::string& value) const;
    bool is_expired(const CacheItem& item) const;
};

template<typename T>
class TypedLRUCache : public TypedCache<T> {
public:
    TypedLRUCache(size_t max_size) : cache_(max_size) {}

    bool set(const std::string& key, const T& value, int ttl_seconds = 0) override {
        // Serialize T to string and store in cache
        return true;
    }

    std::unique_ptr<T> get(const std::string& key) override {
        // Deserialize from cache
        return nullptr;
    }

    bool exists(const std::string& key) override {
        return cache_.exists(key);
    }

    bool remove(const std::string& key) override {
        return cache_.remove(key);
    }

    bool clear() override {
        return cache_.clear();
    }

private:
    LRUMemoryCache cache_;
};

class LRUCacheManager {
public:
    LRUCacheManager();
    ~LRUCacheManager();

    static std::shared_ptr<LRUMemoryCache> create_cache(const std::string& name, size_t max_size);
    static bool destroy_cache(const std::string& name);
    static std::shared_ptr<LRUMemoryCache> get_cache(const std::string& name);

    static void set_global_max_size(size_t max_size);
    static size_t get_global_max_size();

    static nlohmann::json get_global_stats();

private:
    static std::unordered_map<std::string, std::shared_ptr<LRUMemoryCache>> caches_;
    static size_t global_max_size_;
    static std::shared_mutex caches_mutex_;
};

}
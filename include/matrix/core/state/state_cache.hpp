#pragma once

#include "../event/event.hpp"
#include "../room/room_state.hpp"
#include "../matrix_types.hpp"
#include <unordered_map>
#include <memory>
#include <shared_mutex>
#include <list>

namespace matrix {

class StateCache {
public:
    struct CacheConfig {
        size_t max_room_states = 1000;
        size_t max_events_per_room = 10000;
        size_t max_auth_chains = 500;
        int default_ttl_seconds = 3600;
        bool enable_compression = true;
    };

    StateCache(const CacheConfig& config = CacheConfig());
    ~StateCache() = default;

    bool store_room_state(const RoomID& room_id, const RoomStatePtr& state, int ttl_seconds = -1);
    RoomStatePtr get_room_state(const RoomID& room_id) const;
    bool has_room_state(const RoomID& room_id) const;
    bool remove_room_state(const RoomID& room_id);

    bool store_state_event(const RoomID& room_id, const EventPtr& event, int ttl_seconds = -1);
    EventPtr get_state_event(const RoomID& room_id, const std::string& event_type, const std::string& state_key = "") const;
    std::vector<EventPtr> get_state_events(const RoomID& room_id, const std::string& event_type) const;

    bool store_auth_chain(const RoomID& room_id, const std::vector<EventPtr>& auth_chain, int ttl_seconds = -1);
    std::vector<EventPtr> get_auth_chain(const RoomID& room_id) const;
    bool has_auth_chain(const RoomID& room_id) const;

    bool store_power_levels(const RoomID& room_id, const PowerLevels& levels, int ttl_seconds = -1);
    PowerLevels get_power_levels(const RoomID& room_id) const;
    bool has_power_levels(const RoomID& room_id) const;

    bool store_room_members(const RoomID& room_id, const std::vector<UserID>& members, Membership membership, int ttl_seconds = -1);
    std::vector<UserID> get_room_members(const RoomID& room_id, Membership membership) const;
    bool has_room_members(const RoomID& room_id, Membership membership) const;

    bool store_room_summary(const RoomID& room_id, const nlohmann::json& summary, int ttl_seconds = -1);
    nlohmann::json get_room_summary(const RoomID& room_id) const;
    bool has_room_summary(const RoomID& room_id) const;

    void set_room_state_ttl(const RoomID& room_id, int ttl_seconds);
    void set_default_ttl(int ttl_seconds);

    bool touch_room_state(const RoomID& room_id);
    bool touch_auth_chain(const RoomID& room_id);

    size_t get_room_state_count() const;
    size_t get_auth_chain_count() const;
    size_t get_total_cached_items() const;

    nlohmann::json get_cache_stats() const;

    void clear_room_cache(const RoomID& room_id);
    void clear_all_caches();

    void cleanup_expired();
    void cleanup_lru();

    void resize(size_t max_room_states, size_t max_events_per_room);

private:
    struct CacheEntry {
        std::shared_ptr<void> data;
        Timestamp created_ts;
        Timestamp expires_ts;
        Timestamp last_accessed;
        size_t size;
    };

    struct RoomStateCache {
        RoomStatePtr state;
        std::unordered_map<std::string, EventPtr> state_events;
        std::unordered_map<std::string, std::vector<EventPtr>> events_by_type;
    };

    mutable std::shared_mutex mutex_;
    CacheConfig config_;

    std::unordered_map<RoomID, CacheEntry> room_states_;
    std::unordered_map<RoomID, CacheEntry> auth_chains_;
    std::unordered_map<RoomID, CacheEntry> power_levels_;
    std::unordered_map<RoomID, std::unordered_map<Membership, CacheEntry>> room_members_;
    std::unordered_map<RoomID, CacheEntry> room_summaries_;

    std::list<RoomID> room_state_lru_;
    std::list<RoomID> auth_chain_lru_;

    template<typename T>
    bool store_in_cache(std::unordered_map<RoomID, CacheEntry>& cache,
                       const RoomID& room_id,
                       const T& data,
                       int ttl_seconds);

    template<typename T>
    std::shared_ptr<T> get_from_cache(const std::unordered_map<RoomID, CacheEntry>& cache,
                                     const RoomID& room_id) const;

    void update_lru(std::list<RoomID>& lru_list, const RoomID& room_id);
    void evict_lru(std::unordered_map<RoomID, CacheEntry>& cache, std::list<RoomID>& lru_list);

    bool is_expired(const CacheEntry& entry) const;
    int calculate_ttl(int ttl_seconds) const;
    size_t calculate_size(const RoomStatePtr& state) const;
    size_t calculate_size(const std::vector<EventPtr>& events) const;
    size_t calculate_size(const PowerLevels& levels) const;
    size_t calculate_size(const std::vector<UserID>& members) const;
    size_t calculate_size(const nlohmann::json& data) const;

    void compress_cache();
};

}
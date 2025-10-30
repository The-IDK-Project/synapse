#pragma once

#include "../event/event.hpp"
#include "room_state.hpp"
#include <vector>
#include <memory>
#include <set>

namespace matrix {

class StateResolution {
public:
    StateResolution() = default;
    ~StateResolution() = default;

    static RoomStatePtr resolve_state(
        const RoomID& room_id,
        const std::vector<EventPtr>& state_events,
        const std::vector<EventPtr>& conflicted_events,
        const std::vector<EventPtr>& unconflicted_events,
        const std::vector<EventPtr>& auth_events
    );

    static RoomStatePtr resolve_state_events(
        const RoomID& room_id,
        const std::vector<EventPtr>& state_events
    );

    static std::vector<EventPtr> resolve_events_conflict(
        const std::vector<EventPtr>& conflicted_events,
        const std::vector<EventPtr>& auth_events
    );

    static std::vector<EventPtr> separate_events(
        const std::vector<EventPtr>& events,
        std::vector<EventPtr>& unconflicted_events,
        std::vector<EventPtr>& conflicted_events
    );

    static std::vector<EventPtr> get_auth_chain(
        const std::vector<EventPtr>& events
    );

    static std::vector<EventPtr> get_auth_chain_difference(
        const std::vector<EventPtr>& state_sets,
        const std::vector<EventPtr>& base_chain
    );

    static bool is_event_auth_chain_contained(
        const EventPtr& event,
        const std::vector<EventPtr>& auth_chain
    );

    static int calculate_event_power_level(
        const EventPtr& event,
        const std::vector<EventPtr>& auth_events
    );

    static std::string calculate_event_origin(
        const EventPtr& event
    );

    static int64_t calculate_event_depth(
        const EventPtr& event
    );

    class PowerLevelComparator {
    public:
        bool operator()(const EventPtr& a, const EventPtr& b) const;
    };

    class DepthComparator {
    public:
        bool operator()(const EventPtr& a, const EventPtr& b) const;
    };

    class EventKey {
    public:
        EventKey(const std::string& type, const std::string& state_key);
        bool operator<(const EventKey& other) const;

        std::string type;
        std::string state_key;
    };

    static std::set<EventKey> get_event_keys(const std::vector<EventPtr>& events);

private:
    static std::vector<EventPtr> topological_sort(const std::vector<EventPtr>& events);
    static std::vector<EventPtr> get_event_auth(const EventPtr& event);
    static bool is_power_event(const EventPtr& event);
    static int get_event_power_level(const EventPtr& event, const std::vector<EventPtr>& auth_events);
    static std::string get_event_origin(const EventPtr& event);
};

}
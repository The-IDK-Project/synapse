#pragma once

#include "../event/event.hpp"
#include "../room/room_state.hpp"
#include "../matrix_types.hpp"
#include <unordered_map>
#include <memory>
#include <shared_mutex>

namespace matrix {

class StateManager {
public:
    StateManager();
    ~StateManager() = default;

    bool set_room_state(const RoomID& room_id, const RoomStatePtr& state);
    RoomStatePtr get_room_state(const RoomID& room_id) const;
    bool has_room_state(const RoomID& room_id) const;
    bool remove_room_state(const RoomID& room_id);

    bool apply_state_event(const RoomID& room_id, const EventPtr& event);
    bool apply_state_events(const RoomID& room_id, const std::vector<EventPtr>& events);

    EventPtr get_state_event(const RoomID& room_id, const std::string& event_type, const std::string& state_key = "") const;
    std::vector<EventPtr> get_state_events(const RoomID& room_id, const std::string& event_type) const;
    std::vector<EventPtr> get_current_state(const RoomID& room_id) const;

    std::vector<UserID> get_room_members(const RoomID& room_id, Membership membership = Membership::JOIN) const;
    Membership get_membership(const RoomID& room_id, const UserID& user_id) const;
    bool is_user_in_room(const RoomID& room_id, const UserID& user_id) const;
    bool is_user_joined(const RoomID& room_id, const UserID& user_id) const;
    bool is_user_invited(const RoomID& room_id, const UserID& user_id) const;
    bool is_user_banned(const RoomID& room_id, const UserID& user_id) const;

    PowerLevels get_power_levels(const RoomID& room_id) const;
    int get_user_power_level(const RoomID& room_id, const UserID& user_id) const;
    int get_event_power_level(const RoomID& room_id, const std::string& event_type) const;

    bool user_can_send_event(const RoomID& room_id, const UserID& user_id, const std::string& event_type) const;
    bool user_can_invite(const RoomID& room_id, const UserID& user_id) const;
    bool user_can_ban(const RoomID& room_id, const UserID& user_id) const;
    bool user_can_kick(const RoomID& room_id, const UserID& user_id) const;
    bool user_can_redact(const RoomID& room_id, const UserID& user_id) const;

    std::string get_room_name(const RoomID& room_id) const;
    std::string get_room_topic(const RoomID& room_id) const;
    std::string get_room_avatar_url(const RoomID& room_id) const;
    std::string get_room_creator(const RoomID& room_id) const;
    std::string get_room_version(const RoomID& room_id) const;
    std::string get_room_join_rules(const RoomID& room_id) const;
    std::string get_room_guest_access(const RoomID& room_id) const;
    std::string get_room_history_visibility(const RoomID& room_id) const;
    bool is_room_encrypted(const RoomID& room_id) const;
    std::string get_room_encryption_algorithm(const RoomID& room_id) const;

    RoomStatePtr resolve_state(const RoomID& room_id, const std::vector<EventPtr>& state_events);
    RoomStatePtr resolve_state_conflict(const RoomID& room_id, const std::vector<EventPtr>& conflicted_events, const std::vector<EventPtr>& auth_events);

    std::vector<EventPtr> get_auth_chain(const RoomID& room_id, const std::vector<EventPtr>& events);
    std::vector<EventPtr> get_auth_chain_difference(const RoomID& room_id, const std::vector<EventPtr>& state_sets);

    nlohmann::json get_room_summary(const RoomID& room_id) const;
    nlohmann::json get_room_state_snapshot(const RoomID& room_id) const;

    void clear_room_state(const RoomID& room_id);
    void clear_all_states();

    size_t get_room_state_count(const RoomID& room_id) const;
    size_t get_total_state_count() const;

    std::vector<RoomID> get_managed_rooms() const;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<RoomID, RoomStatePtr> room_states_;
    std::unordered_map<RoomID, std::vector<EventPtr>> room_auth_chains_;

    RoomStatePtr get_or_create_room_state(const RoomID& room_id);
    void update_auth_chain(const RoomID& room_id, const EventPtr& event);
    std::vector<EventPtr> calculate_auth_chain(const std::vector<EventPtr>& events) const;

    bool validate_state_event(const EventPtr& event) const;
    bool is_power_event(const EventPtr& event) const;

    void cleanup_old_states();
};

}
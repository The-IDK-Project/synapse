#pragma once

#include "../event/event.hpp"
#include "../matrix_types.hpp"
#include <unordered_map>
#include <vector>
#include <memory>

namespace matrix {

class RoomState {
public:
    RoomState(const RoomID& room_id);
    ~RoomState() = default;

    RoomID room_id() const { return room_id_; }

    bool apply_event(const EventPtr& event);
    bool is_event_allowed(const EventPtr& event) const;

    EventPtr get_state_event(const std::string& event_type, const std::string& state_key = "") const;
    std::vector<EventPtr> get_state_events(const std::string& event_type) const;
    std::vector<EventPtr> get_all_state_events() const;

    std::vector<UserID> get_joined_members() const;
    std::vector<UserID> get_invited_members() const;
    std::vector<UserID> get_banned_members() const;
    Membership get_membership(const UserID& user_id) const;

    PowerLevels power_levels() const;
    int get_user_power_level(const UserID& user_id) const;
    int get_event_power_level(const std::string& event_type) const;

    std::string creator() const;
    std::string name() const;
    std::string topic() const;
    std::string avatar_url() const;
    std::string join_rules() const;
    std::string guest_access() const;
    std::string history_visibility() const;
    bool is_encrypted() const;
    std::string encryption_algorithm() const;

    bool user_can_send_event(const UserID& user_id, const std::string& event_type) const;
    bool user_can_invite(const UserID& user_id) const;
    bool user_can_ban(const UserID& user_id) const;
    bool user_can_kick(const UserID& user_id) const;
    bool user_can_redact(const UserID& user_id) const;

    nlohmann::json to_json() const;
    std::shared_ptr<RoomState> copy() const;

    size_t size() const { return state_events_.size(); }
    void clear();

private:
    RoomID room_id_;
    std::unordered_map<std::string, std::unordered_map<std::string, EventPtr>> state_events_;

    EventPtr get_power_levels_event() const;
    EventPtr get_join_rules_event() const;
    EventPtr get_history_visibility_event() const;
    EventPtr get_guest_access_event() const;
    EventPtr get_encryption_event() const;
    EventPtr get_create_event() const;

    void update_membership(const EventPtr& event);
    void update_power_levels(const EventPtr& event);
    void update_join_rules(const EventPtr& event);

    static const std::vector<std::string> STATE_EVENT_TYPES;
};

using RoomStatePtr = std::shared_ptr<RoomState>;

}
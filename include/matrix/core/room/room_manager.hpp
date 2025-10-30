#pragma once

#include "room.hpp"
#include "../event/event.hpp"
#include <unordered_map>
#include <memory>
#include <shared_mutex>

namespace matrix {

class RoomManager {
public:
    RoomManager();
    ~RoomManager() = default;

    RoomPtr create_room(const UserID& creator, const std::string& name = "", bool is_public = false);
    RoomPtr get_room(const RoomID& room_id) const;
    bool has_room(const RoomID& room_id) const;
    bool remove_room(const RoomID& room_id);

    std::vector<RoomPtr> get_rooms() const;
    std::vector<RoomPtr> get_rooms_for_user(const UserID& user_id) const;
    std::vector<RoomPtr> get_public_rooms() const;

    bool add_user_to_room(const RoomID& room_id, const UserID& user_id, Membership membership);
    bool remove_user_from_room(const RoomID& room_id, const UserID& user_id);
    Membership get_user_membership(const RoomID& room_id, const UserID& user_id) const;

    bool add_event_to_room(const RoomID& room_id, const EventPtr& event);
    EventPtr get_room_event(const RoomID& room_id, const EventID& event_id) const;
    std::vector<EventPtr> get_room_events(const RoomID& room_id) const;
    std::vector<EventPtr> get_room_events_since(const RoomID& room_id, const std::string& since_token) const;

    std::vector<RoomPtr> get_joined_rooms(const UserID& user_id) const;
    std::vector<RoomPtr> get_invited_rooms(const UserID& user_id) const;

    bool user_can_send_event(const RoomID& room_id, const UserID& user_id, const std::string& event_type) const;
    bool user_can_invite(const RoomID& room_id, const UserID& user_id) const;
    bool user_can_ban(const RoomID& room_id, const UserID& user_id) const;
    bool user_can_kick(const RoomID& room_id, const UserID& user_id) const;
    bool user_can_redact(const RoomID& room_id, const UserID& user_id) const;

    PowerLevels get_room_power_levels(const RoomID& room_id) const;
    void set_room_power_levels(const RoomID& room_id, const PowerLevels& levels);

    std::string resolve_room_alias(const std::string& room_alias) const;
    bool set_room_alias(const RoomID& room_id, const std::string& room_alias);
    bool remove_room_alias(const std::string& room_alias);

    nlohmann::json get_room_summary(const RoomID& room_id) const;
    nlohmann::json get_public_rooms_list(int limit = 50, const std::string& since = "") const;

    void cleanup_old_events(const RoomID& room_id, int max_events = 10000);

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<RoomID, RoomPtr> rooms_;
    std::unordered_map<std::string, RoomID> room_aliases_;
    std::unordered_map<UserID, std::vector<RoomID>> user_rooms_;

    RoomPtr create_room_internal(const RoomID& room_id, const UserID& creator);
    void update_user_room_membership(const RoomID& room_id, const UserID& user_id, Membership membership);
    void remove_user_room_membership(const RoomID& room_id, const UserID& user_id);
};

}
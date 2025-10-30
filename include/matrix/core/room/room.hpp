#pragma once

#include "../event/event.hpp"
#include "../matrix_types.hpp"
#include <unordered_map>
#include <vector>
#include <memory>

namespace matrix {

class RoomState;

class Room {
public:
    Room(const RoomID& room_id, const UserID& creator);
    ~Room() = default;

    RoomID room_id() const { return room_id_; }
    UserID creator() const { return creator_; }
    std::string name() const { return name_; }
    std::string topic() const { return topic_; }
    std::string avatar_url() const { return avatar_url_; }
    std::string version() const { return version_; }
    bool is_encrypted() const { return is_encrypted_; }
    bool is_public() const { return is_public_; }
    int num_joined_members() const { return num_joined_members_; }
    std::string join_rules() const { return join_rules_; }
    std::string guest_access() const { return guest_access_; }
    std::string history_visibility() const { return history_visibility_; }

    void set_name(const std::string& name) { name_ = name; }
    void set_topic(const std::string& topic) { topic_ = topic; }
    void set_avatar_url(const std::string& avatar_url) { avatar_url_ = avatar_url; }
    void set_version(const std::string& version) { version_ = version; }
    void set_encrypted(bool encrypted) { is_encrypted_ = encrypted; }
    void set_public(bool is_public) { is_public_ = is_public; }
    void set_num_joined_members(int count) { num_joined_members_ = count; }
    void set_join_rules(const std::string& rules) { join_rules_ = rules; }
    void set_guest_access(const std::string& access) { guest_access_ = access; }
    void set_history_visibility(const std::string& visibility) { history_visibility_ = visibility; }

    bool add_member(const UserID& user_id, Membership membership);
    bool remove_member(const UserID& user_id);
    Membership get_membership(const UserID& user_id) const;
    std::vector<UserID> get_members(Membership membership) const;
    std::vector<UserID> get_joined_members() const;
    std::vector<UserID> get_invited_members() const;

    bool add_event(const EventPtr& event);
    EventPtr get_event(const EventID& event_id) const;
    std::vector<EventPtr> get_events() const;
    std::vector<EventPtr> get_events_since(const std::string& since_token) const;

    PowerLevels power_levels() const { return power_levels_; }
    void set_power_levels(const PowerLevels& levels) { power_levels_ = levels; }

    int get_user_power_level(const UserID& user_id) const;
    bool user_can_send_event(const UserID& user_id, const std::string& event_type) const;
    bool user_can_invite(const UserID& user_id) const;
    bool user_can_ban(const UserID& user_id) const;
    bool user_can_kick(const UserID& user_id) const;
    bool user_can_redact(const UserID& user_id) const;

    std::shared_ptr<RoomState> current_state() const;
    void update_state(const EventPtr& state_event);

    std::string get_prev_batch() const { return prev_batch_; }
    void set_prev_batch(const std::string& batch) { prev_batch_ = batch; }

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);

private:
    RoomID room_id_;
    UserID creator_;
    std::string name_;
    std::string topic_;
    std::string avatar_url_;
    std::string version_ = "1";
    bool is_encrypted_ = false;
    bool is_public_ = false;
    int num_joined_members_ = 0;
    std::string join_rules_ = "invite";
    std::string guest_access_ = "forbidden";
    std::string history_visibility_ = "shared";
    std::string prev_batch_;

    PowerLevels power_levels_;
    std::unordered_map<UserID, Membership> members_;
    std::unordered_map<EventID, EventPtr> events_;
    std::vector<EventID> event_order_;

    std::shared_ptr<RoomState> current_state_;

    void update_member_count();
};

using RoomPtr = std::shared_ptr<Room>;

}
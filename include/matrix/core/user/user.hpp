#pragma once

#include "../matrix_types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace matrix {

class User {
public:
    User(const UserID& user_id);
    ~User() = default;

    UserID user_id() const { return user_id_; }
    std::string display_name() const { return display_name_; }
    std::string avatar_url() const { return avatar_url_; }
    bool is_admin() const { return is_admin_; }
    bool is_deactivated() const { return is_deactivated_; }
    bool is_shadow_banned() const { return is_shadow_banned_; }
    Timestamp created_ts() const { return created_ts_; }
    Timestamp updated_ts() const { return updated_ts_; }

    void set_display_name(const std::string& display_name) { display_name_ = display_name; }
    void set_avatar_url(const std::string& avatar_url) { avatar_url_ = avatar_url; }
    void set_admin(bool admin) { is_admin_ = admin; }
    void set_deactivated(bool deactivated) { is_deactivated_ = deactivated; }
    void set_shadow_banned(bool shadow_banned) { is_shadow_banned_ = shadow_banned; }

    std::vector<DeviceID> devices() const;
    bool has_device(const DeviceID& device_id) const;
    void add_device(const DeviceID& device_id);
    void remove_device(const DeviceID& device_id);

    std::vector<RoomID> joined_rooms() const;
    std::vector<RoomID> invited_rooms() const;
    std::vector<RoomID> left_rooms() const;

    void add_room(const RoomID& room_id, Membership membership);
    void remove_room(const RoomID& room_id);
    Membership get_room_membership(const RoomID& room_id) const;

    PresenceState presence() const { return presence_; }
    std::string status_msg() const { return status_msg_; }
    int last_active_ago() const { return last_active_ago_; }
    bool currently_active() const { return currently_active_; }

    void set_presence(PresenceState presence) { presence_ = presence; }
    void set_status_msg(const std::string& status_msg) { status_msg_ = status_msg; }
    void set_last_active_ago(int ago) { last_active_ago_ = ago; }
    void set_currently_active(bool active) { currently_active_ = active; }

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);

    bool validate() const;

private:
    UserID user_id_;
    std::string display_name_;
    std::string avatar_url_;
    bool is_admin_ = false;
    bool is_deactivated_ = false;
    bool is_shadow_banned_ = false;
    Timestamp created_ts_;
    Timestamp updated_ts_;

    PresenceState presence_ = PresenceState::OFFLINE;
    std::string status_msg_;
    int last_active_ago_ = 0;
    bool currently_active_ = false;

    std::vector<DeviceID> devices_;
    std::unordered_map<RoomID, Membership> room_memberships_;
};

using UserPtr = std::shared_ptr<User>;

}
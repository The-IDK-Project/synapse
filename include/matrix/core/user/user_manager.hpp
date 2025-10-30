#pragma once

#include "user.hpp"
#include "device.hpp"
#include "../matrix_types.hpp"
#include <unordered_map>
#include <memory>
#include <shared_mutex>

namespace matrix {

class UserManager {
public:
    UserManager();
    ~UserManager() = default;

    UserPtr create_user(const UserID& user_id, const std::string& display_name = "");
    UserPtr get_user(const UserID& user_id) const;
    bool has_user(const UserID& user_id) const;
    bool remove_user(const UserID& user_id);

    std::vector<UserPtr> get_users() const;
    std::vector<UserID> get_user_ids() const;

    bool set_display_name(const UserID& user_id, const std::string& display_name);
    bool set_avatar_url(const UserID& user_id, const std::string& avatar_url);
    bool set_admin(const UserID& user_id, bool admin);
    bool set_deactivated(const UserID& user_id, bool deactivated);
    bool set_shadow_banned(const UserID& user_id, bool shadow_banned);

    std::string get_display_name(const UserID& user_id) const;
    std::string get_avatar_url(const UserID& user_id) const;

    bool add_device_to_user(const UserID& user_id, const DeviceID& device_id, const DevicePtr& device);
    bool remove_device_from_user(const UserID& user_id, const DeviceID& device_id);
    DevicePtr get_user_device(const UserID& user_id, const DeviceID& device_id) const;
    std::vector<DevicePtr> get_user_devices(const UserID& user_id) const;
    std::vector<DeviceID> get_user_device_ids(const UserID& user_id) const;

    bool update_user_presence(const UserID& user_id, PresenceState presence, const std::string& status_msg = "", bool currently_active = false);
    PresenceState get_user_presence(const UserID& user_id) const;
    std::string get_user_status_msg(const UserID& user_id) const;

    bool add_user_to_room(const UserID& user_id, const RoomID& room_id, Membership membership);
    bool remove_user_from_room(const UserID& user_id, const RoomID& room_id);
    Membership get_user_room_membership(const UserID& user_id, const RoomID& room_id) const;
    std::vector<RoomID> get_user_rooms(const UserID& user_id, Membership membership = Membership::JOIN) const;
    std::vector<UserID> get_room_users(const RoomID& room_id, Membership membership = Membership::JOIN) const;

    std::vector<UserID> search_users(const std::string& query, int limit = 10) const;

    bool validate_user_id(const UserID& user_id) const;

    nlohmann::json get_user_profile(const UserID& user_id) const;
    nlohmann::json get_user_presence_status(const UserID& user_id) const;

    void cleanup_inactive_users(int max_inactive_days = 30);

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<UserID, UserPtr> users_;
    std::unordered_map<UserID, std::unordered_map<DeviceID, DevicePtr>> user_devices_;
    std::unordered_map<RoomID, std::unordered_map<UserID, Membership>> room_users_;

    UserPtr create_user_internal(const UserID& user_id);
    void update_user_room_membership(const UserID& user_id, const RoomID& room_id, Membership membership);
    void remove_user_room_membership(const UserID& user_id, const RoomID& room_id);
};

}
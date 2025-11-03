#pragma once

#include "repository.hpp"
#include "../../core/user/user.hpp"
#include <vector>
#include <memory>

namespace matrix::storage::repository {

class UserRepository : public CrudRepository<core::User> {
public:
    UserRepository() = default;
    virtual ~UserRepository() = default;

    virtual std::unique_ptr<core::User> read_by_display_name(const std::string& display_name) = 0;
    virtual std::vector<std::unique_ptr<core::User>> read_all_users(int limit = 100, const std::string& since_token = "") = 0;
    virtual std::vector<std::unique_ptr<core::User>> search_users(const std::string& query, int limit = 100) = 0;

    virtual bool set_display_name(const core::UserID& user_id, const std::string& display_name) = 0;
    virtual bool set_avatar_url(const core::UserID& user_id, const std::string& avatar_url) = 0;
    virtual bool set_admin_status(const core::UserID& user_id, bool is_admin) = 0;
    virtual bool set_deactivated_status(const core::UserID& user_id, bool deactivated) = 0;
    virtual bool set_shadow_banned_status(const core::UserID& user_id, bool shadow_banned) = 0;

    virtual std::string get_display_name(const core::UserID& user_id) = 0;
    virtual std::string get_avatar_url(const core::UserID& user_id) = 0;
    virtual bool is_admin(const core::UserID& user_id) = 0;
    virtual bool is_deactivated(const core::UserID& user_id) = 0;
    virtual bool is_shadow_banned(const core::UserID& user_id) = 0;

    virtual bool set_presence(const core::UserID& user_id, core::PresenceState presence, const std::string& status_msg = "") = 0;
    virtual core::PresenceState get_presence(const core::UserID& user_id) = 0;
    virtual std::string get_presence_status_msg(const core::UserID& user_id) = 0;
    virtual bool update_last_active(const core::UserID& user_id) = 0;
    virtual int64_t get_last_active_ts(const core::UserID& user_id) = 0;

    virtual bool set_account_data(const core::UserID& user_id, const std::string& type, const nlohmann::json& data) = 0;
    virtual bool set_room_account_data(const core::UserID& user_id, const core::RoomID& room_id, const std::string& type, const nlohmann::json& data) = 0;
    virtual std::unique_ptr<nlohmann::json> get_account_data(const core::UserID& user_id, const std::string& type) = 0;
    virtual std::unique_ptr<nlohmann::json> get_room_account_data(const core::UserID& user_id, const core::RoomID& room_id, const std::string& type) = 0;
    virtual std::vector<std::string> get_account_data_types(const core::UserID& user_id) = 0;
    virtual std::vector<std::string> get_room_account_data_types(const core::UserID& user_id, const core::RoomID& room_id) = 0;

    virtual bool set_push_rules(const core::UserID& user_id, const nlohmann::json& push_rules) = 0;
    virtual std::unique_ptr<nlohmann::json> get_push_rules(const core::UserID& user_id) = 0;

    virtual bool set_filter(const core::UserID& user_id, const std::string& filter_id, const nlohmann::json& filter) = 0;
    virtual std::unique_ptr<nlohmann::json> get_filter(const core::UserID& user_id, const std::string& filter_id) = 0;
    virtual bool delete_filter(const core::UserID& user_id, const std::string& filter_id) = 0;
    virtual std::vector<std::string> get_filter_ids(const core::UserID& user_id) = 0;

    virtual bool set_room_tag(const core::UserID& user_id, const core::RoomID& room_id, const std::string& tag, const nlohmann::json& data = {}) = 0;
    virtual bool remove_room_tag(const core::UserID& user_id, const core::RoomID& room_id, const std::string& tag) = 0;
    virtual std::unique_ptr<nlohmann::json> get_room_tags(const core::UserID& user_id, const core::RoomID& room_id) = 0;
    virtual std::vector<core::RoomID> get_rooms_by_tag(const core::UserID& user_id, const std::string& tag) = 0;

    virtual bool add_to_presence_list(const core::UserID& user_id, const core::UserID& target_user_id) = 0;
    virtual bool remove_from_presence_list(const core::UserID& user_id, const core::UserID& target_user_id) = 0;
    virtual std::vector<core::UserID> get_presence_list(const core::UserID& user_id) = 0;

    virtual int64_t get_total_user_count() = 0;
    virtual int64_t get_active_user_count(int64_t since_ts) = 0;
    virtual int64_t get_online_user_count() = 0;

    virtual bool cleanup_inactive_users(int64_t max_inactive_ts) = 0;
    virtual bool delete_user_data(const core::UserID& user_id) = 0;

    struct UserStats {
        core::UserID user_id;
        std::string display_name;
        core::PresenceState presence;
        std::string status_msg;
        int64_t last_active_ts;
        int64_t created_ts;
        bool is_admin;
        bool is_deactivated;
        int room_count;
        int device_count;
    };

    virtual std::unique_ptr<UserStats> get_user_stats(const core::UserID& user_id) = 0;
    virtual std::vector<UserStats> get_all_user_stats(int limit = 100, const std::string& since_token = "") = 0;
};

} //Вот и ожил Дед Максим
#pragma once

#include "../../common/api_types.hpp"
#include "../../common/response_builder.hpp"
#include "../../../core/user/user.hpp"
#include "../../../core/user/authentication.hpp"
#include <memory>

namespace matrix::api::client::v1 {

class UserHandler {
public:
    UserHandler(std::shared_ptr<core::UserManager> user_manager,
                std::shared_ptr<core::Authentication> auth_manager);
    ~UserHandler() = default;

    ApiResponse handle_get_profile(const ApiRequest& request);
    ApiResponse handle_set_profile(const ApiRequest& request);
    ApiResponse handle_get_avatar_url(const ApiRequest& request);
    ApiResponse handle_set_avatar_url(const ApiRequest& request);
    ApiResponse handle_get_display_name(const ApiRequest& request);
    ApiResponse handle_set_display_name(const ApiRequest& request);
    ApiResponse handle_get_account_data(const ApiRequest& request);
    ApiResponse handle_set_account_data(const ApiRequest& request);
    ApiResponse handle_get_room_account_data(const ApiRequest& request);
    ApiResponse handle_set_room_account_data(const ApiRequest& request);
    ApiResponse handle_get_presence(const ApiRequest& request);
    ApiResponse handle_set_presence(const ApiRequest& request);
    ApiResponse handle_get_presence_list(const ApiRequest& request);
    ApiResponse handle_change_password(const ApiRequest& request);
    ApiResponse handle_deactivate_account(const ApiRequest& request);
    ApiResponse handle_query_user_directory(const ApiRequest& request);
    ApiResponse handle_get_devices(const ApiRequest& request);
    ApiResponse handle_get_device(const ApiRequest& request);
    ApiResponse handle_update_device(const ApiRequest& request);
    ApiResponse handle_delete_device(const ApiRequest& request);
    ApiResponse handle_delete_devices(const ApiRequest& request);

private:
    bool validate_display_name(const std::string& display_name);
    bool validate_avatar_url(const std::string& avatar_url);
    nlohmann::json build_profile_response(const UserID& user_id);
    nlohmann::json build_display_name_response(const UserID& user_id);
    nlohmann::json build_avatar_url_response(const UserID& user_id);
    bool validate_account_data_type(const std::string& type);
    bool validate_account_data_content(const nlohmann::json& content);
    nlohmann::json get_user_account_data(const UserID& user_id, const std::string& type);
    nlohmann::json get_room_account_data(const UserID& user_id, const RoomID& room_id, const std::string& type);
    bool validate_presence_state(const std::string& presence);
    bool validate_status_message(const std::string& status_msg);
    nlohmann::json build_presence_response(const UserID& user_id);
    nlohmann::json build_presence_list_response(const UserID& user_id);
    void update_user_last_active(const UserID& user_id);
    bool validate_password_strength(const std::string& password);
    bool verify_current_password(const UserID& user_id, const std::string& current_password);
    bool can_deactivate_account(const UserID& user_id);
    void cleanup_user_data(const UserID& user_id);
    void notify_contacts_of_deactivation(const UserID& user_id);
    nlohmann::json build_user_directory_response(const std::string& search_term, int limit);
    bool should_include_user_in_directory(const UserID& user_id, const std::string& search_term);
    std::vector<UserID> search_users_by_display_name(const std::string& search_term, int limit);
    std::vector<UserID> search_users_by_user_id(const std::string& search_term, int limit);
    nlohmann::json build_devices_response(const UserID& user_id);
    nlohmann::json build_device_response(const UserID& user_id, const DeviceID& device_id);
    bool validate_device_display_name(const std::string& display_name);
    void cleanup_device_data(const UserID& user_id, const DeviceID& device_id);
    ApiResponse build_user_not_found_error(const UserID& user_id);
    ApiResponse build_device_not_found_error(const DeviceID& device_id);
    ApiResponse build_profile_update_error(const std::string& field);
    ApiResponse build_presence_update_error(const std::string& reason);
    ApiResponse build_password_change_error(const std::string& reason);
    ApiResponse build_account_deactivation_error(const std::string& reason);
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::Authentication> auth_manager_;
    std::unordered_map<UserID, nlohmann::json> user_account_data_;
    std::unordered_map<UserID, std::unordered_map<RoomID, nlohmann::json>> user_room_account_data_;
    std::unordered_map<UserID, std::vector<UserID>> user_presence_list_;
    std::unordered_map<UserID, std::unordered_map<std::string, int>> user_search_cache_;
};

}
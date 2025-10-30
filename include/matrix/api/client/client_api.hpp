#pragma once

#include "../common/api_types.hpp"
#include "../common/response_builder.hpp"
#include "../../core/event/event.hpp"
#include "../../core/room/room.hpp"
#include "../../core/user/user.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::api::client {

class ClientApi {
public:
    ClientApi();
    ~ClientApi();

    ApiResponse login(const ApiRequest& request);
    ApiResponse register_user(const ApiRequest& request);
    ApiResponse logout(const ApiRequest& request);
    ApiResponse logout_all(const ApiRequest& request);
    ApiResponse whoami(const ApiRequest& request);
    ApiResponse get_capabilities(const ApiRequest& request);
    ApiResponse create_room(const ApiRequest& request);
    ApiResponse join_room(const ApiRequest& request);
    ApiResponse leave_room(const ApiRequest& request);
    ApiResponse invite_to_room(const ApiRequest& request);
    ApiResponse kick_from_room(const ApiRequest& request);
    ApiResponse ban_user(const ApiRequest& request);
    ApiResponse unban_user(const ApiRequest& request);
    ApiResponse get_room_members(const ApiRequest& request);
    ApiResponse get_room_state(const ApiRequest& request);
    ApiResponse get_room_state_event(const ApiRequest& request);
    ApiResponse set_room_state_event(const ApiRequest& request);
    ApiResponse send_message(const ApiRequest& request);
    ApiResponse redact_event(const ApiRequest& request);
    ApiResponse get_room_timeline(const ApiRequest& request);
    ApiResponse get_room_summary(const ApiRequest& request);
    ApiResponse get_room_hierarchy(const ApiRequest& request);
    ApiResponse sync(const ApiRequest& request);
    ApiResponse get_profile(const ApiRequest& request);
    ApiResponse set_profile(const ApiRequest& request);
    ApiResponse get_avatar_url(const ApiRequest& request);
    ApiResponse set_avatar_url(const ApiRequest& request);
    ApiResponse get_display_name(const ApiRequest& request);
    ApiResponse set_display_name(const ApiRequest& request);
    ApiResponse get_account_data(const ApiRequest& request);
    ApiResponse set_account_data(const ApiRequest& request);
    ApiResponse get_room_account_data(const ApiRequest& request);
    ApiResponse set_room_account_data(const ApiRequest& request);
    ApiResponse get_presence(const ApiRequest& request);
    ApiResponse set_presence(const ApiRequest& request);
    ApiResponse get_presence_list(const ApiRequest& request);
    ApiResponse update_presence_list(const ApiRequest& request);
    ApiResponse set_typing(const ApiRequest& request);
    ApiResponse send_receipt(const ApiRequest& request);
    ApiResponse get_devices(const ApiRequest& request);
    ApiResponse get_device(const ApiRequest& request);
    ApiResponse update_device(const ApiRequest& request);
    ApiResponse delete_device(const ApiRequest& request);
    ApiResponse delete_devices(const ApiRequest& request);
    ApiResponse upload_keys(const ApiRequest& request);
    ApiResponse query_keys(const ApiRequest& request);
    ApiResponse claim_keys(const ApiRequest& request);
    ApiResponse get_key_changes(const ApiRequest& request);
    ApiResponse get_push_rules(const ApiRequest& request);
    ApiResponse set_push_rules(const ApiRequest& request);
    ApiResponse enable_push_rules(const ApiRequest& request);
    ApiResponse get_push_notifications(const ApiRequest& request);
    ApiResponse upload_media(const ApiRequest& request);
    ApiResponse download_media(const ApiRequest& request);
    ApiResponse get_media_preview(const ApiRequest& request);
    ApiResponse get_media_thumbnail(const ApiRequest& request);
    ApiResponse get_public_rooms(const ApiRequest& request);
    ApiResponse query_public_rooms(const ApiRequest& request);
    ApiResponse set_room_visibility(const ApiRequest& request);
    ApiResponse set_room_alias(const ApiRequest& request);
    ApiResponse remove_room_alias(const ApiRequest& request);
    ApiResponse resolve_room_alias(const ApiRequest& request);
    ApiResponse query_user_directory(const ApiRequest& request);
    ApiResponse get_versions(const ApiRequest& request);
    ApiResponse get_features(const ApiRequest& request);
    ApiResponse create_filter(const ApiRequest& request);
    ApiResponse get_filter(const ApiRequest& request);
    ApiResponse get_tags(const ApiRequest& request);
    ApiResponse set_tag(const ApiRequest& request);
    ApiResponse remove_tag(const ApiRequest& request);
    ApiResponse get_relations(const ApiRequest& request);

private:
    bool validate_access_token(const ApiRequest& request);
    UserID get_user_from_token(const std::string& access_token);
    DeviceID get_device_from_token(const std::string& access_token);

    bool user_can_send_event(const RoomID& room_id, const UserID& user_id, const std::string& event_type);
    bool user_can_invite(const RoomID& room_id, const UserID& user_id);
    bool user_can_ban(const RoomID& room_id, const UserID& user_id);
    bool user_can_kick(const RoomID& room_id, const UserID& user_id);
    bool user_can_redact(const RoomID& room_id, const UserID& user_id);
    bool user_is_in_room(const RoomID& room_id, const UserID& user_id);

    std::shared_ptr<core::Event> parse_event(const nlohmann::json& data, const RoomID& room_id, const UserID& sender);
    bool validate_event(const std::shared_ptr<core::Event>& event);
    std::string generate_event_id();

    nlohmann::json build_sync_response(const UserID& user_id, const SyncParams& params);
    nlohmann::json build_room_sync(const RoomID& room_id, const UserID& user_id, bool full_state);
    nlohmann::json build_timeline_events(const RoomID& room_id, const std::string& since_token);
    nlohmann::json build_state_events(const RoomID& room_id);
    nlohmann::json build_ephemeral_events(const RoomID& room_id, const UserID& user_id);
    nlohmann::json build_account_data(const UserID& user_id);
    nlohmann::json build_presence_data(const UserID& user_id);
    nlohmann::json build_to_device_events(const UserID& user_id);

    bool apply_filter(const nlohmann::json& filter, const std::shared_ptr<core::Event>& event);
    nlohmann::json load_filter(const std::string& filter_id, const UserID& user_id);
    std::string save_filter(const nlohmann::json& filter, const UserID& user_id);

    std::string generate_pagination_token(const std::vector<std::string>& events);
    std::vector<std::string> parse_pagination_token(const std::string& token);
    nlohmann::json paginate_events(const std::vector<std::shared_ptr<core::Event>>& events,
                                  const PaginationParams& params);

    std::string generate_media_id();
    bool validate_media_type(const std::string& media_type);
    bool validate_media_size(size_t size);

    bool check_rate_limit(const UserID& user_id, const std::string& endpoint);
    void update_rate_limit(const UserID& user_id, const std::string& endpoint);

    ApiResponse build_room_error(const RoomID& room_id, const std::string& error_code, const std::string& message);
    ApiResponse build_user_error(const UserID& user_id, const std::string& error_code, const std::string& message);
    ApiResponse build_permission_error(const std::string& message = "Insufficient permissions");

    std::unordered_map<std::string, UserID> token_to_user_;
    std::unordered_map<std::string, DeviceID> token_to_device_;
    std::unordered_map<UserID, std::unordered_map<std::string, nlohmann::json>> user_filters_;
    std::unordered_map<UserID, std::unordered_map<std::string, int>> user_rate_limits_;

    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;
    std::shared_ptr<core::Authentication> auth_manager_;
};

}
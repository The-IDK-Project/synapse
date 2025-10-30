#pragma once

#include "../../common/api_types.hpp"
#include "../../common/response_builder.hpp"
#include "../../../core/event/event.hpp"
#include "../../../core/room/room.hpp"
#include "../../../core/user/user.hpp"
#include <memory>

namespace matrix::api::client::v1 {

class ClientApiV1 {
public:
    ClientApiV1();
    ~ClientApiV1();

    ApiResponse login(const ApiRequest& request);
    ApiResponse register_user(const ApiRequest& request);
    ApiResponse logout(const ApiRequest& request);
    ApiResponse logout_all(const ApiRequest& request);
    ApiResponse whoami(const ApiRequest& request);
    ApiResponse change_password(const ApiRequest& request);
    ApiResponse deactivate_account(const ApiRequest& request);
    ApiResponse sync(const ApiRequest& request);
    ApiResponse events(const ApiRequest& request);
    ApiResponse initial_sync(const ApiRequest& request);
    ApiResponse create_room(const ApiRequest& request);
    ApiResponse get_room_aliases(const ApiRequest& request);
    ApiResponse get_room_id(const ApiRequest& request);
    ApiResponse get_public_rooms(const ApiRequest& request);
    ApiResponse join_room(const ApiRequest& request);
    ApiResponse join_room_by_id(const ApiRequest& request);
    ApiResponse leave_room(const ApiRequest& request);
    ApiResponse forget_room(const ApiRequest& request);
    ApiResponse invite_to_room(const ApiRequest& request);
    ApiResponse kick_from_room(const ApiRequest& request);
    ApiResponse ban_user(const ApiRequest& request);
    ApiResponse unban_user(const ApiRequest& request);
    ApiResponse get_room_members(const ApiRequest& request);
    ApiResponse get_joined_members(const ApiRequest& request);
    ApiResponse get_member_events(const ApiRequest& request);
    ApiResponse get_room_state(const ApiRequest& request);
    ApiResponse get_room_state_event(const ApiRequest& request);
    ApiResponse set_room_state_event(const ApiRequest& request);
    ApiResponse send_message(const ApiRequest& request);
    ApiResponse send_state_event(const ApiRequest& request);
    ApiResponse redact_event(const ApiRequest& request);
    ApiResponse get_room_messages(const ApiRequest& request);
    ApiResponse get_context(const ApiRequest& request);
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
    ApiResponse get_receipts(const ApiRequest& request);
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
    ApiResponse set_room_visibility(const ApiRequest& request);
    ApiResponse set_room_alias(const ApiRequest& request);
    ApiResponse remove_room_alias(const ApiRequest& request);
    ApiResponse resolve_room_alias(const ApiRequest& request);
    ApiResponse query_user_directory(const ApiRequest& request);
    ApiResponse get_versions(const ApiRequest& request);
    ApiResponse create_filter(const ApiRequest& request);
    ApiResponse get_filter(const ApiRequest& request);
    ApiResponse get_tags(const ApiRequest& request);
    ApiResponse set_tag(const ApiRequest& request);
    ApiResponse remove_tag(const ApiRequest& request);

private:
    bool validate_access_token_v1(const ApiRequest& request);
    UserID get_user_from_token_v1(const std::string& access_token);
    DeviceID get_device_from_token_v1(const std::string& access_token);
    bool user_can_send_event_v1(const RoomID& room_id, const UserID& user_id, const std::string& event_type);
    bool user_can_invite_v1(const RoomID& room_id, const UserID& user_id);
    bool user_can_ban_v1(const RoomID& room_id, const UserID& user_id);
    bool user_can_kick_v1(const RoomID& room_id, const UserID& user_id);
    bool user_can_redact_v1(const RoomID& room_id, const UserID& user_id);
    bool user_is_in_room_v1(const RoomID& room_id, const UserID& user_id);
    std::shared_ptr<core::Event> parse_event_v1(const nlohmann::json& data, const RoomID& room_id, const UserID& sender);
    bool validate_event_v1(const std::shared_ptr<core::Event>& event);
    std::string generate_event_id_v1();
    nlohmann::json build_sync_response_v1(const UserID& user_id, const SyncParams& params);
    nlohmann::json build_room_sync_v1(const RoomID& room_id, const UserID& user_id, bool full_state);
    nlohmann::json build_timeline_events_v1(const RoomID& room_id, const std::string& since_token);
    nlohmann::json build_state_events_v1(const RoomID& room_id);
    nlohmann::json build_ephemeral_events_v1(const RoomID& room_id, const UserID& user_id);
    nlohmann::json build_account_data_v1(const UserID& user_id);
    nlohmann::json build_presence_data_v1(const UserID& user_id);
    nlohmann::json build_to_device_events_v1(const UserID& user_id);
    std::unordered_map<std::string, UserID> token_to_user_v1_;
    std::unordered_map<std::string, DeviceID> token_to_device_v1_;
    std::unordered_map<UserID, std::unordered_map<std::string, nlohmann::json>> user_filters_v1_;
    std::unordered_map<UserID, std::unordered_map<std::string, int>> user_rate_limits_v1_;
    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;
    std::shared_ptr<core::Authentication> auth_manager_;
};

}
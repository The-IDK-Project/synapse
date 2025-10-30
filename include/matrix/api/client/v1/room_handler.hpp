#pragma once

#include "../../common/api_types.hpp"
#include "../../common/response_builder.hpp"
#include "../../../core/event/event.hpp"
#include "../../../core/room/room.hpp"
#include "../../../core/user/user.hpp"
#include <memory>

namespace matrix::api::client::v1 {

class RoomHandler {
public:
    RoomHandler(std::shared_ptr<core::RoomManager> room_manager,
                std::shared_ptr<core::UserManager> user_manager,
                std::shared_ptr<core::StateManager> state_manager);
    ~RoomHandler() = default;
    ApiResponse handle_create_room(const ApiRequest& request);
    ApiResponse handle_join_room(const ApiRequest& request);
    ApiResponse handle_leave_room(const ApiRequest& request);
    ApiResponse handle_invite_to_room(const ApiRequest& request);
    ApiResponse handle_kick_from_room(const ApiRequest& request);
    ApiResponse handle_ban_user(const ApiRequest& request);
    ApiResponse handle_unban_user(const ApiRequest& request);
    ApiResponse handle_forget_room(const ApiRequest& request);
    ApiResponse handle_get_room_aliases(const ApiRequest& request);
    ApiResponse handle_get_room_id(const ApiRequest& request);
    ApiResponse handle_get_public_rooms(const ApiRequest& request);
    ApiResponse handle_get_room_members(const ApiRequest& request);
    ApiResponse handle_get_joined_members(const ApiRequest& request);
    ApiResponse handle_get_member_events(const ApiRequest& request);
    ApiResponse handle_get_room_state(const ApiRequest& request);
    ApiResponse handle_get_room_state_event(const ApiRequest& request);
    ApiResponse handle_set_room_state_event(const ApiRequest& request);
    ApiResponse handle_send_message(const ApiRequest& request);
    ApiResponse handle_send_state_event(const ApiRequest& request);
    ApiResponse handle_redact_event(const ApiRequest& request);
    ApiResponse handle_get_room_messages(const ApiRequest& request);
    ApiResponse handle_get_context(const ApiRequest& request);
    ApiResponse handle_set_room_visibility(const ApiRequest& request);
    ApiResponse handle_set_room_alias(const ApiRequest& request);
    ApiResponse handle_remove_room_alias(const ApiRequest& request);
    ApiResponse handle_resolve_room_alias(const ApiRequest& request);

private:
    std::shared_ptr<core::Room> create_room_from_request(const ApiRequest& request, const UserID& creator);
    bool validate_room_creation(const ApiRequest& request);
    nlohmann::json build_room_creation_content(const ApiRequest& request);
    bool can_user_join_room(const RoomID& room_id, const UserID& user_id);
    bool can_user_invite_to_room(const RoomID& room_id, const UserID& inviter, const UserID& invitee);
    bool can_user_kick_from_room(const RoomID& room_id, const UserID& kicker, const UserID& kickee);
    bool can_user_ban_from_room(const RoomID& room_id, const UserID& banner, const UserID& bannee);
    bool can_user_leave_room(const RoomID& room_id, const UserID& user_id);
    std::shared_ptr<core::Event> create_message_event_from_request(const ApiRequest& request, const RoomID& room_id, const UserID& sender);
    std::shared_ptr<core::Event> create_state_event_from_request(const ApiRequest& request, const RoomID& room_id, const UserID& sender);
    bool validate_event_for_room(const std::shared_ptr<core::Event>& event, const RoomID& room_id, const UserID& sender);
    std::string generate_transaction_id();
    nlohmann::json build_room_messages_response(const RoomID& room_id, const std::string& from_token,
                                               const std::string& to_token, const std::string& direction,
                                               int limit, const UserID& user_id);
    nlohmann::json build_context_response(const RoomID& room_id, const EventID& event_id,
                                         int limit_before, int limit_after, const UserID& user_id);
    nlohmann::json build_room_state_response(const RoomID& room_id);
    nlohmann::json build_room_state_event_response(const RoomID& room_id, const std::string& event_type,
                                                  const std::string& state_key);
    bool validate_state_event_content(const std::string& event_type, const nlohmann::json& content);
    nlohmann::json build_public_rooms_response(int limit, const std::string& since_token,
                                              const std::string& filter, const UserID& user_id);
    bool should_include_room_in_public_list(const std::shared_ptr<core::Room>& room, const std::string& filter);
    bool validate_room_alias(const std::string& alias);
    std::string extract_localpart_from_alias(const std::string& alias);
    std::string generate_room_alias(const std::string& localpart, const std::string& server_name);
    ApiResponse build_room_not_found_error(const RoomID& room_id);
    ApiResponse build_user_not_in_room_error(const RoomID& room_id, const UserID& user_id);
    ApiResponse build_permission_denied_error(const std::string& action, const RoomID& room_id, const UserID& user_id);
    ApiResponse build_event_not_found_error(const EventID& event_id);
    ApiResponse build_alias_already_exists_error(const std::string& alias);
    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;
    std::unordered_map<std::string, std::string> room_aliases_;
    std::unordered_map<std::string, std::vector<std::string>> alias_to_rooms_;
    std::unordered_map<UserID, std::unordered_map<std::string, int>> user_transaction_ids_;
};

}
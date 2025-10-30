#pragma once

#include "../common/api_types.hpp"
#include "../common/response_builder.hpp"
#include "../../core/event/event.hpp"
#include "../../core/room/room.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::api::federation {

class FederationApi {
public:
    FederationApi();
    ~FederationApi();

    ApiResponse get_server_version(const ApiRequest& request);
    ApiResponse get_server_keys(const ApiRequest& request);
    ApiResponse get_server_key(const ApiRequest& request);
    ApiResponse make_join(const ApiRequest& request);
    ApiResponse send_join(const ApiRequest& request);
    ApiResponse make_leave(const ApiRequest& request);
    ApiResponse send_leave(const ApiRequest& request);
    ApiResponse make_invite(const ApiRequest& request);
    ApiResponse send_invite(const ApiRequest& request);
    ApiResponse get_room_state(const ApiRequest& request);
    ApiResponse get_room_state_ids(const ApiRequest& request);
    ApiResponse get_event(const ApiRequest& request);
    ApiResponse get_events(const ApiRequest& request);
    ApiResponse get_missing_events(const ApiRequest& request);
    ApiResponse backfill(const ApiRequest& request);
    ApiResponse get_backfill(const ApiRequest& request);
    ApiResponse get_user_devices(const ApiRequest& request);
    ApiResponse claim_user_keys(const ApiRequest& request);
    ApiResponse query_user_keys(const ApiRequest& request);
    ApiResponse send_transaction(const ApiRequest& request);
    ApiResponse get_transaction(const ApiRequest& request);
    ApiResponse get_public_rooms(const ApiRequest& request);
    ApiResponse query_public_rooms(const ApiRequest& request);
    ApiResponse query_profile(const ApiRequest& request);
    ApiResponse query_directory(const ApiRequest& request);
    ApiResponse get_space_hierarchy(const ApiRequest& request);
    ApiResponse get_space_children(const ApiRequest& request);

private:
    struct FederationConfig {
        std::string server_name;
        std::string signing_key;
        int timeout_ms = 30000;
        int max_retries = 3;
        bool verify_signatures = true;
        std::vector<std::string> trusted_servers;
    };
    bool validate_server_signature(const std::string& server_name, const nlohmann::json& data, const std::string& signature);
    nlohmann::json build_server_keys_response();
    nlohmann::json build_server_version_response();
    bool validate_room_membership(const RoomID& room_id, const UserID& user_id, const std::string& membership);
    bool can_federate_room(const RoomID& room_id);
    std::shared_ptr<core::Event> create_join_event(const RoomID& room_id, const UserID& user_id, const std::string& server_name);
    std::shared_ptr<core::Event> create_leave_event(const RoomID& room_id, const UserID& user_id, const std::string& server_name);
    std::shared_ptr<core::Event> create_invite_event(const RoomID& room_id, const UserID& sender, const UserID& target, const std::string& server_name);
    nlohmann::json build_room_state_response(const RoomID& room_id, const std::string& event_id = "");
    nlohmann::json build_room_state_ids_response(const RoomID& room_id, const std::string& event_id = "");
    bool validate_event_for_federation(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool verify_event_signature(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    std::string sign_event(const std::shared_ptr<core::Event>& event);
    nlohmann::json build_event_response(const EventID& event_id);
    nlohmann::json build_events_response(const std::vector<EventID>& event_ids);
    std::vector<std::shared_ptr<core::Event>> get_events_for_backfill(const RoomID& room_id, const std::vector<EventID>& event_ids, int limit);
    nlohmann::json build_backfill_response(const RoomID& room_id, const std::vector<EventID>& event_ids, int limit);
    bool validate_user_keys_query(const std::string& server_name, const std::vector<UserID>& user_ids);
    nlohmann::json build_user_devices_response(const UserID& user_id);
    nlohmann::json build_user_keys_response(const std::vector<UserID>& user_ids);
    nlohmann::json build_claimed_keys_response(const std::unordered_map<UserID, std::unordered_map<DeviceID, std::string>>& claims);
    bool validate_transaction(const nlohmann::json& transaction, const std::string& origin_server);
    bool process_transaction_events(const nlohmann::json& transaction, const std::string& origin_server);
    nlohmann::json build_transaction_response(const std::string& transaction_id);
    nlohmann::json build_federated_public_rooms_response(int limit, const std::string& since_token, const std::string& server_name);
    bool should_federate_room_publicly(const std::shared_ptr<core::Room>& room);
    nlohmann::json build_profile_response(const UserID& user_id);
    nlohmann::json build_directory_response(const std::string& room_alias);
    nlohmann::json build_space_hierarchy_response(const RoomID& space_id, const std::string& suggested_only, int max_depth);
    nlohmann::json build_space_children_response(const RoomID& space_id);
    ApiResponse build_federation_error(const std::string& error_code, const std::string& message, const std::string& server_name = "");
    ApiResponse build_signature_verification_error(const std::string& server_name);
    ApiResponse build_room_not_found_error(const RoomID& room_id);
    ApiResponse build_event_not_found_error(const EventID& event_id);
    ApiResponse build_user_not_found_error(const UserID& user_id);
    ApiResponse build_transaction_error(const std::string& transaction_id, const std::string& reason);

    FederationConfig config_;
    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;

    std::unordered_map<std::string, nlohmann::json> server_keys_;
    std::unordered_map<std::string, std::vector<nlohmann::json>> pending_transactions_;
    std::unordered_map<std::string, Timestamp> last_transaction_time_;
};

}
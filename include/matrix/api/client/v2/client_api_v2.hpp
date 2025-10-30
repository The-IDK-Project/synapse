#pragma once

#include "../../common/api_types.hpp"
#include "../../common/response_builder.hpp"
#include "../../../core/event/event.hpp"
#include "../../../core/room/room.hpp"
#include "../../../core/user/user.hpp"
#include <memory>

namespace matrix::api::client::v2 {

class ClientApiV2 {
public:
    ClientApiV2();
    ~ClientApiV2();

    ApiResponse login(const ApiRequest& request);
    ApiResponse register_user(const ApiRequest& request);
    ApiResponse logout(const ApiRequest& request);
    ApiResponse logout_all(const ApiRequest& request);
    ApiResponse whoami(const ApiRequest& request);
    ApiResponse get_capabilities(const ApiRequest& request);
    ApiResponse create_room(const ApiRequest& request);
    ApiResponse get_room_hierarchy(const ApiRequest& request);
    ApiResponse upgrade_room(const ApiRequest& request);
    ApiResponse sync(const ApiRequest& request);
    ApiResponse get_profile(const ApiRequest& request);
    ApiResponse set_profile(const ApiRequest& request);
    ApiResponse get_3pid(const ApiRequest& request);
    ApiResponse add_3pid(const ApiRequest& request);
    ApiResponse delete_3pid(const ApiRequest& request);
    ApiResponse bind_3pid(const ApiRequest& request);
    ApiResponse request_token_3pid(const ApiRequest& request);
    ApiResponse get_account_data(const ApiRequest& request);
    ApiResponse set_account_data(const ApiRequest& request);
    ApiResponse request_openid_token(const ApiRequest& request);
    ApiResponse get_notifications(const ApiRequest& request);
    ApiResponse get_push_rules(const ApiRequest& request);
    ApiResponse set_push_rules(const ApiRequest& request);
    ApiResponse get_relations(const ApiRequest& request);
    ApiResponse get_relations_aggregated(const ApiRequest& request);
    ApiResponse send_relation(const ApiRequest& request);
    ApiResponse remove_relation(const ApiRequest& request);
    ApiResponse get_space_hierarchy(const ApiRequest& request);
    ApiResponse get_space_children(const ApiRequest& request);
    ApiResponse get_space_parents(const ApiRequest& request);
    ApiResponse get_threads(const ApiRequest& request);
    ApiResponse get_thread(const ApiRequest& request);
    ApiResponse get_thread_roots(const ApiRequest& request);
    ApiResponse get_room_versions(const ApiRequest& request);
    ApiResponse get_room_version(const ApiRequest& request);
    ApiResponse get_versions(const ApiRequest& request);
    ApiResponse get_features(const ApiRequest& request);

private:
    bool validate_access_token_v2(const ApiRequest& request);
    UserID get_user_from_token_v2(const std::string& access_token);
    DeviceID get_device_from_token_v2(const std::string& access_token);
    bool user_can_upgrade_room(const RoomID& room_id, const UserID& user_id);
    std::string get_latest_room_version();
    bool validate_room_upgrade(const RoomID& room_id, const std::string& new_version);
    nlohmann::json build_sync_response_v2(const UserID& user_id, const SyncParams& params);
    nlohmann::json build_threads_sync(const RoomID& room_id, const UserID& user_id);
    nlohmann::json build_relations_sync(const RoomID& room_id, const UserID& user_id);
    nlohmann::json build_spaces_sync(const UserID& user_id);
    bool validate_3pid_type(const std::string& type);
    bool validate_3pid_address(const std::string& type, const std::string& address);
    nlohmann::json build_3pid_response(const UserID& user_id);
    std::string generate_openid_token(const UserID& user_id);
    bool validate_openid_token(const std::string& token);
    nlohmann::json build_relations_response(const RoomID& room_id, const EventID& event_id,
                                          const std::string& relation_type, const std::string& event_type,
                                          int limit, const std::string& from_token);
    nlohmann::json build_aggregated_relations_response(const RoomID& room_id, const EventID& event_id,
                                                      const std::string& relation_type);
    nlohmann::json build_space_hierarchy_response(const RoomID& space_id, int max_depth,
                                                 const std::string& from_token, int limit);
    nlohmann::json build_space_children_response(const RoomID& space_id);
    nlohmann::json build_space_parents_response(const RoomID& room_id);

    nlohmann::json build_threads_response(const RoomID& room_id, const UserID& user_id,
                                         const std::string& from_token, int limit);
    nlohmann::json build_thread_response(const RoomID& room_id, const EventID& event_id,
                                        const UserID& user_id, const std::string& from_token, int limit);
    std::unordered_map<std::string, UserID> token_to_user_v2_;
    std::unordered_map<std::string, DeviceID> token_to_device_v2_;
    std::unordered_map<UserID, std::unordered_map<std::string, nlohmann::json>> user_3pids_;
    std::unordered_map<std::string, UserID> openid_tokens_;
    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;
    std::shared_ptr<core::Authentication> auth_manager_;
};

}
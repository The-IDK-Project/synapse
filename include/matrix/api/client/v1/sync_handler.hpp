#pragma once

#include "../../common/api_types.hpp"
#include "../../common/response_builder.hpp"
#include "../../../core/event/event.hpp"
#include "../../../core/room/room.hpp"
#include "../../../core/user/user.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::api::client::v1 {

class SyncHandler {
public:
    SyncHandler(std::shared_ptr<core::RoomManager> room_manager,
                std::shared_ptr<core::UserManager> user_manager,
                std::shared_ptr<core::StateManager> state_manager);
    ~SyncHandler() = default;

    ApiResponse handle_sync(const ApiRequest& request);
    ApiResponse handle_events(const ApiRequest& request);
    ApiResponse handle_initial_sync(const ApiRequest& request);

private:
    struct SyncState {
        std::string next_batch;
        std::unordered_map<RoomID, std::string> room_next_batch;
        std::unordered_map<RoomID, bool> room_limited;
        Timestamp last_sync_time;
    };

    nlohmann::json build_sync_response(const UserID& user_id, const SyncParams& params);
    nlohmann::json build_rooms_sync(const UserID& user_id, const SyncParams& params);
    nlohmann::json build_room_sync(const RoomID& room_id, const UserID& user_id, const SyncParams& params);
    nlohmann::json build_timeline_sync(const RoomID& room_id, const UserID& user_id, const std::string& since_token);
    nlohmann::json build_state_sync(const RoomID& room_id, const UserID& user_id, bool full_state);
    nlohmann::json build_ephemeral_sync(const RoomID& room_id, const UserID& user_id);
    nlohmann::json build_account_data_sync(const UserID& user_id);
    nlohmann::json build_presence_sync(const UserID& user_id);
    nlohmann::json build_to_device_sync(const UserID& user_id);

    nlohmann::json build_events_response(const UserID& user_id, const std::string& from_token, const std::string& direction, int limit);
    nlohmann::json build_initial_sync_response(const UserID& user_id);

    std::string generate_next_batch(const UserID& user_id);
    std::string generate_room_next_batch(const RoomID& room_id, const UserID& user_id);
    std::vector<std::string> parse_batch_token(const std::string& token);
    std::string create_batch_token(const std::vector<std::string>& components);

    bool should_include_room(const RoomID& room_id, const UserID& user_id, const SyncParams& params);
    bool should_include_event(const std::shared_ptr<core::Event>& event, const UserID& user_id, const nlohmann::json& filter);

    nlohmann::json apply_room_filter(const nlohmann::json& room_data, const nlohmann::json& filter);
    nlohmann::json apply_timeline_filter(const nlohmann::json& timeline_data, const nlohmann::json& filter);
    nlohmann::json apply_state_filter(const nlohmann::json& state_data, const nlohmann::json& filter);
    nlohmann::json apply_ephemeral_filter(const nlohmann::json& ephemeral_data, const nlohmann::json& filter);

    std::vector<std::shared_ptr<core::Event>> get_room_events_since(const RoomID& room_id, const std::string& since_token, int limit = 50);
    std::vector<std::shared_ptr<core::Event>> get_room_state_events(const RoomID& room_id);
    std::vector<nlohmann::json> get_room_ephemeral_events(const RoomID& room_id, const UserID& user_id);
    std::vector<nlohmann::json> get_user_account_data(const UserID& user_id);
    std::vector<nlohmann::json> get_user_presence_data(const UserID& user_id);
    std::vector<nlohmann::json> get_user_to_device_events(const UserID& user_id);

    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;

    std::unordered_map<UserID, SyncState> user_sync_states_;
    std::unordered_map<UserID, std::string> user_next_batch_;
    std::unordered_map<std::string, std::vector<nlohmann::json>> to_device_events_;
};

}
#pragma once

#include "../common/api_types.hpp"
#include "../../core/event/event.hpp"
#include "websocket_session.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::api::websocket {

class EventsWebSocket {
public:
    EventsWebSocket(std::shared_ptr<WebSocketSessionManager> session_manager,
                   std::shared_ptr<core::RoomManager> room_manager,
                   std::shared_ptr<core::UserManager> user_manager,
                   std::shared_ptr<core::StateManager> state_manager);
    ~EventsWebSocket();

    void on_room_event(const std::shared_ptr<core::Event>& event);
    void on_presence_update(const UserID& user_id, const nlohmann::json& presence);
    void on_typing_notification(const RoomID& room_id, const std::vector<UserID>& typing_users);
    void on_receipt_notification(const RoomID& room_id, const EventID& event_id,
                                const std::string& receipt_type, const nlohmann::json& receipt);
    void on_to_device_message(const UserID& user_id, const std::string& type, const nlohmann::json& content);
    void on_device_list_update(const UserID& user_id);
    void on_key_verification_request(const UserID& user_id, const nlohmann::json& request);

    void subscribe_to_room(const UserID& user_id, const RoomID& room_id);
    void unsubscribe_from_room(const UserID& user_id, const RoomID& room_id);
    void subscribe_to_presence(const UserID& user_id, const UserID& target_user_id);
    void unsubscribe_from_presence(const UserID& user_id, const UserID& target_user_id);

    std::vector<RoomID> get_subscribed_rooms(const UserID& user_id) const;
    std::vector<UserID> get_subscribed_presence(const UserID& user_id) const;
    std::vector<UserID> get_room_subscribers(const RoomID& room_id) const;
    std::vector<UserID> get_presence_subscribers(const UserID& user_id) const;

    void cleanup_subscriptions(const UserID& user_id);

private:
    struct SubscriptionManager {
        std::unordered_map<UserID, std::vector<RoomID>> user_room_subscriptions;
        std::unordered_map<UserID, std::vector<UserID>> user_presence_subscriptions;
        std::unordered_map<RoomID, std::vector<UserID>> room_subscribers;
        std::unordered_map<UserID, std::vector<UserID>> presence_subscribers;
        mutable std::shared_mutex mutex;
    };
    void distribute_room_event(const std::shared_ptr<core::Event>& event);
    void distribute_presence_update(const UserID& user_id, const nlohmann::json& presence);
    void distribute_typing_notification(const RoomID& room_id, const std::vector<UserID>& typing_users);
    void distribute_receipt_notification(const RoomID& room_id, const EventID& event_id,
                                        const std::string& receipt_type, const nlohmann::json& receipt);
    void distribute_to_device_message(const UserID& user_id, const std::string& type, const nlohmann::json& content);
    void distribute_device_list_update(const UserID& user_id);
    void distribute_key_verification_request(const UserID& user_id, const nlohmann::json& request);
    void add_room_subscription(const UserID& user_id, const RoomID& room_id);
    void remove_room_subscription(const UserID& user_id, const RoomID& room_id);
    void add_presence_subscription(const UserID& user_id, const UserID& target_user_id);
    void remove_presence_subscription(const UserID& user_id, const UserID& target_user_id);
    void cleanup_user_subscriptions(const UserID& user_id);
    bool should_send_event_to_user(const std::shared_ptr<core::Event>& event, const UserID& user_id);
    bool should_send_presence_to_user(const UserID& target_user_id, const UserID& subscriber_id);
    bool should_send_typing_to_user(const RoomID& room_id, const UserID& user_id);
    nlohmann::json filter_event_for_user(const std::shared_ptr<core::Event>& event, const UserID& user_id);
    nlohmann::json build_room_event_message(const std::shared_ptr<core::Event>& event);
    nlohmann::json build_presence_message(const UserID& user_id, const nlohmann::json& presence);
    nlohmann::json build_typing_message(const RoomID& room_id, const std::vector<UserID>& typing_users);
    nlohmann::json build_receipt_message(const RoomID& room_id, const EventID& event_id,
                                        const std::string& receipt_type, const nlohmann::json& receipt);
    nlohmann::json build_to_device_message(const std::string& type, const nlohmann::json& content);
    nlohmann::json build_device_list_update_message(const UserID& user_id);
    nlohmann::json build_key_verification_message(const nlohmann::json& request);
    void batch_events_for_user(const UserID& user_id, const std::vector<nlohmann::json>& events);
    void process_event_batches();
    void send_batched_events(const UserID& user_id, const std::vector<nlohmann::json>& events);
    bool check_rate_limit(const UserID& user_id, const std::string& event_type);
    void update_rate_limit(const UserID& user_id, const std::string& event_type);

    std::shared_ptr<WebSocketSessionManager> session_manager_;
    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;

    SubscriptionManager subscriptions_;

    std::unordered_map<UserID, std::vector<nlohmann::json>> event_batches_;
    std::unordered_map<UserID, std::unordered_map<std::string, int>> user_rate_limits_;
    std::unordered_map<UserID, Timestamp> last_batch_flush_;

    mutable std::shared_mutex batches_mutex_;
    std::thread batch_processor_;
    std::atomic<bool> processing_batches_{false};
    int batch_interval_ms_ = 100;
};

class WebSocketEventRouter {
public:
    WebSocketEventRouter(std::shared_ptr<EventsWebSocket> events_ws);
    ~WebSocketEventRouter();

    void route_room_event(const std::shared_ptr<core::Event>& event);
    void route_presence_update(const UserID& user_id, const nlohmann::json& presence);
    void route_typing_notification(const RoomID& room_id, const std::vector<UserID>& typing_users);
    void route_receipt_notification(const RoomID& room_id, const EventID& event_id,
                                   const std::string& receipt_type, const nlohmann::json& receipt);
    void route_to_device_message(const UserID& user_id, const std::string& type, const nlohmann::json& content);
    void route_device_list_update(const UserID& user_id);
    void route_key_verification_request(const UserID& user_id, const nlohmann::json& request);

    void register_event_handler(const std::string& event_type,
                               std::function<void(const std::shared_ptr<core::Event>&)> handler);
    void register_presence_handler(std::function<void(const UserID&, const nlohmann::json&)> handler);
    void register_typing_handler(std::function<void(const RoomID&, const std::vector<UserID>&)> handler);

private:
    std::shared_ptr<EventsWebSocket> events_ws_;

    std::unordered_map<std::string, std::vector<std::function<void(const std::shared_ptr<core::Event>&)>>> event_handlers_;
    std::vector<std::function<void(const UserID&, const nlohmann::json&)>> presence_handlers_;
    std::vector<std::function<void(const RoomID&, const std::vector<UserID>&)>> typing_handlers_;

    mutable std::shared_mutex handlers_mutex_;
};

}
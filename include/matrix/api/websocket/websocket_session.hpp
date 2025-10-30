#pragma once

#include "../common/api_types.hpp"
#include "../../core/event/event.hpp"
#include <memory>
#include <queue>
#include <atomic>

namespace matrix::api::websocket {

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    WebSocketSession(const std::string& session_id,
                    websocketpp::connection_hdl connection_hdl,
                    std::shared_ptr<core::UserManager> user_manager);
    ~WebSocketSession();

    std::string session_id() const { return session_id_; }
    UserID user_id() const { return user_id_; }
    bool is_authenticated() const { return authenticated_; }
    bool is_connected() const { return connected_; }
    Timestamp created_ts() const { return created_ts_; }
    Timestamp last_activity_ts() const { return last_activity_ts_; }

    void authenticate(const UserID& user_id, const std::string& access_token);
    void disconnect(websocketpp::close::status::value code = websocketpp::close::status::normal,
                   const std::string& reason = "");

    void send(const nlohmann::json& message);
    void send_event(const std::shared_ptr<core::Event>& event);
    void send_sync_data(const nlohmann::json& sync_data);
    void send_presence_update(const UserID& user_id, const nlohmann::json& presence);
    void send_typing_notification(const RoomID& room_id, const std::vector<UserID>& typing_users);
    void send_receipt_notification(const RoomID& room_id, const EventID& event_id,
                                  const std::string& receipt_type, const nlohmann::json& receipt);
    void send_to_device_message(const std::string& type, const nlohmann::json& content);

    void handle_message(const nlohmann::json& message);
    void update_activity();

    void set_connection_hdl(websocketpp::connection_hdl hdl) { connection_hdl_ = hdl; }
    websocketpp::connection_hdl connection_hdl() const { return connection_hdl_; }

    void start_ping_interval(int interval_seconds);
    void stop_ping_interval();

    nlohmann::json get_session_info() const;

private:
    struct MessageQueue {
        std::queue<nlohmann::json> messages;
        std::mutex mutex;
        std::condition_variable condition;
        bool stopped = false;
    };

    void process_message_queue();
    void send_queued_message(const nlohmann::json& message);
    void handle_sync_request(const nlohmann::json& request);
    void handle_presence_subscription(const nlohmann::json& subscription);
    void handle_typing_notification(const nlohmann::json& notification);
    void handle_receipt_notification(const nlohmann::json& receipt);
    void handle_to_device_message(const nlohmann::json& message);
    void handle_ping(const nlohmann::json& ping);
    void handle_pong(const nlohmann::json& pong);

    void send_error_response(const std::string& request_id, const std::string& error_code, const std::string& message);
    void send_success_response(const std::string& request_id, const nlohmann::json& data = {});

    bool validate_access_token(const std::string& access_token);
    bool has_permission(const std::string& action, const RoomID& room_id = "");
    void update_user_presence();

    void start_ping_timer();
    void send_ping();
    void handle_pong_timeout();

    std::string session_id_;
    websocketpp::connection_hdl connection_hdl_;
    UserID user_id_;
    std::string access_token_;
    bool authenticated_ = false;
    bool connected_ = true;
    Timestamp created_ts_;
    Timestamp last_activity_ts_;
    Timestamp last_ping_sent_;
    Timestamp last_pong_received_;

    std::shared_ptr<core::UserManager> user_manager_;
    std::unique_ptr<MessageQueue> message_queue_;
    std::thread message_processor_;
    std::atomic<bool> processing_messages_{false};

    std::vector<RoomID> subscribed_rooms_;
    std::vector<UserID> subscribed_presence_;
    std::unordered_map<std::string, Timestamp> pending_requests_;

    std::mutex session_mutex_;
    std::unique_ptr<std::thread> ping_thread_;
    std::atomic<bool> ping_enabled_{false};
    int ping_interval_ = 30;
};

class WebSocketSessionManager {
public:
    WebSocketSessionManager();
    ~WebSocketSessionManager();

    std::shared_ptr<WebSocketSession> create_session(const std::string& session_id,
                                                    websocketpp::connection_hdl connection_hdl,
                                                    std::shared_ptr<core::UserManager> user_manager);
    std::shared_ptr<WebSocketSession> get_session(const std::string& session_id) const;
    bool has_session(const std::string& session_id) const;
    void remove_session(const std::string& session_id);

    std::vector<std::shared_ptr<WebSocketSession>> get_user_sessions(const UserID& user_id) const;
    std::vector<std::shared_ptr<WebSocketSession>> get_all_sessions() const;
    std::vector<UserID> get_connected_users() const;

    void broadcast_to_user(const UserID& user_id, const nlohmann::json& message);
    void broadcast_to_all(const nlohmann::json& message);
    void broadcast_to_room(const RoomID& room_id, const nlohmann::json& message,
                          const std::vector<UserID>& exclude_users = {});

    void cleanup_inactive_sessions(int max_inactive_seconds = 300);
    void cleanup_disconnected_sessions();

    size_t get_session_count() const;
    size_t get_authenticated_session_count() const;
    size_t get_user_session_count(const UserID& user_id) const;

    nlohmann::json get_session_statistics() const;

private:
    mutable std::shared_mutex sessions_mutex_;
    std::unordered_map<std::string, std::shared_ptr<WebSocketSession>> sessions_;
    std::unordered_map<UserID, std::vector<std::string>> user_sessions_;

    std::string generate_session_id() const;
    void update_user_session_index(const std::string& session_id, const UserID& user_id);
    void remove_user_session_index(const std::string& session_id, const UserID& user_id);
};

}
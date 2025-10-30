#pragma once

#include "../common/api_types.hpp"
#include "../../core/event/event.hpp"
#include <memory>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace matrix::api::websocket {

class WebSocketSession;

class WebSocketHandler {
public:
    WebSocketHandler();
    ~WebSocketHandler();

    void start(int port = 8080);
    void stop();
    bool is_running() const;

    void broadcast_event(const std::shared_ptr<core::Event>& event, const std::vector<UserID>& users);
    void send_to_user(const UserID& user_id, const nlohmann::json& message);
    void send_to_session(const std::string& session_id, const nlohmann::json& message);

    void add_authenticated_session(const std::string& session_id, const UserID& user_id, const std::shared_ptr<WebSocketSession>& session);
    void remove_session(const std::string& session_id);
    bool is_user_connected(const UserID& user_id) const;
    std::vector<std::string> get_user_sessions(const UserID& user_id) const;

    void set_max_connections(size_t max_connections);
    void set_max_message_size(size_t max_size);
    void set_ping_interval(int interval_seconds);

private:
    using Server = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHandle = websocketpp::connection_hdl;

    void on_open(ConnectionHandle hdl);
    void on_close(ConnectionHandle hdl);
    void on_message(ConnectionHandle hdl, Server::message_ptr msg);
    void on_http(ConnectionHandle hdl);
    void on_ping(ConnectionHandle hdl, std::string payload);
    void on_pong(ConnectionHandle hdl, std::string payload);
    void on_pong_timeout(ConnectionHandle hdl, std::string payload);

    bool authenticate_connection(ConnectionHandle hdl, const nlohmann::json& auth_data);
    bool validate_message(const nlohmann::json& message);
    void handle_client_message(ConnectionHandle hdl, const nlohmann::json& message);
    void handle_sync_request(ConnectionHandle hdl, const nlohmann::json& request);
    void handle_presence_update(ConnectionHandle hdl, const nlohmann::json& update);
    void handle_typing_notification(ConnectionHandle hdl, const nlohmann::json& notification);
    void handle_receipt_notification(ConnectionHandle hdl, const nlohmann::json& receipt);
    void handle_to_device_message(ConnectionHandle hdl, const nlohmann::json& message);

    void send_error(ConnectionHandle hdl, const std::string& error_code, const std::string& message);
    void send_success(ConnectionHandle hdl, const std::string& request_id, const nlohmann::json& data = {});
    void send_event(ConnectionHandle hdl, const nlohmann::json& event);

    void cleanup_dead_connections();
    void send_ping_to_all();
    void check_connection_health();

    Server server_;
    std::thread server_thread_;
    bool running_ = false;
    int port_ = 8080;
    size_t max_connections_ = 10000;
    size_t max_message_size_ = 16 * 1024 * 1024; // 16MB
    int ping_interval_ = 30;

    std::unordered_map<std::string, std::shared_ptr<WebSocketSession>> sessions_;
    std::unordered_map<UserID, std::vector<std::string>> user_sessions_;
    std::unordered_map<ConnectionHandle, std::string> connection_to_session_;
    std::unordered_map<std::string, ConnectionHandle> session_to_connection_;

    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;

    mutable std::shared_mutex sessions_mutex_;
};

}
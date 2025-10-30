#pragma once

#include "../matrix_types.hpp"
#include "user.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace matrix {

class Authentication {
public:
    Authentication();
    ~Authentication() = default;

    struct LoginRequest {
        std::string type;
        std::string user;
        std::string password;
        std::string device_id;
        std::string initial_device_display_name;
        nlohmann::json additional_fields;
    };

    struct LoginResponse {
        std::string user_id;
        std::string access_token;
        std::string device_id;
        std::string home_server;
        nlohmann::json well_known;
    };

    struct RegisterRequest {
        std::string username;
        std::string password;
        std::string device_id;
        std::string initial_device_display_name;
        bool inhibit_login = false;
        std::string auth_type;
        nlohmann::json auth_data;
    };

    struct RegisterResponse {
        std::string user_id;
        std::string access_token;
        std::string device_id;
        std::string home_server;
    };

    Result<LoginResponse> login(const LoginRequest& request);
    Result<RegisterResponse> register_user(const RegisterRequest& request);
    bool logout(const std::string& access_token);
    bool logout_all_devices(const UserID& user_id);

    bool validate_access_token(const std::string& access_token) const;
    UserID get_user_from_token(const std::string& access_token) const;
    DeviceID get_device_from_token(const std::string& access_token) const;

    std::string create_access_token(const UserID& user_id, const DeviceID& device_id = "");
    bool revoke_access_token(const std::string& access_token);
    bool revoke_all_user_tokens(const UserID& user_id);

    std::vector<std::string> get_user_access_tokens(const UserID& user_id) const;
    std::vector<std::string> get_device_access_tokens(const UserID& user_id, const DeviceID& device_id) const;

    bool change_password(const UserID& user_id, const std::string& new_password);
    bool check_password(const UserID& user_id, const std::string& password);

    std::vector<nlohmann::json> get_login_flows() const;

    bool is_user_registered(const UserID& user_id) const;
    bool is_username_available(const std::string& username) const;

    nlohmann::json get_auth_data(const std::string& session_id) const;
    bool set_auth_data(const std::string& session_id, const nlohmann::json& data);
    bool complete_auth(const std::string& session_id);

    void cleanup_expired_tokens(int max_age_hours = 24);
    void cleanup_old_sessions(int max_age_hours = 1);

private:
    struct AuthSession {
        std::string session_id;
        std::string auth_type;
        nlohmann::json auth_data;
        Timestamp created_ts;
        Timestamp updated_ts;
        bool completed = false;
    };

    struct AccessToken {
        std::string token;
        UserID user_id;
        DeviceID device_id;
        Timestamp created_ts;
        Timestamp expires_ts;
        bool valid = true;
    };

    struct UserCredentials {
        std::string user_id;
        std::string password_hash;
        std::string salt;
        std::string algorithm;
        Timestamp created_ts;
    };

    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, AccessToken> access_tokens_;
    std::unordered_map<UserID, std::vector<std::string>> user_tokens_;
    std::unordered_map<std::string, UserCredentials> user_credentials_;
    std::unordered_map<std::string, AuthSession> auth_sessions_;

    std::string generate_access_token() const;
    std::string generate_session_id() const;
    std::string hash_password(const std::string& password, const std::string& salt) const;
    std::string generate_salt() const;
    bool verify_password(const std::string& password, const std::string& hash, const std::string& salt) const;

    Result<LoginResponse> password_login(const LoginRequest& request);
    Result<LoginResponse> token_login(const LoginRequest& request);

    bool validate_username(const std::string& username) const;
    bool validate_password(const std::string& password) const;

    void cleanup_internal();
};

}
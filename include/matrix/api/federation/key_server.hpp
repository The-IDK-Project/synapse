#pragma once

#include "../common/api_types.hpp"
#include "../../core/user/device.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::api::federation {

class KeyServer {
public:
    KeyServer();
    ~KeyServer();

    //Key's
    ApiResponse upload_keys(const ApiRequest& request);
    ApiResponse upload_signatures(const ApiRequest& request);
    ApiResponse upload_one_time_keys(const ApiRequest& request);
    ApiResponse query_keys(const ApiRequest& request);
    ApiResponse claim_keys(const ApiRequest& request);
    ApiResponse get_key_changes(const ApiRequest& request);
    ApiResponse verify_keys(const ApiRequest& request);
    ApiResponse verify_signatures(const ApiRequest& request);
    ApiResponse upload_cross_signing_keys(const ApiRequest& request);
    ApiResponse query_cross_signing_keys(const ApiRequest& request);
    ApiResponse upload_cross_signing_signatures(const ApiRequest& request);
    ApiResponse create_key_backup(const ApiRequest& request);
    ApiResponse update_key_backup(const ApiRequest& request);
    ApiResponse delete_key_backup(const ApiRequest& request);
    ApiResponse get_key_backup(const ApiRequest& request);
    ApiResponse get_key_backup_sessions(const ApiRequest& request);
    ApiResponse get_key_backup_session(const ApiRequest& request);

private:
    struct KeyServerConfig {
        size_t max_keys_per_user = 1000;
        size_t max_one_time_keys_per_device = 100;
        int key_expiry_days = 30;
        bool enable_cross_signing = true;
        bool enable_key_backup = true;
    };
    bool store_device_keys(const UserID& user_id, const DeviceID& device_id, const nlohmann::json& keys);
    bool store_one_time_keys(const UserID& user_id, const DeviceID& device_id, const nlohmann::json& keys);
    nlohmann::json get_device_keys(const UserID& user_id, const DeviceID& device_id = "");
    nlohmann::json get_one_time_keys(const UserID& user_id, const DeviceID& device_id = "");
    nlohmann::json claim_one_time_keys(const UserID& user_id, const std::unordered_map<DeviceID, std::string>& claims);
    bool validate_device_keys(const nlohmann::json& keys);
    bool validate_one_time_keys(const nlohmann::json& keys);
    bool verify_key_signatures(const nlohmann::json& keys, const nlohmann::json& signatures);
    bool verify_cross_signing_keys(const nlohmann::json& keys);
    bool store_cross_signing_keys(const UserID& user_id, const nlohmann::json& keys);
    bool store_cross_signing_signatures(const UserID& user_id, const nlohmann::json& signatures);
    nlohmann::json get_cross_signing_keys(const UserID& user_id);
    nlohmann::json get_cross_signing_signatures(const UserID& user_id);
    bool verify_user_identity(const UserID& user_id);
    std::string create_key_backup_version(const UserID& user_id, const nlohmann::json& backup_data);
    bool update_key_backup_data(const UserID& user_id, const std::string& version, const nlohmann::json& backup_data);
    bool store_room_key_backup(const UserID& user_id, const std::string& version, const RoomID& room_id,
                              const std::string& session_id, const nlohmann::json& session_data);
    nlohmann::json get_key_backup_data(const UserID& user_id, const std::string& version = "");
    nlohmann::json get_room_key_backup(const UserID& user_id, const std::string& version, const RoomID& room_id,
                                      const std::string& session_id = "");
    void distribute_key_updates(const UserID& user_id, const DeviceID& device_id = "");
    void notify_key_changes(const UserID& user_id, const std::vector<DeviceID>& changed_devices);
    void replicate_keys_to_servers(const UserID& user_id, const std::vector<std::string>& server_names);
    void rotate_user_keys(const UserID& user_id);
    void expire_old_keys();
    void cleanup_orphaned_keys();
    nlohmann::json build_federated_keys_response(const std::vector<UserID>& user_ids);
    nlohmann::json build_federated_claimed_keys_response(const std::unordered_map<UserID, std::unordered_map<DeviceID, std::string>>& claims);
    bool verify_federated_key_query(const std::string& server_name, const std::vector<UserID>& user_ids);
    nlohmann::json build_upload_keys_response(const std::unordered_map<std::string, int>& one_time_key_counts = {});
    nlohmann::json build_query_keys_response(const std::vector<UserID>& user_ids, const std::string& token = "");
    nlohmann::json build_claim_keys_response(const std::unordered_map<UserID, std::unordered_map<DeviceID, nlohmann::json>>& one_time_keys);
    nlohmann::json build_key_changes_response(const std::string& from_token, const std::string& to_token);
    ApiResponse build_key_validation_error(const std::string& reason);
    ApiResponse build_key_not_found_error(const UserID& user_id, const DeviceID& device_id = "");
    ApiResponse build_key_backup_error(const std::string& version, const std::string& reason);
    ApiResponse build_cross_signing_error(const std::string& reason);

    KeyServerConfig config_;
    std::shared_ptr<core::UserManager> user_manager_;

    std::unordered_map<UserID, std::unordered_map<DeviceID, nlohmann::json>> device_keys_;
    std::unordered_map<UserID, std::unordered_map<DeviceID, nlohmann::json>> one_time_keys_;
    std::unordered_map<UserID, nlohmann::json> cross_signing_keys_;
    std::unordered_map<UserID, nlohmann::json> cross_signing_signatures_;
    std::unordered_map<UserID, std::unordered_map<std::string, nlohmann::json>> key_backups_;
    std::unordered_map<std::string, std::vector<UserID>> key_change_log_;
};

}
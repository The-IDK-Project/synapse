#pragma once

#include "repository.hpp"
#include "../../core/user/device.hpp"
#include <vector>
#include <memory>

namespace matrix::storage::repository {

class DeviceRepository : public CrudRepository<core::Device> {
public:
    DeviceRepository() = default;
    virtual ~DeviceRepository() = default;

    virtual std::unique_ptr<core::Device> read_by_user_and_device(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual std::vector<std::unique_ptr<core::Device>> read_by_user(const core::UserID& user_id) = 0;
    virtual std::vector<std::unique_ptr<core::Device>> read_all_devices(int limit = 100, const std::string& since_token = "") = 0;

    virtual bool set_display_name(const core::UserID& user_id, const core::DeviceID& device_id, const std::string& display_name) = 0;
    virtual bool update_last_seen(const core::UserID& user_id, const core::DeviceID& device_id, const std::string& ip = "", const std::string& user_agent = "") = 0;
    virtual bool set_verified_status(const core::UserID& user_id, const core::DeviceID& device_id, bool verified) = 0;
    virtual bool set_blocked_status(const core::UserID& user_id, const core::DeviceID& device_id, bool blocked) = 0;

    virtual std::string get_display_name(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual int64_t get_last_seen_ts(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual bool is_verified(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual bool is_blocked(const core::UserID& user_id, const core::DeviceID& device_id) = 0;

    virtual bool set_device_keys(const core::UserID& user_id, const core::DeviceID& device_id, const nlohmann::json& device_keys) = 0;
    virtual bool set_one_time_keys(const core::UserID& user_id, const core::DeviceID& device_id, const nlohmann::json& one_time_keys) = 0;
    virtual bool mark_one_time_key_used(const core::UserID& user_id, const core::DeviceID& device_id, const std::string& key_id) = 0;

    virtual std::unique_ptr<nlohmann::json> get_device_keys(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual std::unique_ptr<nlohmann::json> get_one_time_keys(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual std::vector<std::string> get_unused_one_time_key_ids(const core::UserID& user_id, const core::DeviceID& device_id) = 0;

    virtual bool set_cross_signing_keys(const core::UserID& user_id, const nlohmann::json& cross_signing_keys) = 0;
    virtual bool set_cross_signing_signatures(const core::UserID& user_id, const nlohmann::json& signatures) = 0;
    virtual std::unique_ptr<nlohmann::json> get_cross_signing_keys(const core::UserID& user_id) = 0;
    virtual std::unique_ptr<nlohmann::json> get_cross_signing_signatures(const core::UserID& user_id) = 0;

    virtual bool set_key_backup(const core::UserID& user_id, const std::string& version, const nlohmann::json& backup_data) = 0;
    virtual bool set_room_key_backup(const core::UserID& user_id, const std::string& version, const core::RoomID& room_id, const std::string& session_id, const nlohmann::json& session_data) = 0;
    virtual std::unique_ptr<nlohmann::json> get_key_backup(const core::UserID& user_id, const std::string& version = "") = 0;
    virtual std::unique_ptr<nlohmann::json> get_room_key_backup(const core::UserID& user_id, const std::string& version, const core::RoomID& room_id, const std::string& session_id = "") = 0;
    virtual bool delete_key_backup(const core::UserID& user_id, const std::string& version) = 0;

    virtual bool set_olm_session(const core::UserID& user_id, const core::DeviceID& device_id, const std::string& session_id, const nlohmann::json& session_data) = 0;
    virtual bool set_megolm_session(const std::string& session_id, const core::RoomID& room_id, const nlohmann::json& session_data) = 0;
    virtual bool set_megolm_inbound_session(const std::string& session_id, const core::RoomID& room_id, const nlohmann::json& session_data) = 0;

    virtual std::unique_ptr<nlohmann::json> get_olm_session(const core::UserID& user_id, const core::DeviceID& device_id, const std::string& session_id) = 0;
    virtual std::unique_ptr<nlohmann::json> get_megolm_session(const std::string& session_id) = 0;
    virtual std::unique_ptr<nlohmann::json> get_megolm_inbound_session(const std::string& session_id, const core::RoomID& room_id) = 0;
    virtual std::vector<std::string> get_olm_session_ids(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual std::vector<std::string> get_megolm_session_ids_for_room(const core::RoomID& room_id) = 0;

    virtual int64_t get_device_count_for_user(const core::UserID& user_id) = 0;
    virtual int64_t get_total_device_count() = 0;
    virtual int64_t get_verified_device_count() = 0;

    virtual bool cleanup_inactive_devices(int64_t max_inactive_ts) = 0;
    virtual bool delete_user_devices(const core::UserID& user_id) = 0;
    virtual bool delete_device_data(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual bool cleanup_expired_sessions(int64_t max_age_ts) = 0;
    virtual bool cleanup_used_one_time_keys() = 0;

    struct DeviceStats {
        core::UserID user_id;
        core::DeviceID device_id;
        std::string display_name;
        int64_t last_seen_ts;
        bool is_verified;
        bool is_blocked;
        int key_count;
        int one_time_key_count;
        int session_count;
    };

    virtual std::unique_ptr<DeviceStats> get_device_stats(const core::UserID& user_id, const core::DeviceID& device_id) = 0;
    virtual std::vector<DeviceStats> get_user_device_stats(const core::UserID& user_id) = 0;
    virtual std::vector<DeviceStats> get_all_device_stats(int limit = 100, const std::string& since_token = "") = 0;
};

}
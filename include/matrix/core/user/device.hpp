#pragma once

#include "../matrix_types.hpp"
#include <string>
#include <unordered_map>

namespace matrix {

class Device {
public:
    Device(const UserID& user_id, const DeviceID& device_id);
    ~Device() = default;

    UserID user_id() const { return user_id_; }
    DeviceID device_id() const { return device_id_; }
    std::string display_name() const { return display_name_; }
    int64_t last_seen_ts() const { return last_seen_ts_; }
    std::string ip() const { return ip_; }
    std::string user_agent() const { return user_agent_; }
    Timestamp created_ts() const { return created_ts_; }

    void set_display_name(const std::string& display_name) { display_name_ = display_name; }
    void set_last_seen_ts(int64_t ts) { last_seen_ts_ = ts; }
    void set_ip(const std::string& ip) { ip_ = ip; }
    void set_user_agent(const std::string& user_agent) { user_agent_ = user_agent; }

    std::unordered_map<std::string, std::string> keys() const { return keys_; }
    void set_keys(const std::unordered_map<std::string, std::string>& keys) { keys_ = keys; }
    void add_key(const std::string& key_id, const std::string& key) { keys_[key_id] = key; }
    bool has_key(const std::string& key_id) const { return keys_.count(key_id) > 0; }
    std::string get_key(const std::string& key_id) const;

    std::unordered_map<std::string, std::string> signatures() const { return signatures_; }
    void set_signatures(const std::unordered_map<std::string, std::string>& signatures) { signatures_ = signatures; }
    void add_signature(const std::string& signer, const std::string& signature) { signatures_[signer] = signature; }

    std::vector<std::string> algorithms() const { return algorithms_; }
    void set_algorithms(const std::vector<std::string>& algorithms) { algorithms_ = algorithms; }
    void add_algorithm(const std::string& algorithm) { algorithms_.push_back(algorithm); }

    std::unordered_map<std::string, nlohmann::json> one_time_keys() const { return one_time_keys_; }
    void set_one_time_keys(const std::unordered_map<std::string, nlohmann::json>& keys) { one_time_keys_ = keys; }
    void add_one_time_key(const std::string& key_id, const nlohmann::json& key) { one_time_keys_[key_id] = key; }
    bool has_one_time_key(const std::string& key_id) const { return one_time_keys_.count(key_id) > 0; }

    bool is_verified() const { return is_verified_; }
    void set_verified(bool verified) { is_verified_ = verified; }

    bool is_blocked() const { return is_blocked_; }
    void set_blocked(bool blocked) { is_blocked_ = blocked; }

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);

    nlohmann::json get_device_keys() const;
    nlohmann::json get_one_time_keys() const;

    bool validate_keys() const;

private:
    UserID user_id_;
    DeviceID device_id_;
    std::string display_name_;
    int64_t last_seen_ts_ = 0;
    std::string ip_;
    std::string user_agent_;
    Timestamp created_ts_;

    std::unordered_map<std::string, std::string> keys_;
    std::unordered_map<std::string, std::string> signatures_;
    std::vector<std::string> algorithms_;
    std::unordered_map<std::string, nlohmann::json> one_time_keys_;

    bool is_verified_ = false;
    bool is_blocked_ = false;
};

using DevicePtr = std::shared_ptr<Device>;

}
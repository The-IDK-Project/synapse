#pragma once

#include "types.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>

namespace matrix {

struct RoomVersion {
    std::string version;
    std::string state_resolution;
    std::vector<std::string> supported_features;
};

struct RoomAlias {
    std::string alias;
    RoomID room_id;
    std::vector<ServerName> servers;
};

struct Content : public nlohmann::json {
    using nlohmann::json::json;
    using nlohmann::json::operator=;
};

struct UnsignedData {
    std::optional<int64_t> age;
    Content transaction_data;
    std::optional<Content> prev_content;
    std::optional<std::string> redacted_because;
    std::optional<std::string> invite_room_state;
};

struct EventReference {
    EventID event_id;
    std::string room_id;
    std::vector<std::string> servers;
};

struct PowerLevels {
    int ban = 50;
    int kick = 50;
    int redact = 50;
    int invite = 0;
    int events_default = 0;
    int state_default = 50;
    int users_default = 0;
    int notifications_room = 50;
    std::unordered_map<std::string, int> events;
    std::unordered_map<UserID, int> users;
};

struct RoomCreateContent {
    std::string creator;
    std::string room_version = "1";
    std::string predecessor_room_id;
    std::vector<std::string> predecessor_servers;
};

struct RoomMemberContent {
    std::string membership;
    std::string displayname;
    std::string avatar_url;
    std::string reason;
    bool is_direct = false;
    std::optional<std::string> third_party_invite;
};

struct RoomMessageContent {
    std::string msgtype;
    std::string body;
    std::string format;
    std::string formatted_body;
    std::vector<std::string> mentions;
    Content info;
    std::string url;
    std::string filename;
};

struct RoomEncryptionContent {
    std::string algorithm;
    int rotation_period_ms = 604800000;
    int rotation_period_msgs = 100;
};

struct RoomRedactionContent {
    std::string reason;
};

struct TypingContent {
    std::vector<UserID> user_ids;
};

struct ReceiptContent {
    std::unordered_map<EventID, std::unordered_map<std::string, Content>> users;
};

struct PresenceContent {
    std::string presence;
    std::string status_msg;
    int last_active_ago = 0;
    bool currently_active = false;
};

struct DeviceKeys {
    UserID user_id;
    DeviceID device_id;
    std::vector<std::string> algorithms;
    std::unordered_map<std::string, std::string> keys;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> signatures;
};

struct OneTimeKey {
    std::string key;
    std::unordered_map<std::string, std::string> signatures;
};

struct KeyQueryResponse {
    std::unordered_map<UserID, std::unordered_map<DeviceID, DeviceKeys>> device_keys;
    std::unordered_map<UserID, std::unordered_map<DeviceID, std::unordered_map<std::string, OneTimeKey>>> one_time_keys;
};

struct KeyClaimResponse {
    std::unordered_map<UserID, std::unordered_map<DeviceID, std::unordered_map<std::string, OneTimeKey>>> one_time_keys;
};

struct SyncResponse {
    std::string next_batch;
    Content account_data;
    Content presence;
    Content to_device;
    Content device_lists;
    Content device_one_time_keys_count;
    std::unordered_map<RoomID, RoomSync> rooms;
};

struct RoomSync {
    Content state;
    Content timeline;
    Content ephemeral;
    Content account_data;
    std::string prev_batch;
    bool limited = false;
};

struct PublicRoomsResponse {
    std::vector<PublicRoom> chunk;
    std::string next_batch;
    int total_room_count_estimate = 0;
};

struct PublicRoom {
    std::vector<std::string> aliases;
    std::string canonical_alias;
    std::string name;
    int num_joined_members = 0;
    std::string room_id;
    std::string topic;
    bool world_readable = false;
    bool guest_can_join = false;
    std::string avatar_url;
};

struct MediaInfo {
    std::string media_id;
    std::string media_type;
    int media_length = 0;
    std::string upload_name;
    UserID user_id;
    Timestamp created_ts;
};

struct Filter {
    std::optional<int> limit;
    std::vector<std::string> senders;
    std::vector<std::string> not_senders;
    std::vector<std::string> types;
    std::vector<std::string> not_types;
    std::vector<std::string> rooms;
    std::vector<std::string> not_rooms;
    bool contains_url = false;
    std::unordered_map<std::string, Filter> room;
};

struct LoginFlow {
    std::string type;
};

struct LoginResponse {
    std::string user_id;
    std::string access_token;
    std::string device_id;
    std::string home_server;
    std::optional<DeviceKeys> well_known;
};

struct WhoamiResponse {
    std::string user_id;
    std::string device_id;
};

struct VersionsResponse {
    std::vector<std::string> versions;
    std::unordered_map<std::string, std::string> unstable_features;
};

}
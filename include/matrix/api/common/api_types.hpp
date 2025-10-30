#pragma once

#include "../../core/matrix_types.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>

namespace matrix::api {

struct ApiRequest {
    std::string method;
    std::string path;
    nlohmann::json query_params;
    nlohmann::json body;
    std::unordered_map<std::string, std::string> headers;
    std::string access_token;
    UserID user_id;
    std::string client_ip;
    std::string user_agent;
};

struct ApiResponse {
    int status_code = 200;
    nlohmann::json body;
    std::unordered_map<std::string, std::string> headers;
    std::string content_type = "application/json";
    std::string error;
    std::string error_code;
};

struct PaginationParams {
    std::optional<std::string> from;
    std::optional<std::string> to;
    std::optional<int> limit;
    std::optional<std::string> dir;
};

struct SyncParams {
    std::optional<std::string> since;
    std::optional<std::string> filter;
    std::optional<bool> full_state;
    std::optional<std::string> set_presence;
    std::optional<int> timeout;
};

struct FilterParams {
    std::optional<int> limit;
    std::optional<std::vector<std::string>> not_senders;
    std::optional<std::vector<std::string>> not_types;
    std::optional<std::vector<std::string>> senders;
    std::optional<std::vector<std::string>> types;
    std::optional<bool> contains_url;
};

struct RoomFilterParams : FilterParams {
    std::optional<std::vector<std::string>> not_rooms;
    std::optional<std::vector<std::string>> rooms;
    std::optional<bool> include_leave;
    std::optional<FilterParams> account_data;
    std::optional<FilterParams> room;
    std::optional<FilterParams> presence;
    std::optional<FilterParams> state;
    std::optional<FilterParams> timeline;
};

struct LoginRequest {
    std::string type;
    std::optional<std::string> user;
    std::optional<std::string> password;
    std::optional<std::string> token;
    std::optional<std::string> device_id;
    std::optional<std::string> initial_device_display_name;
    std::optional<nlohmann::json> identifier;
};

struct LoginResponse {
    std::string user_id;
    std::string access_token;
    std::string device_id;
    std::string home_server;
    std::optional<nlohmann::json> well_known;
};

struct RegisterRequest {
    std::optional<std::string> username;
    std::optional<std::string> password;
    std::optional<std::string> device_id;
    std::optional<std::string> initial_device_display_name;
    std::optional<bool> inhibit_login;
    std::optional<nlohmann::json> auth;
};

struct RegisterResponse {
    std::string user_id;
    std::string access_token;
    std::string device_id;
    std::string home_server;
};

struct WhoamiResponse {
    std::string user_id;
    std::string device_id;
};

struct VersionsResponse {
    std::vector<std::string> versions;
    std::unordered_map<std::string, std::string> unstable_features;
};

struct PublicRoomsResponse {
    std::vector<PublicRoom> chunk;
    std::optional<std::string> next_batch;
    std::optional<int> total_room_count_estimate;
};

struct PublicRoomsRequest {
    std::optional<int> limit;
    std::optional<std::string> since;
    std::optional<std::string> filter;
    std::optional<bool> include_all_networks;
    std::optional<std::string> third_party_instance_id;
};

struct RoomCreateRequest {
    std::optional<std::string> room_alias_name;
    std::optional<std::string> name;
    std::optional<std::string> topic;
    std::optional<std::vector<UserID>> invite;
    std::optional<std::string> preset;
    std::optional<bool> is_direct;
    std::optional<nlohmann::json> creation_content;
    std::optional<nlohmann::json> initial_state;
    std::optional<nlohmann::json> power_level_content_override;
    std::optional<std::string> visibility;
};

struct RoomCreateResponse {
    std::string room_id;
};

struct RoomInviteRequest {
    std::string user_id;
    std::optional<std::string> reason;
};

struct RoomJoinRequest {
    std::optional<std::string> reason;
};

struct RoomKickRequest {
    std::string user_id;
    std::optional<std::string> reason;
};

struct RoomBanRequest {
    std::string user_id;
    std::optional<std::string> reason;
};

struct RoomUnbanRequest {
    std::string user_id;
};

struct RoomSendRequest {
    std::string event_type;
    nlohmann::json content;
    std::optional<std::string> txn_id;
};

struct RoomSendResponse {
    std::string event_id;
};

struct RoomStateResponse {
    nlohmann::json content;
    std::string state_key;
    std::string type;
    std::string event_id;
    std::string sender;
    int64_t origin_server_ts;
    std::optional<nlohmann::json> unsigned_data;
    std::optional<nlohmann::json> prev_content;
};

struct RoomMembersResponse {
    std::vector<RoomStateResponse> chunk;
};

struct RoomMessagesResponse {
    std::string start;
    std::string end;
    std::vector<nlohmann::json> chunk;
    std::vector<nlohmann::json> state;
};

struct TypingRequest {
    bool typing;
    std::optional<int> timeout;
};

struct ReceiptRequest {
    std::string receipt_type;
    nlohmann::json content;
};

struct PresenceRequest {
    std::string presence;
    std::optional<std::string> status_msg;
};

struct ProfileRequest {
    std::optional<std::string> displayname;
    std::optional<std::string> avatar_url;
};

struct DeviceKeysUpdateRequest {
    nlohmann::json device_keys;
};

struct KeysUploadRequest {
    std::optional<nlohmann::json> device_keys;
    std::optional<std::unordered_map<std::string, nlohmann::json>> one_time_keys;
};

struct KeysUploadResponse {
    std::optional<std::unordered_map<std::string, int>> one_time_key_counts;
};

struct KeysQueryRequest {
    std::optional<std::vector<std::string>> device_keys;
    std::optional<std::string> token;
    std::optional<int> timeout;
};

struct KeysClaimRequest {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> one_time_keys;
    std::optional<std::string> token;
    std::optional<int> timeout;
};

struct MediaUploadResponse {
    std::string content_uri;
};

struct ThreepidRequest {
    std::string client_secret;
    std::string email;
    std::string id_server;
    std::string id_access_token;
    int send_attempt;
};

struct ThreepidResponse {
    std::string sid;
};

}
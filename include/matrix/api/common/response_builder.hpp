#pragma once

#include "api_types.hpp"
#include "error_codes.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace matrix::api {

class ResponseBuilder {
public:
    ResponseBuilder();
    ~ResponseBuilder() = default;

    static ApiResponse success(const nlohmann::json& data = {});
    static ApiResponse success(const std::string& message);
    static ApiResponse created(const nlohmann::json& data = {});
    static ApiResponse no_content();

    static ApiResponse error(ErrorCode code, const std::string& message);
    static ApiResponse error(const ErrorResponse& error_response);
    static ApiResponse bad_request(const std::string& message = "Bad request");
    static ApiResponse unauthorized(const std::string& message = "Unauthorized");
    static ApiResponse forbidden(const std::string& message = "Forbidden");
    static ApiResponse not_found(const std::string& message = "Not found");
    static ApiResponse method_not_allowed(const std::string& message = "Method not allowed");
    static ApiResponse conflict(const std::string& message = "Conflict");
    static ApiResponse too_many_requests(const std::string& message = "Too many requests", int retry_after_ms = 0);
    static ApiResponse internal_error(const std::string& message = "Internal server error");
    static ApiResponse not_implemented(const std::string& message = "Not implemented");
    static ApiResponse service_unavailable(const std::string& message = "Service unavailable");

    static ApiResponse with_headers(const ApiResponse& response, const std::unordered_map<std::string, std::string>& headers);
    static ApiResponse with_content_type(const ApiResponse& response, const std::string& content_type);
    static ApiResponse with_cache_control(const ApiResponse& response, const std::string& cache_control);

    static nlohmann::json build_error_body(const ErrorResponse& error);
    static nlohmann::json build_success_body(const nlohmann::json& data = {});
    static nlohmann::json build_paginated_response(const nlohmann::json& chunk, const std::string& next_batch = "");

    static ApiResponse login_response(const LoginResponse& login);
    static ApiResponse register_response(const RegisterResponse& registration);
    static ApiResponse whoami_response(const WhoamiResponse& whoami);
    static ApiResponse versions_response(const VersionsResponse& versions);
    static ApiResponse public_rooms_response(const PublicRoomsResponse& rooms);
    static ApiResponse room_create_response(const RoomCreateResponse& room);
    static ApiResponse room_send_response(const RoomSendResponse& send);
    static ApiResponse room_members_response(const RoomMembersResponse& members);
    static ApiResponse room_messages_response(const RoomMessagesResponse& messages);
    static ApiResponse keys_upload_response(const KeysUploadResponse& keys);
    static ApiResponse media_upload_response(const MediaUploadResponse& media);
    static ApiResponse threepid_response(const ThreepidResponse& threepid);

    static ApiResponse sync_response(const nlohmann::json& sync_data);
    static ApiResponse event_response(const nlohmann::json& event_data);
    static ApiResponse state_response(const nlohmann::json& state_data);
    static ApiResponse profile_response(const nlohmann::json& profile_data);
    static ApiResponse presence_response(const nlohmann::json& presence_data);
    static ApiResponse devices_response(const nlohmann::json& devices_data);
    static ApiResponse device_keys_response(const nlohmann::json& keys_data);
    static ApiResponse one_time_keys_response(const nlohmann::json& keys_data);

    static std::unordered_map<std::string, std::string> cors_headers();
    static std::unordered_map<std::string, std::string> json_headers();
    static std::unordered_map<std::string, std::string> no_cache_headers();

private:
    static ApiResponse build_response(int status_code, const nlohmann::json& body, const std::string& error_code = "");
    static std::string error_code_to_string(ErrorCode code);
};

}
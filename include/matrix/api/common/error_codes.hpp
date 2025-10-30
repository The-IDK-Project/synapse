#pragma once

#include <string>
#include <unordered_map>

namespace matrix::api {

enum class ErrorCode {
    SUCCESS = 0,

    // Client API errors
    FORBIDDEN = 403,
    UNKNOWN = 404,
    UNKNOWN_TOKEN = 401,
    MISSING_TOKEN = 401,
    BAD_JSON = 400,
    NOT_JSON = 400,
    USER_IN_USE = 400,
    ROOM_IN_USE = 400,
    BAD_PAGINATION = 400,
    CANNOT_LEAVE_SERVER_NOTICE_ROOM = 400,
    THREEPID_AUTH_FAILED = 403,
    THREEPID_IN_USE = 400,
    THREEPID_NOT_FOUND = 404,
    SERVER_NOT_TRUSTED = 400,
    SESSION_EXPIRED = 403,
    SESSION_NOT_VALIDATED = 403,
    INVALID_USERNAME = 400,
    SERVER_TIMEOUT = 504,
    GUEST_ACCESS_FORBIDDEN = 403,
    CAPTCHA_NEEDED = 401,
    CAPTCHA_INVALID = 400,
    MISSING_PARAM = 400,
    INVALID_PARAM = 400,
    TOO_LARGE = 413,
    EXCLUSIVE = 403,
    RESOURCE_LIMIT_EXCEEDED = 429,
    LIMIT_EXCEEDED = 429,

    // Federation errors
    FEDERATION_DENIED = 403,
    FEDERATION_TIMEOUT = 504,
    FEDERATION_UNAVAILABLE = 502,
    FEDERATION_INVALID_SIGNATURE = 403,
    FEDERATION_SERVER_NOT_FOUND = 404,

    // Internal errors
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    SERVICE_UNAVAILABLE = 503
};

struct ErrorResponse {
    std::string errcode;
    std::string error;
    std::optional<int> retry_after_ms;
    std::optional<std::string> admin_contact;
    std::optional<nlohmann::json> additional_data;

    ErrorResponse(const std::string& code, const std::string& message)
        : errcode(code), error(message) {}
};

class ErrorCodes {
public:
    static ErrorResponse forbidden(const std::string& message = "Forbidden") {
        return ErrorResponse("M_FORBIDDEN", message);
    }

    static ErrorResponse unknown(const std::string& message = "Unknown error") {
        return ErrorResponse("M_UNKNOWN", message);
    }

    static ErrorResponse unknown_token(const std::string& message = "Unrecognized access token") {
        return ErrorResponse("M_UNKNOWN_TOKEN", message);
    }

    static ErrorResponse missing_token(const std::string& message = "Missing access token") {
        return ErrorResponse("M_MISSING_TOKEN", message);
    }

    static ErrorResponse bad_json(const std::string& message = "Invalid JSON") {
        return ErrorResponse("M_BAD_JSON", message);
    }

    static ErrorResponse not_json(const std::string& message = "Not JSON") {
        return ErrorResponse("M_NOT_JSON", message);
    }

    static ErrorResponse user_in_use(const std::string& message = "User ID already taken") {
        return ErrorResponse("M_USER_IN_USE", message);
    }

    static ErrorResponse room_in_use(const std::string& message = "Room alias already taken") {
        return ErrorResponse("M_ROOM_IN_USE", message);
    }

    static ErrorResponse bad_pagination(const std::string& message = "Invalid pagination parameters") {
        return ErrorResponse("M_BAD_PAGINATION", message);
    }

    static ErrorResponse cannot_leave_server_notice_room(const std::string& message = "Cannot leave server notice room") {
        return ErrorResponse("M_CANNOT_LEAVE_SERVER_NOTICE_ROOM", message);
    }

    static ErrorResponse threepid_auth_failed(const std::string& message = "Third-party identifier auth failed") {
        return ErrorResponse("M_THREEPID_AUTH_FAILED", message);
    }

    static ErrorResponse threepid_in_use(const std::string& message = "Third-party identifier already in use") {
        return ErrorResponse("M_THREEPID_IN_USE", message);
    }

    static ErrorResponse threepid_not_found(const std::string& message = "Third-party identifier not found") {
        return ErrorResponse("M_THREEPID_NOT_FOUND", message);
    }

    static ErrorResponse server_not_trusted(const std::string& message = "Server not trusted") {
        return ErrorResponse("M_SERVER_NOT_TRUSTED", message);
    }

    static ErrorResponse session_expired(const std::string& message = "Session expired") {
        return ErrorResponse("M_SESSION_EXPIRED", message);
    }

    static ErrorResponse session_not_validated(const std::string& message = "Session not validated") {
        return ErrorResponse("M_SESSION_NOT_VALIDATED", message);
    }

    static ErrorResponse invalid_username(const std::string& message = "Invalid username") {
        return ErrorResponse("M_INVALID_USERNAME", message);
    }

    static ErrorResponse server_timeout(const std::string& message = "Server timeout") {
        return ErrorResponse("M_SERVER_TIMEOUT", message);
    }

    static ErrorResponse guest_access_forbidden(const std::string& message = "Guest access forbidden") {
        return ErrorResponse("M_GUEST_ACCESS_FORBIDDEN", message);
    }

    static ErrorResponse captcha_needed(const std::string& message = "Captcha needed") {
        return ErrorResponse("M_CAPTCHA_NEEDED", message);
    }

    static ErrorResponse captcha_invalid(const std::string& message = "Captcha invalid") {
        return ErrorResponse("M_CAPTCHA_INVALID", message);
    }

    static ErrorResponse missing_param(const std::string& message = "Missing parameter") {
        return ErrorResponse("M_MISSING_PARAM", message);
    }

    static ErrorResponse invalid_param(const std::string& message = "Invalid parameter") {
        return ErrorResponse("M_INVALID_PARAM", message);
    }

    static ErrorResponse too_large(const std::string& message = "Request too large") {
        return ErrorResponse("M_TOO_LARGE", message);
    }

    static ErrorResponse exclusive(const std::string& message = "Resource is exclusive") {
        return ErrorResponse("M_EXCLUSIVE", message);
    }

    static ErrorResponse resource_limit_exceeded(const std::string& message = "Resource limit exceeded") {
        return ErrorResponse("M_RESOURCE_LIMIT_EXCEEDED", message);
    }

    static ErrorResponse limit_exceeded(const std::string& message = "Rate limit exceeded") {
        return ErrorResponse("M_LIMIT_EXCEEDED", message);
    }

    static ErrorResponse federation_denied(const std::string& message = "Federation denied") {
        return ErrorResponse("M_FEDERATION_DENIED", message);
    }

    static ErrorResponse federation_timeout(const std::string& message = "Federation timeout") {
        return ErrorResponse("M_FEDERATION_TIMEOUT", message);
    }

    static ErrorResponse federation_unavailable(const std::string& message = "Federation unavailable") {
        return ErrorResponse("M_FEDERATION_UNAVAILABLE", message);
    }

    static ErrorResponse federation_invalid_signature(const std::string& message = "Invalid federation signature") {
        return ErrorResponse("M_FEDERATION_INVALID_SIGNATURE", message);
    }

    static ErrorResponse federation_server_not_found(const std::string& message = "Federation server not found") {
        return ErrorResponse("M_FEDERATION_SERVER_NOT_FOUND", message);
    }

    static ErrorResponse internal_error(const std::string& message = "Internal server error") {
        return ErrorResponse("M_INTERNAL_ERROR", message);
    }

    static ErrorResponse not_implemented(const std::string& message = "Not implemented") {
        return ErrorResponse("M_NOT_IMPLEMENTED", message);
    }

    static ErrorResponse service_unavailable(const std::string& message = "Service unavailable") {
        return ErrorResponse("M_SERVICE_UNAVAILABLE", message);
    }

    static std::string to_string(ErrorCode code);
    static ErrorCode from_string(const std::string& code);
    static bool is_client_error(ErrorCode code);
    static bool is_server_error(ErrorCode code);
    static int to_http_status(ErrorCode code);
};

}
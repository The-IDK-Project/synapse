#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <chrono>

namespace matrix {

using UserID = std::string;
using RoomID = std::string;
using EventID = std::string;
using DeviceID = std::string;
using TransactionID = std::string;
using ServerName = std::string;

using Timestamp = std::chrono::system_clock::time_point;
using Milliseconds = std::chrono::milliseconds;

enum class Membership {
    JOIN,
    LEAVE,
    INVITE,
    BAN,
    KNOCK
};

enum class PresenceState {
    ONLINE,
    OFFLINE,
    UNAVAILABLE
};

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

enum class EventType {
    ROOM_CREATE,
    ROOM_JOIN_RULES,
    ROOM_POWER_LEVELS,
    ROOM_MEMBER,
    ROOM_MESSAGE,
    ROOM_ENCRYPTION,
    ROOM_REDACTION,
    ROOM_HISTORY_VISIBILITY,
    ROOM_GUEST_ACCESS,
    ROOM_ALIASES,
    ROOM_CANONICAL_ALIAS,
    ROOM_NAME,
    ROOM_TOPIC,
    ROOM_AVATAR,
    ROOM_PINNED_EVENTS,
    ROOM_TOMBSTONE,
    ROOM_SERVER_ACL,
    TYPING,
    RECEIPT,
    PRESENCE,
    FULLY_READ,
    TAG,
    DIRECT,
    IGNORED_USER_LIST,
    UNKNOWN
};

enum class MessageType {
    TEXT,
    EMOTE,
    NOTICE,
    IMAGE,
    FILE,
    AUDIO,
    VIDEO,
    LOCATION
};

enum class EncryptionAlgorithm {
    OLM_V1,
    MEGOLM_V1,
    NONE
};

enum class ErrorCode {
    SUCCESS = 0,
    FORBIDDEN = 1,
    UNKNOWN = 2,
    NOT_FOUND = 3,
    LIMIT_EXCEEDED = 4,
    UNAUTHORIZED = 5,
    BAD_JSON = 6,
    NOT_JSON = 7,
    USER_IN_USE = 8,
    ROOM_IN_USE = 9,
    BAD_PAGINATION = 10,
    CANNOT_LEAVE_SERVER_NOTICE_ROOM = 11,
    THREEPID_AUTH_FAILED = 12,
    THREEPID_IN_USE = 13,
    THREEPID_NOT_FOUND = 14,
    SERVER_NOT_TRUSTED = 15,
    SESSION_EXPIRED = 16,
    SESSION_NOT_VALIDATED = 17,
    INVALID_USERNAME = 18,
    SERVER_TIMEOUT = 19,
    GUEST_ACCESS_FORBIDDEN = 20,
    CAPTCHA_NEEDED = 21,
    CAPTCHA_INVALID = 22,
    MISSING_PARAM = 23,
    INVALID_PARAM = 24,
    TOO_LARGE = 25,
    EXCLUSIVE = 26,
    RESOURCE_LIMIT_EXCEEDED = 27,
    UNKNOWN_TOKEN = 401,
    MISSING_TOKEN = 401,
    BAD_JSON_MATRIX = 403,
    NOT_FOUND_MATRIX = 404,
    LIMIT_EXCEEDED_MATRIX = 429,
    UNRECOGNIZED_MATRIX = 400
};

struct MatrixError : public std::exception {
    std::string message;
    ErrorCode code;

    MatrixError(const std::string& msg, ErrorCode c = ErrorCode::UNKNOWN)
        : message(msg), code(c) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

template<typename T>
class Result {
public:
    Result(const T& value) : value_(value), error_() {}
    Result(const MatrixError& error) : value_(), error_(error) {}

    bool success() const { return !error_.message.empty(); }
    const T& value() const { return value_; }
    const MatrixError& error() const { return error_; }

    explicit operator bool() const { return success(); }

private:
    T value_;
    MatrixError error_;
};

}
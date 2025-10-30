#pragma once

#include "event.hpp"
#include <memory>
#include <functional>

namespace matrix {

class EventParser {
public:
    EventParser() = default;
    ~EventParser() = default;

    static EventPtr parse(const nlohmann::json& data);
    static EventPtr parse(const std::string& json_string);

    static bool validate_event(const nlohmann::json& data);
    static bool validate_event(const EventPtr& event);

    static std::string extract_event_type(const nlohmann::json& data);
    static std::string extract_room_id(const nlohmann::json& data);
    static std::string extract_sender(const nlohmann::json& data);
    static std::string extract_state_key(const nlohmann::json& data);

    static bool is_state_event(const nlohmann::json& data);
    static bool is_message_event(const nlohmann::json& data);
    static bool is_room_event(const nlohmann::json& data);

    static EventType classify_event_type(const std::string& event_type);

    void set_strict_validation(bool strict) { strict_validation_ = strict; }
    bool strict_validation() const { return strict_validation_; }

    class Validator {
    public:
        static bool validate_room_event(const nlohmann::json& data);
        static bool validate_state_event(const nlohmann::json& data);
        static bool validate_message_event(const nlohmann::json& data);
        static bool validate_event_structure(const nlohmann::json& data);

        static std::vector<std::string> get_validation_errors(const nlohmann::json& data);
    };

private:
    bool strict_validation_ = true;

    static EventPtr parse_room_event(const nlohmann::json& data);
    static EventPtr parse_state_event(const nlohmann::json& data);
    static EventPtr parse_message_event(const nlohmann::json& data);
    static EventPtr parse_basic_event(const nlohmann::json& data);

    static void populate_common_fields(Event& event, const nlohmann::json& data);
    static void populate_room_event_fields(RoomEvent& event, const nlohmann::json& data);
};

}
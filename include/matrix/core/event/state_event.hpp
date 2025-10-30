#pragma once

#include "room_event.hpp"

namespace matrix {

class StateEvent : public RoomEvent {
public:
    StateEvent() = default;
    StateEvent(const RoomID& room_id, const std::string& state_key);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

    bool is_state_event() const { return true; }
};

class RoomCreateEvent : public StateEvent {
public:
    RoomCreateEvent() = default;
    RoomCreateEvent(const RoomID& room_id, const UserID& creator);

    UserID creator() const;
    std::string room_version() const;
    void set_creator(const UserID& creator);
    void set_room_version(const std::string& version);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

private:
    void ensure_content();
};

class RoomPowerLevelsEvent : public StateEvent {
public:
    RoomPowerLevelsEvent() = default;
    RoomPowerLevelsEvent(const RoomID& room_id);

    PowerLevels power_levels() const;
    void set_power_levels(const PowerLevels& levels);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

    int get_user_power_level(const UserID& user_id) const;
    int get_event_power_level(const std::string& event_type) const;

private:
    void ensure_content();
};

class RoomJoinRulesEvent : public StateEvent {
public:
    RoomJoinRulesEvent() = default;
    RoomJoinRulesEvent(const RoomID& room_id, const std::string& join_rule);

    std::string join_rule() const;
    void set_join_rule(const std::string& join_rule);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

private:
    void ensure_content();
};

class RoomEncryptionEvent : public StateEvent {
public:
    RoomEncryptionEvent() = default;
    RoomEncryptionEvent(const RoomID& room_id, const std::string& algorithm);

    std::string algorithm() const;
    void set_algorithm(const std::string& algorithm);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

private:
    void ensure_content();
};

}
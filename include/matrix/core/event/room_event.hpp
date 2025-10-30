#pragma once

#include "event.hpp"

namespace matrix {

class RoomEvent : public Event {
public:
    RoomEvent() = default;
    explicit RoomEvent(const RoomID& room_id) { room_id_ = room_id; }

    std::string state_key() const { return state_key_; }
    void set_state_key(const std::string& state_key) { state_key_ = state_key; }

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

    bool has_state_key() const { return state_key_.has_value(); }

private:
    std::optional<std::string> state_key_;
};

class RoomMessageEvent : public RoomEvent {
public:
    RoomMessageEvent() = default;
    RoomMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& body);

    std::string body() const;
    std::string msgtype() const;
    void set_body(const std::string& body);
    void set_msgtype(const std::string& msgtype);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

private:
    void ensure_content();
};

class RoomMemberEvent : public RoomEvent {
public:
    RoomMemberEvent() = default;
    RoomMemberEvent(const RoomID& room_id, const UserID& sender, const UserID& target, Membership membership);

    UserID target_user_id() const;
    Membership membership() const;
    std::string display_name() const;
    std::string avatar_url() const;

    void set_membership(Membership membership);
    void set_display_name(const std::string& display_name);
    void set_avatar_url(const std::string& avatar_url);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

private:
    void ensure_content();
};

}
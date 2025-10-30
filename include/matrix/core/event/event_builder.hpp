#pragma once

#include "event.hpp"
#include "room_event.hpp"
#include "state_event.hpp"
#include "message_event.hpp"
#include <memory>

namespace matrix {

class EventBuilder {
public:
    static EventPtr create_event(const std::string& type);
    static EventPtr create_event(const nlohmann::json& data);

    static std::shared_ptr<RoomEvent> create_room_event(const RoomID& room_id, const std::string& type);
    static std::shared_ptr<StateEvent> create_state_event(const RoomID& room_id, const std::string& type, const std::string& state_key = "");
    static std::shared_ptr<MessageEvent> create_message_event(const RoomID& room_id, const UserID& sender, const std::string& msgtype);

    static std::shared_ptr<RoomCreateEvent> create_room_create_event(const RoomID& room_id, const UserID& creator);
    static std::shared_ptr<RoomMemberEvent> create_room_member_event(const RoomID& room_id, const UserID& sender, const UserID& target, Membership membership);
    static std::shared_ptr<RoomPowerLevelsEvent> create_room_power_levels_event(const RoomID& room_id);
    static std::shared_ptr<RoomJoinRulesEvent> create_room_join_rules_event(const RoomID& room_id, const std::string& join_rule);
    static std::shared_ptr<RoomEncryptionEvent> create_room_encryption_event(const RoomID& room_id, const std::string& algorithm);

    static std::shared_ptr<TextMessageEvent> create_text_message_event(const RoomID& room_id, const UserID& sender, const std::string& body);
    static std::shared_ptr<NoticeMessageEvent> create_notice_message_event(const RoomID& room_id, const UserID& sender, const std::string& body);
    static std::shared_ptr<EmoteMessageEvent> create_emote_message_event(const RoomID& room_id, const UserID& sender, const std::string& body);
    static std::shared_ptr<ImageMessageEvent> create_image_message_event(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
    static std::shared_ptr<FileMessageEvent> create_file_message_event(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
    static std::shared_ptr<AudioMessageEvent> create_audio_message_event(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
    static std::shared_ptr<VideoMessageEvent> create_video_message_event(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);

    static EventPtr from_json(const nlohmann::json& data);
    static std::string generate_event_id();
    static int64_t current_timestamp();

private:
    static std::string generate_random_string(size_t length);
    static EventType parse_event_type(const std::string& type_str);
};

}
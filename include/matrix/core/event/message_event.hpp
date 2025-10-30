#pragma once

#include "room_event.hpp"

namespace matrix {

class MessageEvent : public RoomEvent {
public:
    MessageEvent() = default;
    MessageEvent(const RoomID& room_id, const UserID& sender, const std::string& msgtype);

    std::string msgtype() const;
    void set_msgtype(const std::string& msgtype);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;

protected:
    void ensure_content();
};

class TextMessageEvent : public MessageEvent {
public:
    TextMessageEvent() = default;
    TextMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& body);

    std::string body() const;
    void set_body(const std::string& body);

    std::string formatted_body() const;
    void set_formatted_body(const std::string& formatted_body);

    std::string format() const;
    void set_format(const std::string& format);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;
};

class NoticeMessageEvent : public TextMessageEvent {
public:
    NoticeMessageEvent() = default;
    NoticeMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& body);
};

class EmoteMessageEvent : public TextMessageEvent {
public:
    EmoteMessageEvent() = default;
    EmoteMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& body);
};

class MediaMessageEvent : public MessageEvent {
public:
    MediaMessageEvent() = default;
    MediaMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& msgtype);

    std::string url() const;
    void set_url(const std::string& url);

    Content info() const;
    void set_info(const Content& info);

    std::string filename() const;
    void set_filename(const std::string& filename);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;
};

class ImageMessageEvent : public MediaMessageEvent {
public:
    ImageMessageEvent() = default;
    ImageMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
};

class FileMessageEvent : public MediaMessageEvent {
public:
    FileMessageEvent() = default;
    FileMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
};

class AudioMessageEvent : public MediaMessageEvent {
public:
    AudioMessageEvent() = default;
    AudioMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
};

class VideoMessageEvent : public MediaMessageEvent {
public:
    VideoMessageEvent() = default;
    VideoMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& url, const Content& info);
};

class LocationMessageEvent : public MessageEvent {
public:
    LocationMessageEvent() = default;
    LocationMessageEvent(const RoomID& room_id, const UserID& sender, const std::string& body, const Content& geo_uri);

    Content geo_uri() const;
    void set_geo_uri(const Content& geo_uri);

    nlohmann::json to_json() const override;
    void from_json(const nlohmann::json& j) override;
};

}
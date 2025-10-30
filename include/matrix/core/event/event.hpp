#pragma once

#include "../matrix_types.hpp"
#include <string>
#include <memory>

namespace matrix {

    class Event {
    public:
        Event() = default;
        virtual ~Event() = default;

        EventID event_id() const { return event_id_; }
        RoomID room_id() const { return room_id_; }
        UserID sender() const { return sender_; }
        std::string type() const { return type_; }
        int64_t origin_server_ts() const { return origin_server_ts_; }
        UnsignedData unsigned_data() const { return unsigned_data_; }
        Content content() const { return content_; }

        void set_event_id(const EventID& id) { event_id_ = id; }
        void set_room_id(const RoomID& id) { room_id_ = id; }
        void set_sender(const UserID& sender) { sender_ = sender; }
        void set_type(const std::string& type) { type_ = type; }
        void set_origin_server_ts(int64_t ts) { origin_server_ts_ = ts; }
        void set_unsigned_data(const UnsignedData& data) { unsigned_data_ = data; }
        void set_content(const Content& content) { content_ = content; }

        virtual nlohmann::json to_json() const;
        virtual void from_json(const nlohmann::json& j);

        bool has_room_id() const { return !room_id_.empty(); }
        bool has_event_id() const { return !event_id_.empty(); }
        bool has_sender() const { return !sender_.empty(); }

    protected:
        EventID event_id_;
        RoomID room_id_;
        UserID sender_;
        std::string type_;
        int64_t origin_server_ts_ = 0;
        UnsignedData unsigned_data_;
        Content content_;
    };

    using EventPtr = std::shared_ptr<Event>;

}
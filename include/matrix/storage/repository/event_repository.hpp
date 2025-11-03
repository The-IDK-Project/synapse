#pragma once

#include "repository.hpp"
#include "../../core/event/event.hpp"
#include <vector>
#include <memory>

namespace matrix::storage::repository {

class EventRepository : public PaginatedRepository<core::Event> {
public:
    EventRepository() = default;
    virtual ~EventRepository() = default;

    virtual std::unique_ptr<core::Event> read_by_event_id(const core::EventID& event_id) = 0;
    virtual std::vector<std::unique_ptr<core::Event>> read_by_room_id(const core::RoomID& room_id, int limit = 100, const std::string& since_token = "") = 0;
    virtual std::vector<std::unique_ptr<core::Event>> read_by_sender(const core::UserID& sender, int limit = 100) = 0;
    virtual std::vector<std::unique_ptr<core::Event>> read_by_type(const std::string& event_type, int limit = 100) = 0;
    virtual std::vector<std::unique_ptr<core::Event>> read_state_events_for_room(const core::RoomID& room_id) = 0;
    virtual std::vector<std::unique_ptr<core::Event>> read_events_by_reference(const core::EventID& event_id, const std::string& relation_type = "") = 0;

    virtual bool create_batch(const std::vector<core::Event>& events) = 0;
    virtual bool update_unsigned_data(const core::EventID& event_id, const core::UnsignedData& unsigned_data) = 0;
    virtual bool add_relation(const core::EventID& from_event, const core::EventID& to_event, const std::string& relation_type) = 0;
    virtual bool remove_relations(const core::EventID& event_id) = 0;

    virtual std::vector<core::EventID> get_room_event_ids(const core::RoomID& room_id, int limit = 100, const std::string& since_token = "") = 0;
    virtual std::vector<core::EventID> get_event_references(const core::EventID& event_id, const std::string& relation_type = "") = 0;
    virtual std::unique_ptr<core::Event> get_room_state_event(const core::RoomID& room_id, const std::string& event_type, const std::string& state_key = "") = 0;

    virtual int64_t get_room_event_count(const core::RoomID& room_id) = 0;
    virtual int64_t get_event_depth(const core::EventID& event_id) = 0;
    virtual std::string get_latest_event_id_for_room(const core::RoomID& room_id) = 0;

    virtual bool delete_events_for_room(const core::RoomID& room_id) = 0;
    virtual bool delete_events_older_than(int64_t timestamp) = 0;
    virtual bool cleanup_orphaned_events() = 0;

    struct EventFilter {
        std::vector<core::RoomID> rooms;
        std::vector<core::UserID> senders;
        std::vector<std::string> types;
        std::vector<core::UserID> not_senders;
        std::vector<std::string> not_types;
        int64_t since_ts = 0;
        int64_t until_ts = 0;
        int limit = 100;
        bool contains_url = false;
    };

    virtual std::vector<std::unique_ptr<core::Event>> search_events(const EventFilter& filter) = 0;
};

}
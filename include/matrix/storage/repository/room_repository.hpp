#pragma once

#include "repository.hpp"
#include "../../core/room/room.hpp"
#include "../../core/event/event.hpp"
#include <vector>
#include <memory>

namespace matrix::storage::repository {

class RoomRepository : public CrudRepository<core::Room> {
public:
    RoomRepository() = default;
    virtual ~RoomRepository() = default;

    virtual std::unique_ptr<core::Room> read_by_alias(const std::string& room_alias) = 0;
    virtual std::vector<std::unique_ptr<core::Room>> read_by_creator(const core::UserID& creator) = 0;
    virtual std::vector<std::unique_ptr<core::Room>> read_public_rooms(int limit = 100, const std::string& since_token = "") = 0;
    virtual std::vector<std::unique_ptr<core::Room>> read_rooms_for_user(const core::UserID& user_id, core::Membership membership = core::Membership::JOIN) = 0;

    virtual bool set_room_alias(const core::RoomID& room_id, const std::string& room_alias) = 0;
    virtual bool remove_room_alias(const std::string& room_alias) = 0;
    virtual std::string get_room_alias(const core::RoomID& room_id) = 0;
    virtual core::RoomID resolve_room_alias(const std::string& room_alias) = 0;

    virtual bool add_room_member(const core::RoomID& room_id, const core::UserID& user_id, core::Membership membership) = 0;
    virtual bool update_room_member(const core::RoomID& room_id, const core::UserID& user_id, core::Membership membership) = 0;
    virtual bool remove_room_member(const core::RoomID& room_id, const core::UserID& user_id) = 0;
    virtual core::Membership get_room_membership(const core::RoomID& room_id, const core::UserID& user_id) = 0;
    virtual std::vector<core::UserID> get_room_members(const core::RoomID& room_id, core::Membership membership = core::Membership::JOIN) = 0;
    virtual std::vector<core::UserID> get_room_members_with_power_level(const core::RoomID& room_id, int min_power_level) = 0;

    virtual bool set_room_power_levels(const core::RoomID& room_id, const core::PowerLevels& power_levels) = 0;
    virtual std::unique_ptr<core::PowerLevels> get_room_power_levels(const core::RoomID& room_id) = 0;
    virtual int get_user_power_level(const core::RoomID& room_id, const core::UserID& user_id) = 0;

    virtual bool set_room_state(const core::RoomID& room_id, const std::vector<core::Event>& state_events) = 0;
    virtual std::vector<std::unique_ptr<core::Event>> get_room_state(const core::RoomID& room_id) = 0;
    virtual std::unique_ptr<core::Event> get_room_state_event(const core::RoomID& room_id, const std::string& event_type, const std::string& state_key = "") = 0;

    virtual bool set_room_visibility(const core::RoomID& room_id, bool is_public) = 0;
    virtual bool get_room_visibility(const core::RoomID& room_id) = 0;

    virtual bool set_room_encryption(const core::RoomID& room_id, bool is_encrypted, const std::string& algorithm = "") = 0;
    virtual bool is_room_encrypted(const core::RoomID& room_id) = 0;
    virtual std::string get_room_encryption_algorithm(const core::RoomID& room_id) = 0;

    virtual bool set_room_version(const core::RoomID& room_id, const std::string& version) = 0;
    virtual std::string get_room_version(const core::RoomID& room_id) = 0;

    virtual int64_t get_room_member_count(const core::RoomID& room_id, core::Membership membership = core::Membership::JOIN) = 0;
    virtual int64_t get_total_room_count() = 0;
    virtual int64_t get_public_room_count() = 0;

    virtual bool cleanup_orphaned_rooms() = 0;
    virtual bool delete_room_data(const core::RoomID& room_id) = 0;

    struct RoomSummary {
        core::RoomID room_id;
        std::string name;
        std::string topic;
        std::string avatar_url;
        int64_t joined_members_count;
        core::UserID creator;
        bool is_encrypted;
        bool is_public;
        int64_t created_ts;
    };

    virtual std::unique_ptr<RoomSummary> get_room_summary(const core::RoomID& room_id) = 0;
    virtual std::vector<RoomSummary> get_public_room_summaries(int limit = 100, const std::string& since_token = "") = 0;
};

}
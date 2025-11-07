#pragma once

#include "../database.hpp"
#include "../database_connection.hpp"
#include <libpq-fe.h>
#include <memory>
#include <unordered_map>

namespace matrix::storage::database::postgresql {

class PostgreSQLDatabase : public Database {
public:
    PostgreSQLDatabase(const DatabaseConfig& config);
    ~PostgreSQLDatabase();

    bool initialize() override;
    bool shutdown() override;
    bool is_connected() const override;

    bool begin_transaction() override;
    bool commit_transaction() override;
    bool rollback_transaction() override;
    bool is_in_transaction() const override;

    bool backup(const std::string& backup_path) override;
    bool restore(const std::string& backup_path) override;
    bool vacuum() override;

    std::string database_version() const override;
    int64_t database_size() const override;
    int64_t last_insert_id() const override;

    std::shared_ptr<repository::EventRepository> event_repository() override;
    std::shared_ptr<repository::RoomRepository> room_repository() override;
    std::shared_ptr<repository::UserRepository> user_repository() override;
    std::shared_ptr<repository::DeviceRepository> device_repository() override;

    DatabaseStats get_stats() const override;
    nlohmann::json get_detailed_stats() const override;

    bool run_migration(const std::string& migration_sql) override;
    bool check_migration_version() const override;
    int get_current_migration_version() const override;

private:
    DatabaseConfig config_;
    std::unique_ptr<ConnectionPool> connection_pool_;

    std::shared_ptr<repository::EventRepository> event_repository_;
    std::shared_ptr<repository::RoomRepository> room_repository_;
    std::shared_ptr<repository::UserRepository> user_repository_;
    std::shared_ptr<repository::DeviceRepository> device_repository_;

    bool create_tables();
    bool create_indexes();
    bool setup_prepared_statements();

    std::string build_connection_string() const;
};

class PostgreSQLEventRepository : public repository::EventRepository {
public:
    PostgreSQLEventRepository(std::shared_ptr<ConnectionPool> connection_pool);
    ~PostgreSQLEventRepository();

    bool initialize() override;
    bool shutdown() override;
    bool is_connected() const override;

    bool create(const core::Event& entity) override;
    std::unique_ptr<core::Event> read(const std::string& id) override;
    bool update(const core::Event& entity) override;
    bool remove(const std::string& id) override;
    bool exists(const std::string& id) const override;

    PaginationResult read_all(const std::string& start_token = "", int limit = 100) override;
    std::vector<core::Event> read_by_ids(const std::vector<std::string>& ids) override;

    std::unique_ptr<core::Event> read_by_event_id(const core::EventID& event_id) override;
    std::vector<std::unique_ptr<core::Event>> read_by_room_id(const core::RoomID& room_id, int limit = 100, const std::string& since_token = "") override;
    std::vector<std::unique_ptr<core::Event>> read_by_sender(const core::UserID& sender, int limit = 100) override;
    std::vector<std::unique_ptr<core::Event>> read_by_type(const std::string& event_type, int limit = 100) override;
    std::vector<std::unique_ptr<core::Event>> read_state_events_for_room(const core::RoomID& room_id) override;
    std::vector<std::unique_ptr<core::Event>> read_events_by_reference(const core::EventID& event_id, const std::string& relation_type = "") override;

    bool create_batch(const std::vector<core::Event>& events) override;
    bool update_unsigned_data(const core::EventID& event_id, const core::UnsignedData& unsigned_data) override;
    bool add_relation(const core::EventID& from_event, const core::EventID& to_event, const std::string& relation_type) override;
    bool remove_relations(const core::EventID& event_id) override;

    std::vector<core::EventID> get_room_event_ids(const core::RoomID& room_id, int limit = 100, const std::string& since_token = "") override;
    std::vector<core::EventID> get_event_references(const core::EventID& event_id, const std::string& relation_type = "") override;
    std::unique_ptr<core::Event> get_room_state_event(const core::RoomID& room_id, const std::string& event_type, const std::string& state_key = "") override;

    int64_t get_room_event_count(const core::RoomID& room_id) override;
    int64_t get_event_depth(const core::EventID& event_id) override;
    std::string get_latest_event_id_for_room(const core::RoomID& room_id) override;

    bool delete_events_for_room(const core::RoomID& room_id) override;
    bool delete_events_older_than(int64_t timestamp) override;
    bool cleanup_orphaned_events() override;

    std::vector<std::unique_ptr<core::Event>> search_events(const EventFilter& filter) override;

private:
    std::shared_ptr<ConnectionPool> connection_pool_;

    core::Event parse_event_from_row(const std::vector<std::string>& row) const;
    std::vector<std::string> serialize_event_for_insert(const core::Event& event) const;
};

class PostgreSQLRoomRepository : public repository::RoomRepository {
public:
    PostgreSQLRoomRepository(std::shared_ptr<ConnectionPool> connection_pool);
    ~PostgreSQLRoomRepository();

    bool initialize() override;
    bool shutdown() override;
    bool is_connected() const override;

    bool create(const core::Room& entity) override;
    std::unique_ptr<core::Room> read(const std::string& id) override;
    bool update(const core::Room& entity) override;
    bool remove(const std::string& id) override;
    bool exists(const std::string& id) const override;

    std::unique_ptr<core::Room> read_by_alias(const std::string& room_alias) override;
    std::vector<std::unique_ptr<core::Room>> read_by_creator(const core::UserID& creator) override;
    std::vector<std::unique_ptr<core::Room>> read_public_rooms(int limit = 100, const std::string& since_token = "") override;
    std::vector<std::unique_ptr<core::Room>> read_rooms_for_user(const core::UserID& user_id, core::Membership membership = core::Membership::JOIN) override;

    bool set_room_alias(const core::RoomID& room_id, const std::string& room_alias) override;
    bool remove_room_alias(const std::string& room_alias) override;
    std::string get_room_alias(const core::RoomID& room_id) override;
    core::RoomID resolve_room_alias(const std::string& room_alias) override;

    bool add_room_member(const core::RoomID& room_id, const core::UserID& user_id, core::Membership membership) override;
    bool update_room_member(const core::RoomID& room_id, const core::UserID& user_id, core::Membership membership) override;
    bool remove_room_member(const core::RoomID& room_id, const core::UserID& user_id) override;
    core::Membership get_room_membership(const core::RoomID& room_id, const core::UserID& user_id) override;
    std::vector<core::UserID> get_room_members(const core::RoomID& room_id, core::Membership membership = core::Membership::JOIN) override;
    std::vector<core::UserID> get_room_members_with_power_level(const core::RoomID& room_id, int min_power_level) override;

    bool set_room_power_levels(const core::RoomID& room_id, const core::PowerLevels& power_levels) override;
    std::unique_ptr<core::PowerLevels> get_room_power_levels(const core::RoomID& room_id) override;
    int get_user_power_level(const core::RoomID& room_id, const core::UserID& user_id) override;

    bool set_room_state(const core::RoomID& room_id, const std::vector<core::Event>& state_events) override;
    std::vector<std::unique_ptr<core::Event>> get_room_state(const core::RoomID& room_id) override;
    std::unique_ptr<core::Event> get_room_state_event(const core::RoomID& room_id, const std::string& event_type, const std::string& state_key = "") override;

    bool set_room_visibility(const core::RoomID& room_id, bool is_public) override;
    bool get_room_visibility(const core::RoomID& room_id) override;

    bool set_room_encryption(const core::RoomID& room_id, bool is_encrypted, const std::string& algorithm = "") override;
    bool is_room_encrypted(const core::RoomID& room_id) override;
    std::string get_room_encryption_algorithm(const core::RoomID& room_id) override;

    bool set_room_version(const core::RoomID& room_id, const std::string& version) override;
    std::string get_room_version(const core::RoomID& room_id) override;

    int64_t get_room_member_count(const core::RoomID& room_id, core::Membership membership = core::Membership::JOIN) override;
    int64_t get_total_room_count() override;
    int64_t get_public_room_count() override;

    bool cleanup_orphaned_rooms() override;
    bool delete_room_data(const core::RoomID& room_id) override;

    std::unique_ptr<RoomSummary> get_room_summary(const core::RoomID& room_id) override;
    std::vector<RoomSummary> get_public_room_summaries(int limit = 100, const std::string& since_token = "") override;

private:
    std::shared_ptr<ConnectionPool> connection_pool_;

    core::Room parse_room_from_row(const std::vector<std::string>& row) const;
    std::vector<std::string> serialize_room_for_insert(const core::Room& room) const;
};

}
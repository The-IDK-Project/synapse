#pragma once

#include "migration_manager.hpp"
#include <vector>

namespace matrix::storage::migration {

class BuiltinMigrations {
public:
    static std::vector<Migration> get_all_migrations();
    static std::vector<Migration> get_migrations_up_to(int version);
    static std::unique_ptr<Migration> get_migration(int version);

private:
    static Migration create_initial_schema_migration();
    static Migration create_events_table_migration();
    static Migration create_rooms_table_migration();
    static Migration create_users_table_migration();
    static Migration create_devices_table_migration();
    static Migration create_room_members_table_migration();
    static Migration create_room_state_table_migration();
    static Migration create_event_relations_table_migration();
    static Migration create_room_aliases_table_migration();
    static Migration create_user_account_data_migration();
    static Migration create_user_presence_migration();
    static Migration create_push_rules_migration();
    static Migration create_federation_tables_migration();
    static Migration create_e2ee_tables_migration();
    static Migration create_media_repository_migration();
    static Migration create_optimization_indexes_migration();
    static Migration create_advanced_features_migration();
};

class MigrationSQL {
public:
    static const std::string CREATE_MIGRATIONS_TABLE;
    static const std::string CREATE_EVENTS_TABLE;
    static const std::string CREATE_ROOMS_TABLE;
    static const std::string CREATE_USERS_TABLE;
    static const std::string CREATE_DEVICES_TABLE;
    static const std::string CREATE_ROOM_MEMBERS_TABLE;
    static const std::string CREATE_ROOM_STATE_TABLE;
    static const std::string CREATE_ROOM_ALIASES_TABLE;
    static const std::string CREATE_ROOM_POWER_LEVELS_TABLE;
    static const std::string CREATE_USER_ACCOUNT_DATA_TABLE;
    static const std::string CREATE_USER_PRESENCE_TABLE;
    static const std::string CREATE_PUSH_RULES_TABLE;
    static const std::string CREATE_USER_FILTERS_TABLE;
    static const std::string CREATE_USER_TAGS_TABLE;
    static const std::string CREATE_EVENT_RELATIONS_TABLE;
    static const std::string CREATE_EVENT_EDGES_TABLE;
    static const std::string CREATE_EVENT_AUTH_CHAIN_TABLE;
    static const std::string CREATE_FEDERATION_QUEUES_TABLE;
    static const std::string CREATE_FEDERATION_PDU_ORIGIN_TABLE;
    static const std::string CREATE_FEDERATION_TRANSACTIONS_TABLE;
    static const std::string CREATE_FEDERATION_STATE_SERVERS_TABLE;
    static const std::string CREATE_FEDERATION_INVITES_TABLE;
    static const std::string CREATE_DEVICE_KEYS_TABLE;
    static const std::string CREATE_ONE_TIME_KEYS_TABLE;
    static const std::string CREATE_OLM_SESSIONS_TABLE;
    static const std::string CREATE_MEGOLM_SESSIONS_TABLE;
    static const std::string CREATE_MEGOLM_INBOUND_SESSIONS_TABLE;
    static const std::string CREATE_KEY_BACKUPS_TABLE;
    static const std::string CREATE_CROSS_SIGNING_KEYS_TABLE;
    static const std::string CREATE_MEDIA_REPOSITORY_TABLE;
    static const std::string CREATE_MEDIA_THUMBNAILS_TABLE;
    static const std::string CREATE_EVENTS_INDEXES;
    static const std::string CREATE_ROOMS_INDEXES;
    static const std::string CREATE_USERS_INDEXES;
    static const std::string CREATE_DEVICES_INDEXES;
    static const std::string CREATE_FEDERATION_INDEXES;
    static const std::string CREATE_E2EE_INDEXES;
    static const std::string CREATE_SPACES_TABLE;
    static const std::string CREATE_THREADS_TABLE;
    static const std::string CREATE_REACTIONS_TABLE;
    static const std::string CREATE_POLLS_TABLE;
    static const std::string CREATE_LOCATIONS_TABLE;
    static const std::string DROP_MIGRATIONS_TABLE;
    static const std::string DROP_EVENTS_TABLE;
    static const std::string DROP_ROOMS_TABLE;
    static const std::string DROP_USERS_TABLE;
    static const std::string DROP_DEVICES_TABLE;
};

class MigrationValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };

    static ValidationResult validate_migration(const Migration& migration);
    static ValidationResult validate_sql_syntax(const std::string& sql);
    static ValidationResult validate_sql_safety(const std::string& sql);

    static bool is_destructive_operation(const std::string& sql);
    static bool contains_dangerous_operations(const std::string& sql);
    static bool is_reversible_migration(const Migration& migration);

private:
    static const std::vector<std::string> DANGEROUS_KEYWORDS;
    static const std::vector<std::string> DESTRUCTIVE_OPERATIONS;
};

}
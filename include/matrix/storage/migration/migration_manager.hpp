#pragma once

#include "../database/database.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace matrix::storage::migration {

struct Migration {
    int version;
    std::string name;
    std::string description;
    std::string up_sql;
    std::string down_sql;
    Timestamp created_ts;
};

class MigrationManager {
public:
    MigrationManager(std::shared_ptr<database::Database> database);
    ~MigrationManager();

    bool initialize();
    bool shutdown();

    bool migrate_to_version(int target_version = -1);
    bool rollback_to_version(int target_version);
    bool rollback_last();

    bool add_migration(const Migration& migration);
    bool remove_migration(int version);

    int get_current_version() const;
    int get_latest_version() const;
    std::vector<int> get_applied_versions() const;
    std::vector<int> get_pending_versions() const;

    std::vector<Migration> get_migrations() const;
    std::unique_ptr<Migration> get_migration(int version) const;

    bool validate_migrations() const;
    bool has_pending_migrations() const;

    struct MigrationStatus {
        int current_version;
        int latest_version;
        int applied_count;
        int pending_count;
        bool initialized;
        std::string last_error;
    };

    MigrationStatus get_status() const;
    nlohmann::json get_detailed_status() const;

    bool create_migration_table();
    bool drop_migration_table();

private:
    std::shared_ptr<database::Database> database_;
    std::vector<Migration> migrations_;
    std::unordered_map<int, Migration> migration_map_;

    bool execute_migration(const Migration& migration);
    bool execute_rollback(const Migration& migration);
    bool record_migration_applied(const Migration& migration);
    bool record_migration_rolled_back(const Migration& migration);

    void load_migrations_from_files(const std::string& migrations_dir = "migrations");
    void load_builtin_migrations();

    bool verify_migration_hash(const Migration& migration) const;
    std::string calculate_migration_hash(const Migration& migration) const;

    mutable std::mutex migration_mutex_;
};

class MigrationBuilder {
public:
    MigrationBuilder();
    ~MigrationBuilder();

    MigrationBuilder& set_version(int version);
    MigrationBuilder& set_name(const std::string& name);
    MigrationBuilder& set_description(const std::string& description);
    MigrationBuilder& setUpSQL(const std::string& up_sql);
    MigrationBuilder& setDownSQL(const std::string& down_sql);

    Migration build();

    static MigrationBuilder create();

private:
    Migration migration_;
};

class MigrationExecutor {
public:
    MigrationExecutor(std::shared_ptr<database::Database> database);
    ~MigrationExecutor();

    bool execute_sql(const std::string& sql);
    bool execute_sql_batch(const std::vector<std::string>& sql_statements);
    bool execute_sql_file(const std::string& file_path);

    bool verify_sql(const std::string& sql);
    bool validate_sql_syntax(const std::string& sql);

    struct ExecutionResult {
        bool success;
        std::string error;
        int affected_rows;
        int64_t execution_time_ms;
    };

    ExecutionResult execute_with_result(const std::string& sql);

private:
    std::shared_ptr<database::Database> database_;
};

}
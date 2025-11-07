#pragma once

#include "../repository/repository.hpp"
#include "../repository/event_repository.hpp"
#include "../repository/room_repository.hpp"
#include "../repository/user_repository.hpp"
#include "../repository/device_repository.hpp"
#include <memory>
#include <string>

namespace matrix::storage::database {

class Database {
public:
    Database() = default;
    virtual ~Database() = default;

    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool is_connected() const = 0;

    virtual bool begin_transaction() = 0;
    virtual bool commit_transaction() = 0;
    virtual bool rollback_transaction() = 0;
    virtual bool is_in_transaction() const = 0;

    virtual bool backup(const std::string& backup_path) = 0;
    virtual bool restore(const std::string& backup_path) = 0;
    virtual bool vacuum() = 0;

    virtual std::string database_version() const = 0;
    virtual int64_t database_size() const = 0;
    virtual int64_t last_insert_id() const = 0;

    virtual std::shared_ptr<repository::EventRepository> event_repository() = 0;
    virtual std::shared_ptr<repository::RoomRepository> room_repository() = 0;
    virtual std::shared_ptr<repository::UserRepository> user_repository() = 0;
    virtual std::shared_ptr<repository::DeviceRepository> device_repository() = 0;

    struct DatabaseStats {
        int64_t total_size;
        int64_t table_count;
        int64_t event_count;
        int64_t room_count;
        int64_t user_count;
        int64_t device_count;
        int64_t connection_count;
        std::string version;
        int64_t uptime_seconds;
    };

    virtual DatabaseStats get_stats() const = 0;
    virtual nlohmann::json get_detailed_stats() const = 0;

    virtual bool run_migration(const std::string& migration_sql) = 0;
    virtual bool check_migration_version() const = 0;
    virtual int get_current_migration_version() const = 0;

protected:
    std::string last_error_;
};

class DatabaseFactory {
public:
    enum DatabaseType {
        POSTGRESQL,
        SQLITE,
        MYSQL
    };

    struct DatabaseConfig {
        DatabaseType type;
        std::string connection_string;
        std::string host;
        int port;
        std::string database_name;
        std::string username;
        std::string password;
        int connection_pool_size;
        int connection_timeout;
        bool  enable_ssl;
        std::string ssl_cert;
        std::string ssl_key;
        std::string ssl_ca;
    };

    static std::unique_ptr<Database> create_database(const DatabaseConfig& config);
    static std::unique_ptr<Database> create_database_from_config(const nlohmann::json& config);

private:
    static std::unique_ptr<Database> create_postgresql_database(const DatabaseConfig& config);
    static std::unique_ptr<Database> create_sqlite_database(const DatabaseConfig& config);
    static std::unique_ptr<Database> create_mysql_database(const DatabaseConfig& config);
};

}
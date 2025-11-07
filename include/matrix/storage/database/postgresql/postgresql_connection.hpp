#pragma once

#include "../database_connection.hpp"
#include <libpq-fe.h>
#include <memory>
#include <vector>

namespace matrix::storage::database::postgresql {

class PostgreSQLConnection : public DatabaseConnection {
public:
    PostgreSQLConnection(const std::string& connection_string);
    ~PostgreSQLConnection();

    bool connect() override;
    bool disconnect() override;
    bool is_connected() const override;
    bool ping() override;

    QueryResult execute(const std::string& sql) override;
    QueryResult execute(const std::string& sql, const std::vector<std::string>& params) override;
    QueryResult execute_prepared(const std::string& statement_name, const std::vector<std::string>& params) override;

    bool prepare_statement(const std::string& name, const std::string& sql, const std::vector<std::string>& param_types = {}) override;
    bool unprepare_statement(const std::string& name) override;
    bool has_prepared_statement(const std::string& name) const override;

    bool begin_transaction() override;
    bool commit_transaction() override;
    bool rollback_transaction() override;
    bool is_in_transaction() const override;

    bool set_auto_commit(bool auto_commit) override;
    bool get_auto_commit() const override;

    std::string last_error() const override;
    int last_error_code() const override;

    int get_timeout() const override;
    bool set_timeout(int timeout_ms) override;

    std::string get_client_version() const override;
    std::string get_server_version() const override;

    bool backup(const std::string& backup_path) override;
    bool restore(const std::string& backup_path) override;

    ConnectionInfo get_connection_info() const override;

private:
    PGconn* connection_ = nullptr;
    std::unordered_map<std::string, std::string> prepared_statements_;

    mutable std::mutex connection_mutex_;
    std::atomic<int> total_queries_{0};
    std::atomic<int> failed_queries_{0};

    bool check_connection() const;
    void clear_results(PGresult* result);
    QueryResult process_result(PGresult* result);
    std::vector<std::string> get_column_names(PGresult* result) const;
    std::vector<std::vector<std::string>> get_rows(PGresult* result) const;

    Oid pg_type_from_string(const std::string& type_name) const;
    std::string escape_string(const std::string& str) const;
    std::string escape_identifier(const std::string& identifier) const;
};

class PostgreSQLConnectionFactory {
public:
    static std::unique_ptr<DatabaseConnection> create_connection(const std::string& connection_string) {
        return std::make_unique<PostgreSQLConnection>(connection_string);
    }
};

class PostgreSQLBatchOperation : public BatchOperation {
public:
    PostgreSQLBatchOperation(DatabaseConnection& connection, const std::string& base_sql, size_t batch_size = 1000);
    ~PostgreSQLBatchOperation();

    bool add_batch(const std::vector<std::string>& params) override;
    bool execute_batch() override;
    bool flush() override;

private:
    std::string build_batch_sql() const;
};

class PostgreSQLBackupManager {
public:
    PostgreSQLBackupManager(const std::string& connection_string);
    ~PostgreSQLBackupManager();

    bool create_backup(const std::string& backup_path);
    bool restore_backup(const std::string& backup_path);
    bool verify_backup(const std::string& backup_path);

    struct BackupInfo {
        std::string backup_path;
        int64_t backup_size;
        Timestamp backup_time;
        std::string database_version;
        bool verified;
    };

    std::vector<BackupInfo> list_backups(const std::string& backup_dir);
    bool cleanup_old_backups(const std::string& backup_dir, int keep_count);

private:
    std::string connection_string_;
    std::string pg_dump_path_;
    std::string pg_restore_path_;

    bool find_postgresql_tools();
    std::string build_pg_dump_command(const std::string& backup_path) const;
    std::string build_pg_restore_command(const std::string& backup_path) const;
    bool execute_system_command(const std::string& command);
};

}
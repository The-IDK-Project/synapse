#pragma once

#include "../database_connection.hpp"
#include <sqlite3.h>
#include <memory>
#include <vector>

namespace matrix::storage::database::sqlite {

class SQLiteConnection : public DatabaseConnection {
public:
    SQLiteConnection(const std::string& database_path);
    ~SQLiteConnection();

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

    bool enable_foreign_keys(bool enable);
    bool enable_wal_mode(bool enable);
    bool set_journal_mode(const std::string& mode);
    bool set_synchronous_mode(const std::string& mode);
    bool set_temp_store(const std::string& store);
    bool set_cache_size(int size);
    bool set_page_size(int size);

private:
    sqlite3* db_ = nullptr;
    std::string database_path_;
    std::unordered_map<std::string, sqlite3_stmt*> prepared_statements_;

    mutable std::mutex connection_mutex_;
    std::atomic<int> total_queries_{0};
    std::atomic<int> failed_queries_{0};

    bool check_connection() const;
    QueryResult execute_impl(const std::string& sql, const std::vector<std::string>& params = {});
    QueryResult execute_prepared_impl(sqlite3_stmt* stmt, const std::vector<std::string>& params);
    void clear_statement(sqlite3_stmt* stmt);

    std::vector<std::string> get_column_names(sqlite3_stmt* stmt) const;
    std::vector<std::vector<std::string>> get_rows(sqlite3_stmt* stmt) const;

    bool set_pragma(const std::string& pragma, const std::string& value);
    std::string get_pragma(const std::string& pragma) const;
};

class SQLiteConnectionFactory {
public:
    static std::unique_ptr<DatabaseConnection> create_connection(const std::string& database_path) {
        return std::make_unique<SQLiteConnection>(database_path);
    }
};

class SQLiteBatchOperation : public BatchOperation {
public:
    SQLiteBatchOperation(DatabaseConnection& connection, const std::string& base_sql, size_t batch_size = 1000);
    ~SQLiteBatchOperation();

    bool add_batch(const std::vector<std::string>& params) override;
    bool execute_batch() override;
    bool flush() override;

private:
    bool execute_current_batch();
};

class SQLiteBackupManager {
public:
    SQLiteBackupManager(const std::string& database_path);
    ~SQLiteBackupManager();

    bool create_backup(const std::string& backup_path);
    bool restore_backup(const std::string& backup_path);
    bool verify_backup(const std::string& backup_path);
    bool create_incremental_backup(const std::string& backup_path);
    bool restore_incremental_backup(const std::string& backup_path);

    struct BackupInfo {
        std::string backup_path;
        int64_t backup_size;
        Timestamp backup_time;
        std::string database_version;
        std::string checksum;
        bool verified;
    };

    std::vector<BackupInfo> list_backups(const std::string& backup_dir);
    bool cleanup_old_backups(const std::string& backup_dir, int keep_count);
    bool optimize_database();

private:
    std::string database_path_;

    bool perform_vacuum();
    bool perform_analyze();
    bool perform_integrity_check();
    std::string calculate_checksum(const std::string& file_path);
    bool verify_database_integrity(const std::string& db_path);
};

class SQLiteVFS {
public:
    static bool register_custom_vfs(const std::string& vfs_name);
    static bool unregister_custom_vfs(const std::string& vfs_name);

    static bool enable_encryption(const std::string& database_path, const std::string& key);
    static bool disable_encryption(const std::string& database_path);
    static bool change_encryption_key(const std::string& database_path, const std::string& new_key);

    static bool compress_database(const std::string& database_path);
    static bool decompress_database(const std::string& database_path);
};

}
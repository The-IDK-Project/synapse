#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace matrix::storage::database {

class DatabaseConnection {
public:
    struct QueryResult {
        bool success;
        std::vector<std::vector<std::string>> rows;
        std::vector<std::string> columns;
        int affected_rows;
        std::string error_message;
        int64_t last_insert_id;
    };

    struct PreparedStatement {
        std::string name;
        std::string sql;
        std::vector<std::string> param_types;
    };

    DatabaseConnection() = default;
    virtual ~DatabaseConnection() = default;

    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool is_connected() const = 0;
    virtual bool ping() = 0;

    virtual QueryResult execute(const std::string& sql) = 0;
    virtual QueryResult execute(const std::string& sql, const std::vector<std::string>& params) = 0;
    virtual QueryResult execute_prepared(const std::string& statement_name, const std::vector<std::string>& params) = 0;

    virtual bool prepare_statement(const std::string& name, const std::string& sql, const std::vector<std::string>& param_types = {}) = 0;
    virtual bool unprepare_statement(const std::string& name) = 0;
    virtual bool has_prepared_statement(const std::string& name) const = 0;

    virtual bool begin_transaction() = 0;
    virtual bool commit_transaction() = 0;
    virtual bool rollback_transaction() = 0;
    virtual bool is_in_transaction() const = 0;

    virtual bool set_auto_commit(bool auto_commit) = 0;
    virtual bool get_auto_commit() const = 0;

    virtual std::string last_error() const = 0;
    virtual int last_error_code() const = 0;

    virtual int get_timeout() const = 0;
    virtual bool set_timeout(int timeout_ms) = 0;

    virtual std::string get_client_version() const = 0;
    virtual std::string get_server_version() const = 0;

    virtual bool backup(const std::string& backup_path) = 0;
    virtual bool restore(const std::string& backup_path) = 0;

    struct ConnectionInfo {
        std::string client_version;
        std::string server_version;
        std::string connection_string;
        int timeout_ms;
        bool is_connected;
        bool in_transaction;
        bool auto_commit;
        int64_t total_queries;
        int64_t failed_queries;
    };

    virtual ConnectionInfo get_connection_info() const = 0;

protected:
    std::string connection_string_;
    int timeout_ms_ = 30000;
    bool auto_commit_ = true;
};

class ConnectionPool {
public:
    struct PoolConfig {
        int min_connections;
        int max_connections;
        int connection_timeout;
        int idle_timeout;
        int validation_interval;
        bool test_on_borrow;
        bool test_on_return;
    };

    ConnectionPool(std::unique_ptr<DatabaseConnection> (*connection_factory)(), const PoolConfig& config);
    ~ConnectionPool();

    std::unique_ptr<DatabaseConnection> acquire_connection();
    bool release_connection(std::unique_ptr<DatabaseConnection> connection);

    bool initialize();
    bool shutdown();

    int get_active_connections() const;
    int getIdle_connections() const;
    int get_total_connections() const;

    struct PoolStats {
        int active_connections;
        int idle_connections;
        int total_connections;
        int max_connections;
        int acquire_timeouts;
        int validation_errors;
    };

    PoolStats get_stats() const;

private:
    std::unique_ptr<DatabaseConnection> (*connection_factory_)();
    PoolConfig config_;

    mutable std::mutex pool_mutex_;
    std::condition_variable pool_condition_;
    std::vector<std::unique_ptr<DatabaseConnection>> idle_connections_;
    std::vector<std::unique_ptr<DatabaseConnection>> active_connections_;
    std::atomic<int> total_connections_{0};
    std::atomic<int> acquire_timeouts_{0};
    std::atomic<int> validation_errors_{0};

    bool running_ = false;
    std::thread maintenance_thread_;

    void maintenance_worker();
    bool validate_connection(DatabaseConnection* connection);
    void cleanup_idle_connections();
};

class Transaction {
public:
    Transaction(DatabaseConnection& connection);
    ~Transaction();

    bool begin();
    bool commit();
    bool rollback();
    bool is_active() const;

private:
    DatabaseConnection& connection_;
    bool active_ = false;
};

class BatchOperation {
public:
    BatchOperation(DatabaseConnection& connection, const std::string& base_sql, size_t batch_size = 1000);
    ~BatchOperation();

    bool add_batch(const std::vector<std::string>& params);
    bool execute_batch();
    bool flush();

    size_t get_batch_count() const;
    size_t get_total_operations() const;

private:
    DatabaseConnection& connection_;
    std::string base_sql_;
    size_t batch_size_;
    std::vector<std::vector<std::string>> current_batch_;
    size_t total_operations_ = 0;
};

}
#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace matrix::storage::database::sqlite {

enum class SQLiteType {
    INTEGER,
    REAL,
    TEXT,
    BLOB,
    NUMERIC,
    BOOLEAN,
    DATE,
    DATETIME,
    ANY
};

struct SQLiteColumn {
    std::string name;
    SQLiteType type;
    bool not_null;
    bool primary_key;
    bool auto_increment;
    std::string default_value;
    std::string check_constraint;
    std::string collation;
};

struct SQLiteTable {
    std::string name;
    std::vector<SQLiteColumn> columns;
    std::vector<std::string> primary_keys;
    std::vector<std::string> indexes;
    std::vector<std::string> foreign_keys;
    bool without_rowid;
    bool strict;
};

struct SQLiteIndex {
    std::string name;
    std::string table_name;
    std::vector<std::string> columns;
    bool unique;
    bool partial;
    std::string where_clause;
    std::string collation;
};

struct SQLiteForeignKey {
    std::string name;
    std::string from_table;
    std::vector<std::string> from_columns;
    std::string to_table;
    std::vector<std::string> to_columns;
    std::string on_delete;
    std::string on_update;
    bool deferred;
};

class SQLiteTypeConverter {
public:
    static std::string to_string(SQLiteType type);
    static SQLiteType from_string(const std::string& type_str);
    static std::string get_type_definition(SQLiteType type);
    static std::string get_affinity(SQLiteType type);

    static std::string escape_value(const std::string& value, SQLiteType type);
    static std::string escape_identifier(const std::string& identifier);
    static std::string escape_literal(const std::string& literal);

    static nlohmann::json sql_to_json(const std::string& sql_value, SQLiteType type);
    static std::string json_to_sql(const nlohmann::json& json_value, SQLiteType type);

    static bool validate_value(const std::string& value, SQLiteType type);
    static bool is_numeric_type(SQLiteType type);
    static bool is_text_type(SQLiteType type);
    static bool is_blob_type(SQLiteType type);
    static bool is_integer_type(SQLiteType type);
};

class SQLiteSchemaManager {
public:
    SQLiteSchemaManager(const std::string& database_path);
    ~SQLiteSchemaManager();

    bool create_table(const SQLiteTable& table);
    bool drop_table(const std::string& table_name);
    bool table_exists(const std::string& table_name) const;

    bool create_index(const SQLiteIndex& index);
    bool drop_index(const std::string& index_name);
    bool index_exists(const std::string& index_name) const;

    bool create_foreign_key(const SQLiteForeignKey& foreign_key);
    bool drop_foreign_key(const std::string& constraint_name);
    bool foreign_key_exists(const std::string& constraint_name) const;

    bool create_view(const std::string& view_name, const std::string& select_sql);
    bool drop_view(const std::string& view_name);
    bool view_exists(const std::string& view_name) const;

    bool create_trigger(const std::string& trigger_name, const std::string& table_name,
                       const std::string& when, const std::string& event,
                       const std::string& trigger_sql);
    bool drop_trigger(const std::string& trigger_name);
    bool trigger_exists(const std::string& trigger_name) const;

    bool vacuum_database();
    bool analyze_database();
    bool optimize_database();
    bool integrity_check();

    std::vector<SQLiteTable> get_tables() const;
    std::vector<SQLiteIndex> get_indexes(const std::string& table_name = "") const;
    std::vector<SQLiteForeignKey> get_foreign_keys(const std::string& table_name) const;
    std::vector<std::string> get_views() const;
    std::vector<std::string> get_triggers() const;

    nlohmann::json get_table_stats(const std::string& table_name) const;
    nlohmann::json get_database_stats() const;

private:
    std::string database_path_;

    SQLiteTable parse_table_from_result(const std::vector<std::string>& row) const;
    SQLiteIndex parse_index_from_result(const std::vector<std::string>& row) const;
    SQLiteForeignKey parse_foreign_key_from_result(const std::vector<std::string>& row) const;
};

class SQLiteQueryBuilder {
public:
    SQLiteQueryBuilder();
    ~SQLiteQueryBuilder();

    std::string build_select(const std::string& table,
                           const std::vector<std::string>& columns = {"*"},
                           const std::string& where = "",
                           const std::string& order_by = "",
                           int limit = 0,
                           int offset = 0,
                           const std::string& group_by = "");

    std::string build_insert(const std::string& table,
                           const std::vector<std::string>& columns,
                           const std::vector<std::vector<std::string>>& values,
                           const std::string& conflict_action = "",
                           const std::string& returning = "");

    std::string build_update(const std::string& table,
                           const std::vector<std::pair<std::string, std::string>>& set_values,
                           const std::string& where = "",
                           const std::string& conflict_action = "");

    std::string build_delete(const std::string& table, const std::string& where = "");

    std::string build_create_table(const SQLiteTable& table);
    std::string build_create_index(const SQLiteIndex& index);
    std::string build_create_foreign_key(const SQLiteForeignKey& foreign_key);

    std::string build_join(const std::string& left_table,
                          const std::string& right_table,
                          const std::vector<std::pair<std::string, std::string>>& join_conditions,
                          const std::string& join_type = "INNER");

    std::string build_with_cte(const std::vector<std::pair<std::string, std::string>>& ctes,
                              const std::string& main_query);

    std::string build_upsert(const std::string& table,
                            const std::vector<std::string>& columns,
                            const std::vector<std::string>& values,
                            const std::vector<std::string>& conflict_columns,
                            const std::vector<std::pair<std::string, std::string>>& update_values);

    static std::string placeholder(size_t index);
    static std::string build_in_clause(const std::vector<std::string>& values, size_t start_index = 1);
    static std::string build_json_extract(const std::string& column, const std::string& json_path);
    static std::string build_json_set(const std::string& column, const std::vector<std::pair<std::string, std::string>>& path_values);
};

class SQLiteJSON1 {
public:
    static bool is_available();

    static std::string json_extract(const std::string& json, const std::string& path);
    static std::string json_set(const std::string& json, const std::vector<std::pair<std::string, std::string>>& path_values);
    static std::string json_insert(const std::string& json, const std::vector<std::pair<std::string, std::string>>& path_values);
    static std::string json_replace(const std::string& json, const std::vector<std::pair<std::string, std::string>>& path_values);
    static std::string json_remove(const std::string& json, const std::vector<std::string>& paths);
    static std::string json_array(const std::vector<std::string>& values);
    static std::string json_object(const std::vector<std::pair<std::string, std::string>>& key_values);
    static std::string json_type(const std::string& json, const std::string& path = "$");
    static std::string json_valid(const std::string& json);
    static std::string json_quote(const std::string& value);
    static std::string json_group_array(const std::string& expression);
    static std::string json_group_object(const std::string& key, const std::string& value);
};

}
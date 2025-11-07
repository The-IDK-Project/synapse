#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace matrix::storage::database::postgresql {

enum class PostgreSQLType {
    BOOLEAN,
    SMALLINT,
    INTEGER,
    BIGINT,
    REAL,
    DOUBLE_PRECISION,
    DECIMAL,
    NUMERIC,
    VARCHAR,
    TEXT,
    CHAR,
    BYTEA,
    TIMESTAMP,
    TIMESTAMPTZ,
    DATE,
    TIME,
    TIMETZ,
    INTERVAL,
    JSON,
    JSONB,
    UUID,
    INET,
    CIDR,
    MACADDR,
    MACADDR8,
    BIT,
    VARBIT,
    TSVECTOR,
    TSQUERY,
    XML,
    ARRAY,
    COMPOSITE
};

struct PostgreSQLColumn {
    std::string name;
    PostgreSQLType type;
    int length;
    bool not_null;
    bool primary_key;
    std::string default_value;
    std::string check_constraint;
};

struct PostgreSQLTable {
    std::string name;
    std::string schema;
    std::vector<PostgreSQLColumn> columns;
    std::vector<std::string> primary_keys;
    std::vector<std::string> indexes;
    std::vector<std::string> foreign_keys;
};

struct PostgreSQLIndex {
    std::string name;
    std::string table_name;
    std::vector<std::string> columns;
    bool unique;
    std::string method;
    std::vector<std::string> include_columns;
    std::string where_clause;
};

struct PostgreSQLForeignKey {
    std::string name;
    std::string from_table;
    std::vector<std::string> from_columns;
    std::string to_table;
    std::vector<std::string> to_columns;
    std::string on_delete;
    std::string on_update;
};

class PostgreSQLTypeConverter {
public:
    static std::string to_string(PostgreSQLType type);
    static PostgreSQLType from_string(const std::string& type_str);
    static std::string get_type_definition(PostgreSQLType type, int length = 0);
    static std::string get_array_type_definition(PostgreSQLType element_type, int element_length = 0);

    static std::string escape_value(const std::string& value, PostgreSQLType type);
    static std::string escape_identifier(const std::string& identifier);
    static std::string escape_literal(const std::string& literal);

    static nlohmann::json sql_to_json(const std::string& sql_value, PostgreSQLType type);
    static std::string json_to_sql(const nlohmann::json& json_value, PostgreSQLType type);

    static bool validate_value(const std::string& value, PostgreSQLType type);
    static bool is_numeric_type(PostgreSQLType type);
    static bool is_text_type(PostgreSQLType type);
    static bool is_temporal_type(PostgreSQLType type);
    static bool is_json_type(PostgreSQLType type);
};

class PostgreSQLSchemaManager {
public:
    PostgreSQLSchemaManager(const std::string& connection_string);
    ~PostgreSQLSchemaManager();

    bool create_table(const PostgreSQLTable& table);
    bool drop_table(const std::string& table_name, const std::string& schema = "public");
    bool table_exists(const std::string& table_name, const std::string& schema = "public") const;

    bool create_index(const PostgreSQLIndex& index);
    bool drop_index(const std::string& index_name, const std::string& schema = "public");
    bool index_exists(const std::string& index_name, const std::string& schema = "public") const;

    bool create_foreign_key(const PostgreSQLForeignKey& foreign_key);
    bool drop_foreign_key(const std::string& constraint_name, const std::string& schema = "public");
    bool foreign_key_exists(const std::string& constraint_name, const std::string& schema = "public") const;

    bool create_schema(const std::string& schema_name);
    bool drop_schema(const std::string& schema_name, bool cascade = false);
    bool schema_exists(const std::string& schema_name) const;

    std::vector<PostgreSQLTable> get_tables(const std::string& schema = "public") const;
    std::vector<PostgreSQLIndex> get_indexes(const std::string& table_name, const std::string& schema = "public") const;
    std::vector<PostgreSQLForeignKey> get_foreign_keys(const std::string& table_name, const std::string& schema = "public") const;

    bool vacuum_table(const std::string& table_name, const std::string& schema = "public");
    bool analyze_table(const std::string& table_name, const std::string& schema = "public");
    bool reindex_table(const std::string& table_name, const std::string& schema = "public");

    nlohmann::json get_table_stats(const std::string& table_name, const std::string& schema = "public") const;
    nlohmann::json get_database_stats() const;

private:
    std::string connection_string_;

    PostgreSQLTable parse_table_from_result(const std::vector<std::string>& row) const;
    PostgreSQLIndex parse_index_from_result(const std::vector<std::string>& row) const;
    PostgreSQLForeignKey parse_foreign_key_from_result(const std::vector<std::string>& row) const;
};

class PostgreSQLQueryBuilder {
public:
    PostgreSQLQueryBuilder();
    ~PostgreSQLQueryBuilder();

    std::string build_select(const std::string& table,
                           const std::vector<std::string>& columns = {"*"},
                           const std::string& where = "",
                           const std::string& order_by = "",
                           int limit = 0,
                           int offset = 0);

    std::string build_insert(const std::string& table,
                           const std::vector<std::string>& columns,
                           const std::vector<std::vector<std::string>>& values,
                           const std::string& returning = "");

    std::string build_update(const std::string& table,
                           const std::vector<std::pair<std::string, std::string>>& set_values,
                           const std::string& where = "");

    std::string build_delete(const std::string& table, const std::string& where = "");

    std::string build_create_table(const PostgreSQLTable& table);
    std::string build_create_index(const PostgreSQLIndex& index);
    std::string build_create_foreign_key(const PostgreSQLForeignKey& foreign_key);

    std::string build_join(const std::string& left_table,
                          const std::string& right_table,
                          const std::vector<std::pair<std::string, std::string>>& join_conditions,
                          const std::string& join_type = "INNER");

    std::string build_with_cte(const std::vector<std::pair<std::string, std::string>>& ctes,
                              const std::string& main_query);

    static std::string placeholder(size_t index);
    static std::string build_in_clause(const std::vector<std::string>& values, size_t start_index = 1);
    static std::string build_array_literal(const std::vector<std::string>& values, PostgreSQLType element_type);
};

}
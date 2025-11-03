#pragma once

#include "../../core/matrix_types.hpp"
#include <memory>
#include <string>

namespace matrix::storage::repository {

    class Repository {
    public:
        Repository() = default;
        virtual ~Repository() = default;

        virtual bool initialize() = 0;
        virtual bool shutdown() = 0;
        virtual bool is_connected() const = 0;

        virtual std::string last_error() const { return last_error_; }

    protected:
        mutable std::string last_error_;
    };

    template<typename T>
    class CrudRepository : public Repository {
    public:
        virtual bool create(const T& entity) = 0;
        virtual std::unique_ptr<T> read(const std::string& id) = 0;
        virtual bool update(const T& entity) = 0;
        virtual bool remove(const std::string& id) = 0;
        virtual bool exists(const std::string& id) const = 0;
    };

    template<typename T>
    class PaginatedRepository : public CrudRepository<T> {
    public:
        struct PaginationResult {
            std::vector<T> items;
            std::string next_token;
            bool has_more;
        };

        virtual PaginationResult read_all(const std::string& start_token = "", int limit = 100) = 0;
        virtual std::vector<T> read_by_ids(const std::vector<std::string>& ids) = 0;
    };

}
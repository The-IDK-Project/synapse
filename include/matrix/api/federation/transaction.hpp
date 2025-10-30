#pragma once

#include "../common/api_types.hpp"
#include "../../core/event/event.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace matrix::api::federation {

class Transaction {
public:
    Transaction(const std::string& transaction_id, const std::string& origin_server);
    ~Transaction();

    std::string transaction_id() const { return transaction_id_; }
    std::string origin_server() const { return origin_server_; }
    Timestamp created_ts() const { return created_ts_; }
    Timestamp processed_ts() const { return processed_ts_; }

    void add_pdu(const std::shared_ptr<core::Event>& event);
    void add_edu(const nlohmann::json& edu);
    void add_pdus(const std::vector<nlohmann::json>& pdus);
    void add_edus(const std::vector<nlohmann::json>& edus);

    const std::vector<std::shared_ptr<core::Event>>& pdus() const { return pdus_; }
    const std::vector<nlohmann::json>& edus() const { return edus_; }

    bool is_empty() const { return pdus_.empty() && edus_.empty(); }
    bool is_processed() const { return processed_ts_ != Timestamp(); }

    void mark_processed();
    void mark_failed(const std::string& error);

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);

    bool validate() const;
    bool process();

private:
    std::string transaction_id_;
    std::string origin_server_;
    Timestamp created_ts_;
    Timestamp processed_ts_;
    std::string error_;
    bool processed_ = false;

    std::vector<std::shared_ptr<core::Event>> pdus_;
    std::vector<nlohmann::json> edus_;

    bool validate_pdu(const std::shared_ptr<core::Event>& event) const;
    bool validate_edu(const nlohmann::json& edu) const;
    bool process_pdu(const std::shared_ptr<core::Event>& event);
    bool process_edu(const nlohmann::json& edu);
};

class TransactionManager {
public:
    TransactionManager();
    ~TransactionManager();

    std::shared_ptr<Transaction> create_transaction(const std::string& origin_server);
    std::shared_ptr<Transaction> get_transaction(const std::string& transaction_id) const;
    bool has_transaction(const std::string& transaction_id) const;
    bool remove_transaction(const std::string& transaction_id);

    std::vector<std::shared_ptr<Transaction>> get_pending_transactions() const;
    std::vector<std::shared_ptr<Transaction>> get_transactions_for_server(const std::string& server_name) const;

    bool process_transaction(const std::string& transaction_id);
    bool process_all_transactions();

    void cleanup_old_transactions(int max_age_seconds = 3600);
    void cleanup_failed_transactions(int max_age_seconds = 86400);

    size_t get_transaction_count() const { return transactions_.size(); }
    size_t get_pending_count() const;
    size_t get_processed_count() const;
    size_t get_failed_count() const;

    nlohmann::json get_statistics() const;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<Transaction>> transactions_;
    std::unordered_map<std::string, std::vector<std::string>> server_transactions_;

    std::string generate_transaction_id(const std::string& origin_server);
    bool is_transaction_duplicate(const std::string& transaction_id, const std::string& origin_server);
    void update_server_transaction_index(const std::string& transaction_id, const std::string& origin_server);
    void remove_server_transaction_index(const std::string& transaction_id, const std::string& origin_server);
};

class TransactionProcessor {
public:
    TransactionProcessor(std::shared_ptr<core::RoomManager> room_manager,
                        std::shared_ptr<core::UserManager> user_manager,
                        std::shared_ptr<core::StateManager> state_manager);
    ~TransactionProcessor();

    bool process_transaction(const std::shared_ptr<Transaction>& transaction);
    bool process_pdu(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool process_edu(const nlohmann::json& edu, const std::string& origin_server);

    struct ProcessingResult {
        bool success;
        std::vector<std::string> processed_events;
        std::vector<std::string> failed_events;
        std::string error;
    };

    ProcessingResult process_transaction_batch(const std::shared_ptr<Transaction>& transaction);

private:
    bool process_room_event(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool process_state_event(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool process_message_event(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool process_ephemeral_event(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool process_presence_edu(const nlohmann::json& edu, const std::string& origin_server);
    bool process_receipt_edu(const nlohmann::json& edu, const std::string& origin_server);
    bool process_typing_edu(const nlohmann::json& edu, const std::string& origin_server);
    bool process_device_list_update_edu(const nlohmann::json& edu, const std::string& origin_server);
    bool process_signature_edu(const nlohmann::json& edu, const std::string& origin_server);
    bool validate_federation_event(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool verify_event_signature(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    bool check_event_authorization(const std::shared_ptr<core::Event>& event, const std::string& origin_server);
    std::vector<std::shared_ptr<core::Event>> resolve_conflicting_events(const std::vector<std::shared_ptr<core::Event>>& events);
    bool apply_state_events(const RoomID& room_id, const std::vector<std::shared_ptr<core::Event>>& state_events);

    std::shared_ptr<core::RoomManager> room_manager_;
    std::shared_ptr<core::UserManager> user_manager_;
    std::shared_ptr<core::StateManager> state_manager_;

    std::unordered_map<std::string, int> server_processing_limits_;
    std::unordered_map<std::string, Timestamp> last_processing_time_;
};

}
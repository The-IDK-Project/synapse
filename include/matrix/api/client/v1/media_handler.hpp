#pragma once

#include "../../common/api_types.hpp"
#include "../../common/response_builder.hpp"
#include <memory>
#include <unordered_map>

namespace matrix::api::client::v1 {

class MediaHandler {
public:
    MediaHandler();
    ~MediaHandler();
    ApiResponse handle_upload_media(const ApiRequest& request);
    ApiResponse handle_upload_media_with_filename(const ApiRequest& request);
    ApiResponse handle_download_media(const ApiRequest& request);
    ApiResponse handle_get_media_preview(const ApiRequest& request);
    ApiResponse handle_get_media_thumbnail(const ApiRequest& request);
    ApiResponse handle_get_media_info(const ApiRequest& request);
    ApiResponse handle_delete_media(const ApiRequest& request);
    ApiResponse handle_quarantine_media(const ApiRequest& request);
    ApiResponse handle_get_media_statistics(const ApiRequest& request);

private:
    struct MediaInfo {
        std::string media_id;
        std::string content_type;
        size_t content_length;
        std::string filename;
        UserID user_id;
        Timestamp created_ts;
        Timestamp last_accessed_ts;
        int access_count;
        bool quarantined;
        std::string sha256_hash;
        std::string storage_path;
    };

    struct MediaConfig {
        size_t max_file_size;
        std::vector<std::string> allowed_content_types;
        std::string storage_path;
        size_t storage_quota_per_user;
        bool enable_quarantine;
        int media_retention_days;
    };

    std::string generate_media_id();
    bool validate_media_content_type(const std::string& content_type);
    bool validate_media_size(size_t content_length, const UserID& user_id);
    std::string calculate_media_hash(const std::vector<uint8_t>& data);
    std::string generate_storage_path(const std::string& media_id, const std::string& content_type);
    bool store_media_data(const std::string& media_id, const std::vector<uint8_t>& data);
    nlohmann::json build_upload_response(const std::string& media_id, const std::string& content_uri);
    std::vector<uint8_t> load_media_data(const std::string& media_id);
    bool media_exists(const std::string& media_id);
    bool can_user_access_media(const std::string& media_id, const UserID& user_id);
    void update_media_access_stats(const std::string& media_id);
    ApiResponse build_media_download_response(const std::string& media_id, const std::string& filename = "");
    ApiResponse build_media_thumbnail_response(const std::string& media_id, int width, int height,
                                              const std::string& method, const std::string& content_type);

    std::vector<uint8_t> generate_image_thumbnail(const std::vector<uint8_t>& image_data, int width, int height,
                                                 const std::string& method, const std::string& content_type);
    std::vector<uint8_t> generate_video_thumbnail(const std::vector<uint8_t>& video_data, int width, int height,
                                                 const std::string& content_type);
    bool supports_thumbnail_generation(const std::string& content_type);

    nlohmann::json generate_media_preview(const std::string& media_id, const std::string& content_type,
                                         size_t content_length);
    nlohmann::json generate_image_preview(const std::vector<uint8_t>& image_data);
    nlohmann::json generate_video_preview(const std::vector<uint8_t>& video_data);
    nlohmann::json generate_audio_preview(const std::vector<uint8_t>& audio_data);
    nlohmann::json generate_file_preview(const std::string& filename, size_t content_length);
    bool can_user_manage_media(const std::string& media_id, const UserID& user_id);
    bool can_user_quarantine_media(const UserID& user_id);
    void quarantine_media_internal(const std::string& media_id, const UserID& user_id, const std::string& reason);
    void delete_media_internal(const std::string& media_id);
    void cleanup_expired_media();
    nlohmann::json build_media_statistics(const UserID& user_id = "");
    size_t calculate_user_media_usage(const UserID& user_id);
    size_t calculate_total_media_usage();
    std::vector<std::string> get_most_accessed_media(int limit = 10);
    std::vector<std::string> get_largest_media_files(int limit = 10);
    ApiResponse build_media_not_found_error(const std::string& media_id);
    ApiResponse build_media_too_large_error(size_t content_length, size_t max_size);
    ApiResponse build_unsupported_media_type_error(const std::string& content_type);
    ApiResponse build_storage_quota_exceeded_error(const UserID& user_id, size_t current_usage, size_t quota);
    ApiResponse build_media_quarantined_error(const std::string& media_id);
    ApiResponse build_permission_denied_error(const std::string& media_id, const UserID& user_id);

    MediaConfig config_;
    std::unordered_map<std::string, MediaInfo> media_registry_;
    std::unordered_map<UserID, size_t> user_media_usage_;
    std::unordered_map<std::string, std::vector<uint8_t>> media_cache_;

    std::string media_base_url_;
};

}
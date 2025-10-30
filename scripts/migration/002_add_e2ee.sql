CREATE TABLE device_keys (
                             user_id TEXT NOT NULL,
                             device_id TEXT NOT NULL,
                             algorithm TEXT NOT NULL,
                             key_json JSONB NOT NULL,
                             signature TEXT,
                             created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                             updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                             PRIMARY KEY (user_id, device_id)
);

CREATE TABLE one_time_keys (
                               user_id TEXT NOT NULL,
                               device_id TEXT NOT NULL,
                               algorithm TEXT NOT NULL,
                               key_id TEXT NOT NULL,
                               key_json JSONB NOT NULL,
                               used BOOLEAN DEFAULT FALSE,
                               created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                               PRIMARY KEY (user_id, device_id, algorithm, key_id)
);

CREATE TABLE olm_sessions (
                              session_id TEXT PRIMARY KEY,
                              user_id TEXT NOT NULL,
                              device_id TEXT NOT NULL,
                              sender_key TEXT NOT NULL,
                              session_data JSONB NOT NULL,
                              created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                              updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE megolm_sessions (
                                 session_id TEXT PRIMARY KEY,
                                 room_id TEXT NOT NULL,
                                 sender_key TEXT NOT NULL,
                                 signing_key TEXT NOT NULL,
                                 forwarding_chains TEXT[],
                                 session_data JSONB NOT NULL,
                                 is_shared BOOLEAN DEFAULT FALSE,
                                 max_messages INTEGER DEFAULT 1000,
                                 message_count INTEGER DEFAULT 0,
                                 max_age_ms BIGINT DEFAULT 604800000,
                                 created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                 updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE megolm_inbound_sessions (
                                         session_id TEXT NOT NULL,
                                         room_id TEXT NOT NULL,
                                         sender_key TEXT NOT NULL,
                                         signing_key TEXT NOT NULL,
                                         session_data JSONB NOT NULL,
                                         forwarded_keys TEXT[],
                                         trusted BOOLEAN DEFAULT FALSE,
                                         created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                         PRIMARY KEY (session_id, sender_key)
);

CREATE TABLE key_backups (
                             user_id TEXT PRIMARY KEY,
                             version TEXT NOT NULL,
                             algorithm TEXT NOT NULL,
                             auth_data JSONB,
                             key_data JSONB,
                             created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE key_backup_sessions (
                                     user_id TEXT NOT NULL,
                                     room_id TEXT NOT NULL,
                                     session_id TEXT NOT NULL,
                                     version TEXT NOT NULL,
                                     first_message_index INTEGER,
                                     forwarded_count INTEGER DEFAULT 0,
                                     is_verified BOOLEAN DEFAULT FALSE,
                                     session_data JSONB NOT NULL,
                                     created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                     PRIMARY KEY (user_id, room_id, session_id)
);

CREATE TABLE cross_signing_keys (
                                    user_id TEXT NOT NULL,
                                    usage TEXT NOT NULL,
                                    key_id TEXT NOT NULL,
                                    key_data JSONB NOT NULL,
                                    signature TEXT,
                                    created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                    PRIMARY KEY (user_id, usage, key_id)
);

CREATE TABLE cross_signing_signatures (
                                          user_id TEXT NOT NULL,
                                          target_user_id TEXT NOT NULL,
                                          target_key TEXT NOT NULL,
                                          signature TEXT NOT NULL,
                                          created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                          PRIMARY KEY (user_id, target_user_id, target_key)
);

CREATE INDEX idx_device_keys_user ON device_keys(user_id);
CREATE INDEX idx_one_time_keys_user_device ON one_time_keys(user_id, device_id);
CREATE INDEX idx_olm_sessions_user_device ON olm_sessions(user_id, device_id);
CREATE INDEX idx_megolm_sessions_room ON megolm_sessions(room_id);
CREATE INDEX idx_megolm_inbound_room ON megolm_inbound_sessions(room_id);
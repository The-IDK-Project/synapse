CREATE TABLE events (
                        event_id TEXT PRIMARY KEY,
                        room_id TEXT NOT NULL,
                        type TEXT NOT NULL,
                        sender TEXT NOT NULL,
                        state_key TEXT,
                        content JSONB NOT NULL,
                        origin_server_ts BIGINT NOT NULL,
                        unsigned JSONB,
                        redacts TEXT,
                        depth BIGINT NOT NULL DEFAULT 0,
                        prev_events TEXT[],
                        auth_events TEXT[],
                        created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE rooms (
                       room_id TEXT PRIMARY KEY,
                       creator TEXT NOT NULL,
                       name TEXT,
                       topic TEXT,
                       avatar_url TEXT,
                       guest_access TEXT DEFAULT 'forbidden',
                       history_visibility TEXT DEFAULT 'shared',
                       join_rules TEXT DEFAULT 'invite',
                       encryption TEXT,
                       version TEXT DEFAULT '1',
                       created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                       updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE room_membership (
                                 room_id TEXT NOT NULL,
                                 user_id TEXT NOT NULL,
                                 membership TEXT NOT NULL,
                                 display_name TEXT,
                                 avatar_url TEXT,
                                 event_id TEXT,
                                 created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                 updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                 PRIMARY KEY (room_id, user_id)
);

CREATE TABLE room_state (
                            room_id TEXT NOT NULL,
                            type TEXT NOT NULL,
                            state_key TEXT NOT NULL,
                            event_id TEXT NOT NULL,
                            content JSONB NOT NULL,
                            created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                            PRIMARY KEY (room_id, type, state_key)
);

CREATE TABLE users (
                       user_id TEXT PRIMARY KEY,
                       display_name TEXT,
                       avatar_url TEXT,
                       admin BOOLEAN DEFAULT FALSE,
                       deactivated BOOLEAN DEFAULT FALSE,
                       shadow_banned BOOLEAN DEFAULT FALSE,
                       created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                       updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE devices (
                         user_id TEXT NOT NULL,
                         device_id TEXT NOT NULL,
                         display_name TEXT,
                         last_seen_ts BIGINT,
                         ip TEXT,
                         user_agent TEXT,
                         created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                         PRIMARY KEY (user_id, device_id)
);

CREATE TABLE access_tokens (
                               token TEXT PRIMARY KEY,
                               user_id TEXT NOT NULL,
                               device_id TEXT,
                               valid_until TIMESTAMP,
                               created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE user_credentials (
                                  user_id TEXT PRIMARY KEY,
                                  password_hash TEXT,
                                  salt TEXT,
                                  algorithm TEXT DEFAULT 'bcrypt',
                                  created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE presence (
                          user_id TEXT PRIMARY KEY,
                          presence TEXT DEFAULT 'offline',
                          status_msg TEXT,
                          last_active_ts BIGINT,
                          created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                          updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE push_rules (
                            user_id TEXT NOT NULL,
                            scope TEXT NOT NULL,
                            kind TEXT NOT NULL,
                            rule_id TEXT NOT NULL,
                            conditions JSONB,
                            actions JSONB NOT NULL,
                            default_rule BOOLEAN DEFAULT FALSE,
                            enabled BOOLEAN DEFAULT TRUE,
                            PRIMARY KEY (user_id, scope, kind, rule_id)
);

CREATE TABLE receipts (
                          room_id TEXT NOT NULL,
                          receipt_type TEXT NOT NULL,
                          user_id TEXT NOT NULL,
                          event_id TEXT NOT NULL,
                          data JSONB,
                          created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                          PRIMARY KEY (room_id, receipt_type, user_id)
);

CREATE TABLE account_data (
                              user_id TEXT NOT NULL,
                              room_id TEXT,
                              type TEXT NOT NULL,
                              content JSONB NOT NULL,
                              created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                              updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                              PRIMARY KEY (user_id, COALESCE(room_id, ''), type)
);

CREATE TABLE media_repository (
                                  media_id TEXT PRIMARY KEY,
                                  media_type TEXT NOT NULL,
                                  media_length INTEGER,
                                  upload_name TEXT,
                                  user_id TEXT NOT NULL,
                                  created_ts BIGINT NOT NULL,
                                  quarantined BOOLEAN DEFAULT FALSE,
                                  safe_from_quarantine BOOLEAN DEFAULT FALSE
);

CREATE TABLE server_keys (
                             server_name TEXT NOT NULL,
                             key_id TEXT NOT NULL,
                             from_ts BIGINT NOT NULL,
                             until_ts BIGINT NOT NULL,
                             key_json JSONB NOT NULL,
                             verified BOOLEAN DEFAULT FALSE,
                             PRIMARY KEY (server_name, key_id)
);
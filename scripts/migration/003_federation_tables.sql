CREATE TABLE federation_queues (
                                   destination TEXT NOT NULL,
                                   event_id TEXT NOT NULL,
                                   origin TEXT NOT NULL,
                                   event_json JSONB NOT NULL,
                                   need_wait BOOLEAN DEFAULT FALSE,
                                   added_ts BIGINT NOT NULL,
                                   retries INTEGER DEFAULT 0,
                                   last_attempt_ts BIGINT,
                                   error TEXT,
                                   PRIMARY KEY (destination, event_id)
);

CREATE TABLE federation_pdu_origin (
                                       event_id TEXT PRIMARY KEY,
                                       origin TEXT NOT NULL
);

CREATE TABLE federation_outbound_pokes (
                                           destination TEXT NOT NULL,
                                           stream_id BIGINT NOT NULL,
                                           pokes JSONB NOT NULL,
                                           sent_ts BIGINT,
                                           PRIMARY KEY (destination, stream_id)
);

CREATE TABLE federation_transactions (
                                         transaction_id TEXT NOT NULL,
                                         destination TEXT NOT NULL,
                                         origin TEXT NOT NULL,
                                         ts BIGINT NOT NULL,
                                         transaction_json JSONB NOT NULL,
                                         response_code INTEGER,
                                         response_json JSONB,
                                         sent_ts BIGINT,
                                         PRIMARY KEY (transaction_id, destination)
);

CREATE TABLE federation_state_servers (
                                          event_id TEXT NOT NULL,
                                          state_server TEXT NOT NULL,
                                          PRIMARY KEY (event_id, state_server)
);

CREATE TABLE federation_state_groups (
                                         state_group BIGINT PRIMARY KEY,
                                         room_id TEXT NOT NULL,
                                         event_id TEXT NOT NULL
);

CREATE TABLE federation_state_group_edges (
                                              state_group BIGINT NOT NULL,
                                              prev_state_group BIGINT,
                                              PRIMARY KEY (state_group)
);

CREATE TABLE federation_state_group_state (
                                              state_group BIGINT NOT NULL,
                                              room_id TEXT NOT NULL,
                                              type TEXT NOT NULL,
                                              state_key TEXT NOT NULL,
                                              event_id TEXT NOT NULL,
                                              PRIMARY KEY (state_group, room_id, type, state_key)
);

CREATE TABLE federation_invites (
                                    room_id TEXT NOT NULL,
                                    event_id TEXT NOT NULL,
                                    sender TEXT NOT NULL,
                                    target TEXT NOT NULL,
                                    room_version TEXT DEFAULT '1',
                                    event_json JSONB NOT NULL,
                                    state_json JSONB,
                                    created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                    PRIMARY KEY (room_id, event_id)
);

CREATE TABLE federation_rooms (
                                  room_id TEXT PRIMARY KEY,
                                  room_version TEXT DEFAULT '1',
                                  creator TEXT NOT NULL,
                                  is_public BOOLEAN DEFAULT FALSE,
                                  num_joined_members INTEGER DEFAULT 0,
                                  public_rooms_json JSONB,
                                  created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                  updated_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE federation_room_aliases (
                                         room_alias TEXT PRIMARY KEY,
                                         room_id TEXT NOT NULL,
                                         servers TEXT[] NOT NULL,
                                         creator TEXT NOT NULL,
                                         created_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE federation_room_servers (
                                         room_id TEXT NOT NULL,
                                         server TEXT NOT NULL,
                                         PRIMARY KEY (room_id, server)
);

CREATE TABLE federation_user_ips (
                                     user_id TEXT NOT NULL,
                                     access_token TEXT,
                                     ip TEXT NOT NULL,
                                     user_agent TEXT,
                                     last_seen BIGINT NOT NULL,
                                     PRIMARY KEY (user_id, ip)
);

CREATE TABLE federation_destinations (
                                         destination TEXT PRIMARY KEY,
                                         retry_interval BIGINT,
                                         retry_ts BIGINT,
                                         failure_ts BIGINT,
                                         last_successful_stream_ts BIGINT
);

CREATE INDEX idx_federation_queues_destination ON federation_queues(destination, added_ts);
CREATE INDEX idx_federation_queues_need_wait ON federation_queues(need_wait, added_ts);
CREATE INDEX idx_federation_transactions_destination ON federation_transactions(destination, ts);
CREATE INDEX idx_federation_invites_target ON federation_invites(target);
CREATE INDEX idx_federation_room_aliases_room ON federation_room_aliases(room_id);
CREATE INDEX idx_federation_room_servers_server ON federation_room_servers(server);
CREATE INDEX idx_federation_user_ips_ip ON federation_user_ips(ip);
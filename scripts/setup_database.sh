#!/bin/bash

set -e

POSTGRES_HOST=${POSTGRES_HOST:-localhost}
POSTGRES_PORT=${POSTGRES_PORT:-5432}
POSTGRES_DB=${POSTGRES_DB:-matrix}
POSTGRES_USER=${POSTGRES_USER:-matrix}
POSTGRES_PASSWORD=${POSTGRES_PASSWORD:-matrix_password}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MIGRATION_DIR="$SCRIPT_DIR/migration"

export PGPASSWORD=$POSTGRES_PASSWORD

wait_for_postgres() {
    echo "Waiting for PostgreSQL to be ready..."
    until psql -h "$POSTGRES_HOST" -p "$POSTGRES_PORT" -U "$POSTGRES_USER" -d postgres -c '\q' > /dev/null 2>&1; do
        echo "PostgreSQL is unavailable - sleeping"
        sleep 1
    done
    echo "PostgreSQL is ready"
}

create_database() {
    echo "Creating database '$POSTGRES_DB'..."
    psql -h "$POSTGRES_HOST" -p "$POSTGRES_PORT" -U "$POSTGRES_USER" -d postgres -c "CREATE DATABASE $POSTGRES_DB;" || echo "Database already exists"
}

run_migrations() {
    echo "Running migrations..."
    for migration in "$MIGRATION_DIR"/*.sql; do
        if [ -f "$migration" ]; then
            echo "Applying $(basename "$migration")"
            psql -h "$POSTGRES_HOST" -p "$POSTGRES_PORT" -U "$POSTGRES_USER" -d "$POSTGRES_DB" -f "$migration"
        fi
    done
}

create_indexes() {
    echo "Creating indexes..."
    psql -h "$POSTGRES_HOST" -p "$POSTGRES_PORT" -U "$POSTGRES_USER" -d "$POSTGRES_DB" << EOF
CREATE INDEX IF NOT EXISTS idx_events_room_id ON events(room_id);
CREATE INDEX IF NOT EXISTS idx_events_sender ON events(sender);
CREATE INDEX IF NOT EXISTS idx_events_type ON events(type);
CREATE INDEX IF NOT EXISTS idx_room_membership_user_id ON room_membership(user_id);
CREATE INDEX IF NOT EXISTS idx_room_membership_room_id ON room_membership(room_id);
CREATE INDEX IF NOT EXISTS idx_device_keys_user_id ON device_keys(user_id);
CREATE INDEX IF NOT EXISTS idx_federation_queues_destination ON federation_queues(destination);
EOF
}

main() {
    case "${1:-setup}" in
        "setup")
            wait_for_postgres
            create_database
            run_migrations
            create_indexes
            ;;
        "migrate")
            wait_for_postgres
            run_migrations
            ;;
        "reset")
            wait_for_postgres
            psql -h "$POSTGRES_HOST" -p "$POSTGRES_PORT" -U "$POSTGRES_USER" -d postgres -c "DROP DATABASE IF EXISTS $POSTGRES_DB;"
            create_database
            run_migrations
            create_indexes
            ;;
        *)
            echo "Usage: $0 {setup|migrate|reset}"
            exit 1
            ;;
    esac

    echo "Database setup completed successfully"
}

main "$@"
FROM ubuntu:22.04 as builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libpq-dev \
    libcurl4-openssl-dev \
    libhiredis-dev \
    pkg-config \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install conan

WORKDIR /app

COPY conanfile.txt .

RUN conan install . --build missing -s build_type=Release

COPY . .

RUN mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTS=OFF \
        -DBUILD_BENCHMARKS=OFF \
        -DBUILD_TOOLS=ON \
    && make -j$(nproc) \
    && make install

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl3 \
    libpq5 \
    libcurl4 \
    libhiredis0.14 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

RUN groupadd -r matrix && useradd -r -g matrix matrix

WORKDIR /app

COPY --from=builder /app/build/src/matrix-server /app/matrix-server
COPY --from=builder /app/build/tools/ /app/tools/

COPY config/ /app/config/
COPY scripts/ /app/scripts/

RUN mkdir -p /app/data /app/logs /app/keys \
    && chown -R matrix:matrix /app

USER matrix

EXPOSE 8008 8448

HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8008/_matrix/client/versions || exit 1

CMD ["/app/matrix-server", "/app/config/config.yaml"]
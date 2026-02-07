FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    python3 \
    python3-pip \
    git \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN curl https://sh.rustup.rs -sSf | sh -s -- -y

RUN pip install tomli

WORKDIR /app

COPY . .

ENV CARGO_HOME=/root/.cargo
ENV RUSTUP_HOME=/root/.rustup
ENV PATH="$CARGO_HOME/bin:$PATH"

CMD python3 -m ci.ci && mkdir -p /output && cp -r . /output/

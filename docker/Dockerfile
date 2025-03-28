## -- Debian 11
FROM debian:11-slim AS debian11

RUN apt update
RUN apt install -y file cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev

WORKDIR /app

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN cmake --build . -j 4 --target webx-engine
RUN cpack -G DEB


## -- Debian 12
FROM debian:12-slim AS debian12

RUN apt update
RUN apt install -y file cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev

WORKDIR /app

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN cmake --build . -j 4 --target webx-engine
RUN cpack -G DEB


## -- Ubuntu 22.04
FROM ubuntu:22.04 AS ubuntu22

RUN apt update
RUN apt install -y file cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev

WORKDIR /app

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN cmake --build . -j 4 --target webx-engine
RUN cpack -G DEB


## -- Ubuntu 24.04
FROM ubuntu:24.04 AS ubuntu24

RUN apt update
RUN apt install -y file cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev

WORKDIR /app

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN cmake --build . -j 4 --target webx-engine
RUN cpack -G DEB


## -- Ubuntu 20.04
FROM ubuntu:20.04 AS ubuntu20

RUN apt update
RUN apt install -y file cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev

WORKDIR /app

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_DAMAGE_FIX=ON .
RUN cmake --build . -j 4 --target webx-engine
RUN cpack -G DEB


FROM alpine:3

WORKDIR /app

# Copy package to standard directory
RUN mkdir -p target/debian11
RUN mkdir -p target/debian12
RUN mkdir -p target/ubuntu22
RUN mkdir -p target/ubuntu24
RUN mkdir -p target/ubuntu20
COPY --from=debian11 /app/packages/* target/debian11
COPY --from=debian12 /app/packages/* target/debian12
COPY --from=ubuntu22 /app/packages/* target/ubuntu22
COPY --from=ubuntu24 /app/packages/* target/ubuntu24
COPY --from=ubuntu20 /app/packages/* target/ubuntu20

# to obtain built deb package:
# docker build -t webx-engine-builder .
# docker create -ti --name webx-engine-builder webx-engine-builder bash
# docker cp webx-engine-builder:/app/target/debian/. .
# docker rm -f webx-engine-builder

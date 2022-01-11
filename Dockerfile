FROM debian:11-slim

RUN apt update
RUN apt install -y cmake pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev 

WORKDIR /app

# to build webx:
# docker build -t webx-engine-builder .
# docker run --rm --user "$(id -u)":"$(id -g)" -v "$PWD":/app webx-engine-builder cmake .
# docker run --rm --user "$(id -u)":"$(id -g)" -v "$PWD":/app webx-engine-builder cmake --build . -j 4 --target webx
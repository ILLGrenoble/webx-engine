FROM debian:11-slim

RUN apt update
RUN apt install -y cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev 
RUN apt install -y file

WORKDIR /app

COPY . .

RUN cmake .
RUN cmake --build . -j 4 --target webx-engine
RUN cpack -G DEB

# Save the version to a file
RUN awk '/CPACK_PACKAGE_VERSION /{ gsub(/[()]/, " "); gsub(/[\"]/, ""); print $3}' CMakeLists.txt > VERSION

# Copy package to standard directory
RUN mkdir -p target/debian
RUN cp packages/* target/debian

# to obtain built deb package:
# docker build -t webx-engine-builder .
# docker create -ti --name webx-engine-builder webx-engine-builder bash
# docker cp webx-engine-builder:/app/target/debian/. .
# docker rm -f webx-engine-builder


#!/bin/bash

# Install dependencies
apt install -y apt-utils libclang-dev libpam-dev clang

# Install webx user
addgroup --system --quiet webx
adduser --system --quiet --ingroup webx --no-create-home --home /nonexistent webx

# Install Rust
curl https://sh.rustup.rs -sSf > /tmp/rustup-init.sh \
    && chmod +x /tmp/rustup-init.sh \
    && sh /tmp/rustup-init.sh -y \
    && rm -rf /tmp/rustup-init.sh

~/.cargo/bin/rustup default nightly

cd ..
WORKSPACE=`pwd`

# Build and install latest stable version of webx-session-manager
git clone https://github.com/ILLGrenoble/webx-session-manager.git
cd webx-session-manager

~/.cargo/bin/cargo build --release

mkdir /var/log/webx
mkdir -p /etc/webx/webx-session-manager

cp target/release/server /usr/bin/webx-session-manager
cp config.example.yml /etc/webx/webx-session-manager-config.yml
cp bin/pam-webx /etc/pam.d/webx
cp bin/startwm.sh /etc/webx/webx-session-manager/startwm.sh

# Update X config to allow creation of xfce4 desktop manager
sed -i 's/startxfce4/dbus-launch startxfce4/g' /etc/webx/webx-session-manager/startwm.sh
sed -i 's|/etc/X11/xrdp/xorg.conf|xorg.conf|g' /etc/webx/webx-session-manager-config.yml
sed -i 's/console/anybody/g' /etc/X11/Xwrapper.config
echo "needs_root_rights = no" | tee -a /etc/X11/Xwrapper.config
cp /etc/X11/xrdp/xorg.conf /etc/X11/xorg.conf

# Build and install latest stable version of webx-router
cd ..
git clone https://github.com/ILLGrenoble/webx-router.git
cd webx-router

~/.cargo/bin/cargo build --release

cp target/release/webx-router /usr/bin/webx-router
cp config.yml /etc/webx/webx-router-config.yml

# Use webx engine from sources
sed -i "s|/usr/bin/webx-engine|${WORKSPACE}/webx-engine/bin/webx-engine|g" /etc/webx/webx-router-config.yml

# Create users
useradd -m -U -s /bin/bash -p $(openssl passwd -6 'mario') mario
useradd -m -U -s /bin/bash -p $(openssl passwd -6 'luigi') luigi
useradd -m -U -s /bin/bash -p $(openssl passwd -6 'peach') peach
useradd -m -U -s /bin/bash -p $(openssl passwd -6 'toad') toad
useradd -m -U -s /bin/bash -p $(openssl passwd -6 'bowser') bowser
useradd -m -U -s /bin/bash -p $(openssl passwd -6 'yoshi') yoshi

sleep 1

# Run the servers
nohup webx-session-manager > /var/log/webx/webx-session-manager.log 2>&1 &

sleep 1

nohup webx-router > /var/log/webx/webx-router.log 2>&1 &

sleep 1

echo multi-user environment ready

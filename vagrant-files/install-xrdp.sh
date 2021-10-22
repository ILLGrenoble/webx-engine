#!/bin/bash

apt-get update -y
apt-get install -yy git autoconf libtool pkg-config gcc g++ make libssl-dev libpam0g-dev libjpeg-dev libx11-dev libxfixes-dev libxrandr-dev flex bison libxml2-dev intltool xsltproc xutils-dev python-libxml2 g++ xutils libfuse-dev libmp3lame-dev nasm libpixman-1-dev xserver-xorg-dev

XRDP_DIRECTORY_PATH=/tmp/xrdp
mkdir -p ${XRDP_DIRECTORY_PATH}

wget https://github.com/neutrinolabs/xrdp/releases/download/v0.9.17/xrdp-0.9.17.tar.gz -O ${XRDP_DIRECTORY_PATH}/xrdp-0.9.17.tar.gz
wget https://github.com/neutrinolabs/xorgxrdp/releases/download/v0.2.17/xorgxrdp-0.2.17.tar.gz -O ${XRDP_DIRECTORY_PATH}/xorgxrdp-0.2.17.tar.gz

if [ ! -f "${XRDP_DIRECTORY_PATH}/xrdp-0.9.17.tar.gz" ]; then
    echo "Could not download xrdp"
    exit 1
fi

if [ ! -f "${XRDP_DIRECTORY_PATH}/xorgxrdp-0.2.17.tar.gz" ]; then
    echo "Could not download xorgxrdp"
    exit 1
fi

tar xvfz ${XRDP_DIRECTORY_PATH}/xrdp-0.9.17.tar.gz --directory ${XRDP_DIRECTORY_PATH}
tar xvfz ${XRDP_DIRECTORY_PATH}/xorgxrdp-0.2.17.tar.gz --directory ${XRDP_DIRECTORY_PATH}

echo "Installing xrdp..."

cd ${XRDP_DIRECTORY_PATH}/xrdp-0.9.17

./bootstrap
./configure --enable-pixman
make
make install 
ln -s /usr/local/sbin/xrdp{,-sesman} /usr/sbin

echo "Installing xorgxrdp..."
cd ${XRDP_DIRECTORY_PATH}/xorgxrdp-0.2.17

./bootstrap
./configure
make
make install

ldconfig

xrdp-keygen xrdp auto

systemctl enable xrdp.service
systemctl daemon-reload

sed -i "s/console/anybody/g" /etc/X11/Xwrapper.config
sed -i "s/param=Xorg/param=\/usr\/lib\/xorg\/Xorg/g" /etc/xrdp/sesman.ini

# to use xfce as default change the /etc/xrdp/startwm.sh file :
#
##wm_start
#startxfce4
#
#exit 1


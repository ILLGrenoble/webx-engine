cd /tmp 

wget https://github.com/apache/guacamole-server/archive/1.2.0.tar.gz

tar -xvvf 1.2.0.tar.gz

apt-get install -yy \
    libcairo2-dev \
    freerdp2-dev \
    libossp-uuid-dev \
    libavcodec-dev \
    libavutil-dev \
    libpango1.0-dev \
    libssh2-1-dev \
    libssl-dev \
    libswscale-dev \
    libvorbis-dev \
    libwebp-dev \

cd guacamole-server-1.2.0/

autoreconf -fi

./configure --with-init-dir=/etc/init.d --with-rdp

make

make install

ldconfig

mkdir -p /etc/guacamole
cp /vagrant/vagrant-files/guacd.conf /etc/guacamole

systemctl enable guacd
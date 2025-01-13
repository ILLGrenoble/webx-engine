#!/bin/bash

WEBX_ENGINE_PATH=`pwd`

# Use webx engine from sources
sed -i "s|/usr/bin/webx-engine|${WEBX_ENGINE_PATH}/bin/webx-engine|g" /etc/webx/webx-router-config.yml

sleep 1

# Run the servers
nohup webx-session-manager > /var/log/webx/webx-session-manager.log 2>&1 &

sleep 1

nohup webx-router > /var/log/webx/webx-router.log 2>&1 &

sleep 1

echo multi-user environment ready


#!/bin/bash

set -e

mkdir -p "/root/.ssh"
cp -a /.host-ssh/. /root/.ssh/
rm -f /root/.ssh/config

exec sleep infinity

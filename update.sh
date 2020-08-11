#!/bin/bash
# Simple update
sudo systemctl stop fake-smtp-server.service
git stash
git pull
git stash apply
cmake .
make
sudo systemctl start fake-smtp-server.service

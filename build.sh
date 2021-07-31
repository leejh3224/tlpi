#!/bin/sh
docker build -t tlpi . --build-arg TARGET=$1

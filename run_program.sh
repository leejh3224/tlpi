#!/bin/sh
if [ -z "$ARG0" ]; then
    ./exe/out
elif [ -z "$ARG1" ]; then
    ./exe/out "$ARG0"
elif [ -z "$ARG2" ]; then
    ./exe/out "$ARG0" "$ARG1"
elif [ -z "$ARG3" ]; then
    ./exe/out "$ARG0" "$ARG1" "$ARG2"
else
    ./exe/out "$ARG0" "$ARG1" "$ARG2" "$ARG3"
fi

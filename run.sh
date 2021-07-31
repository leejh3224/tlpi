#!/bin/sh
docker run -it --rm -e ARG0=$1 -e ARG1=$2 -e ARG2=$3 -e ARG3=$4 tlpi

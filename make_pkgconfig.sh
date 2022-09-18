#!/bin/bash

prefix=$1
output=$2

cat << pkgconfig > $output
prefix=$prefix
exec_prefix=\${prefix}
libdir=\${prefix}/lib
includedir=\${prefix}/include

Name: pb_prod
Description: Simple library for producing playback streams
Version: 0.1.0
Requires: anon
Libs: -L\${libdir} -lpb_prod
Cflags: -I\${includedir}
pkgconfig

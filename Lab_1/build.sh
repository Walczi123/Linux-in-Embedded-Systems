#!/bin/bash
set -e
wget https://buildroot.org/downloads/buildroot-2020.02.tar.bz2
tar -xjf buildroot-2020.02.tar.bz2
cp config buildroot-2020.02/.config
cd buildroot-2020.02
make tcl-dirclean
make


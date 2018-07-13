#!/bin/sh

make clean -C ./led

make clean -C ./ddr

cp -rf ./* ~/src/bare_device

cd ~/src/bare_device

rm copy.sh

chmod -x led/* ddr/*

chmod +x led/write2sd.sh ddr/write2sd.sh


#!/bin/sh

WRITE2SD="write2sd.sh"

make clean -C ./led

make clean -C ./ddr

make clean -C ./uart

cp -rf ./* ~/src/bare_device

cd ~/src/bare_device

rm copy.sh

chmod -x led/* ddr/* uart/*

chmod +x led/${WRITE2SD} ddr/${WRITE2SD} uart/${WRITE2SD}


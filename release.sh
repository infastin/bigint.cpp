#!/bin/env sh
meson --reconfigure --buildtype=release --prefix=/usr build
cd build
meson compile
sudo meson install

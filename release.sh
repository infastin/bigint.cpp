#!/bin/env sh
meson --reconfigure --buildtype=release build
cd build
meson compile
sudo meson install

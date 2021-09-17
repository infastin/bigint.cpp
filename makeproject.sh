#!/bin/bash
meson --reconfigure build
ln -sf build/compile_commands.json .
ctags -R --exclude={build,compile_commands.json,makeproject.sh,CMakeLists.txt,doxygen,docs,doxygen.conf} .
#doxygen doxygen.conf
cd build
meson compile

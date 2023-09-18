#!/usr/bin/sh

set -ex

CFLAGS="-Wall -Werror -Wextra -ggdb -O0 -Ivendored/id3v2lib/include"
CC=clang
LIBS="-lm"
ID3LIB="vendored/id3v2lib/lib/libid3v2.a"

$CC -MJ a.o.json $CFLAGS -o player player.c $ID3LIB $LIBS



sed -e '1s/^/[\n/' -e '$s/,$/\n]/' *.o.json > compile_commands.json

#!/bin/sh

set -xe

if command -v gcc >/dev/null 2>&1; then
    CC=gcc
elif command -v clang >/dev/null 2>&1; then
    CC=clang
else
    echo "MSVC user? I'm pretty sure you know what to do next." >&2
    exit 1
fi

src=genericc.c

$CC -E $src > "expanded_$src"
$CC -o "${src%.c}" $src

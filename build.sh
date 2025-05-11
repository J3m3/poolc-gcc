#!/bin/sh

# Obviously, only works on POSIX-compliant systems.

set -e

: "${CC=}"

if [ -z "$CC" ]; then
    if command -v gcc >/dev/null 2>&1; then
        CC=gcc
    elif command -v clang >/dev/null 2>&1; then
        CC=clang
    else
        echo "ERROR: no C comiler found; set CC=<compiler>" >&2
        exit 1
    fi
fi

src=genericc.c

set -x

$CC -o "${src%.c}" -g $src
$CC -E $src > "expanded_$src" 2>/dev/null

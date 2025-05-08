#!/bin/sh

# Obviously, only works on POSIX-compliant systems.

set -e

CC=""

while [ "$#" -gt 0 ]; do
    case "$1" in
        --with|-with)
            shift
            CC="$1"
            ;;
        *)
            echo "Usage: $0 [--with|-with <compiler>]" >&2
            exit 1
            ;;
    esac
    shift
done

if [ -z "$CC" ]; then
    if command -v gcc >/dev/null 2>&1; then
        CC=gcc
    elif command -v clang >/dev/null 2>&1; then
        CC=clang
    else
        echo "ERROR: no C comiler found; use --with=<compiler>" >&2
        exit 1
    fi
fi

src=genericc.c

set -x

$CC -o "${src%.c}" -g $src
$CC -E $src > "expanded_$src" 2>/dev/null

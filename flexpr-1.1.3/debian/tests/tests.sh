#!/bin/bash

set -e

echo "Testing binary executable..."
flexpr --version
flexpr --help

echo "Smoke test passed successfully!"

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
while read -r args && read -r expected; do
    [[ -z "$args" ]] && continue
    
    actual=$(xargs flexpr <<< "$args")
    
    if [ "$actual" = "$expected" ]; then
        echo "PASS: flexpr $args"
    else
        echo "FAIL: flexpr $args (Expected: $expected | Got: $actual)"
    fi
    read
done < $SCRIPT_DIR/value_tests.txt
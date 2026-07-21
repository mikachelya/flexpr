#!/bin/bash

echo "Testing binary executable..."
flexpr --version
flexpr --help

echo "Smoke test passed successfully!"

PASSED=2
FAILDE=0

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
while read -r args && read -r expected; do
    [[ -z "$args" ]] && continue
    
    actual=$(xargs flexpr <<< "$args")
    
    if [ "$actual" = "$expected" ]; then
        echo "-ok-: flexpr $args"
        ((PASSED++))
    else
        echo "FAIL: flexpr $args (Expected: $expected | Got: $actual)"
        ((FAILED++))
    fi
    read
done < $SCRIPT_DIR/value_tests.txt

while read -r args_line && read -r expected_line && read -r _; do
    [[ -z "$args_line" ]] && continue

    exp_code="${expected_line%% *}"
    exp_err="${expected_line#* }"
    [[ "$exp_err" == "$exp_code" ]] && exp_err=""

    read -r -a args <<< "$args_line" && act_err=$(./flexpr "${args[@]}" 2>&1 >/dev/null)
    act_code=$?
    act_err=${act_err%%$'\n'*}

    if [[ "$act_code" -eq "$exp_code" ]] && [[ -z "$exp_err" || "$act_err" == *"$exp_err"* ]]; then
        echo "-ok-: flexpr $args_line"
        ((PASSED++))
    else
        echo "FAIL: flexpr $args_line (Expected: $exp_code, $exp_err | Got: $act_code, $act_err)"
        ((FAILED++))
    fi
done < $SCRIPT_DIR/exit_code_tests.txt

echo $PASSED passed, $FAILED failed
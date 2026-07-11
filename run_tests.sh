#!/bin/bash
echo "==========================================="
echo "INITIALIZING FORGE-STREAM REGRESSION SUITE"
echo "==========================================="

for test_file in tests/*.csv; do
    echo -e "\n[TEST TARGET]: $test_file"
    # Pass the file twice to satisfy the <broker> <bank> argument requirement
    ./forge-stream "$test_file" "$test_file"
done
echo -e "\n==========================================="

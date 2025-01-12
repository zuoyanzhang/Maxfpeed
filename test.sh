#!/bin/bash

# Configuration
WARMUP_RUNS=1
TEST_RUNS=1

# Function to run a single test and return execution time
run_single_test() {
    local expr="$1"
    local start="$2"
    local end="$3"
    local flags="$4"
    local show_output="$5"
    
    # Use time command for precise measurement
    if [ "$show_output" = "true" ]; then
        # Capture both time and output for last run
        output=$( { time -p ./run.sh "$expr" "$start" "$end" "$flags"; } 2>&1 )
        # Extract the real time and save output
        time=$(echo "$output" | grep "^real" | awk '{print $2}')
        # Save everything except the time output
        echo "$output" | grep -v "^real" | grep -v "^user" | grep -v "^sys" > "last_run_output.txt"
    else
        # Just capture time for normal runs
        time=$( { time -p ./run.sh "$expr" "$start" "$end" "$flags" >/dev/null; } 2>&1 | grep "^real" | awk '{print $2}')
    fi
    
    # Convert seconds to milliseconds
    echo "scale=3; $time * 1000" | bc
}

# Function to run test case with warmup and averaging
run_test_case() {
    local expr="$1"
    local start="$2"
    local end="$3"
    local flags="$4"
    
    echo "Testing expression: $expr"
    echo "Range: [$start, $end]"
    echo "Flags: $flags"
    
    # Warmup runs
    echo "Performing $WARMUP_RUNS warmup runs..."
    for ((i=1; i<=$WARMUP_RUNS; i++)); do
        run_single_test "$expr" "$start" "$end" "$flags" "false" > /dev/null
        echo -n "."
    done
    echo " Done!"
    
    # Test runs with timing
    echo "Performing $TEST_RUNS measured runs..."
    total_time=0
    times=()
    
    for ((i=1; i<=$TEST_RUNS; i++)); do
        # For the last run, capture and show output
        if [ $i -eq $TEST_RUNS ]; then
            time=$(run_single_test "$expr" "$start" "$end" "$flags" "true")
        else
            time=$(run_single_test "$expr" "$start" "$end" "$flags" "false")
        fi
        times+=($time)
        total_time=$(echo "scale=3; $total_time + $time" | bc)
        echo "Run $i: ${time}ms"
    done
    
    # Calculate average
    average=$(echo "scale=3; $total_time / $TEST_RUNS" | bc)
    echo "Average execution time: ${average}ms"
    
    # Calculate standard deviation
    sum_squared_diff=0
    for time in "${times[@]}"; do
        diff=$(echo "scale=6; $time - $average" | bc)
        squared_diff=$(echo "scale=6; $diff * $diff" | bc)
        sum_squared_diff=$(echo "scale=6; $sum_squared_diff + $squared_diff" | bc)
    done
    stddev=$(echo "scale=3; sqrt($sum_squared_diff / $TEST_RUNS)" | bc)
    
    echo "Standard deviation: ${stddev}ms"
    
    # Show the output of the last run
    echo "----------------------------------------"
    echo "Last run output:"
    cat "last_run_output.txt"
    echo "----------------------------------------"
}

# Test cases array
declare -a test_cases=(
    "1.0 + 0.5 * x - 0.125 * x * x + 0.0625 * x * x * x - 0.0390625 * x * x * x * x;0;100000;--EIFFEL"
    "1.0 / (sqrt(x + 1.0) + sqrt(x));1;100000;--EIFFEL"
    "(exp(x) - 1.0) / x;0.01;708;--EIFFEL"
    "(exp(x) - 1.0) / log(exp(x));0.01;708;--EIFFEL"
    "exp(x) - 1.0;0.01;708;--EIFFEL"
    "log(x + 1.0) - log(x);0.01;100000;--EIFFEL"
    "1.0 / x - 1.0 / tan(x);0.01;100000;--EIFFEL"
    "sqrt((exp(2 * x) - 1.0) / (exp(x) - 1.0));0.01;708;--EIFFEL"
    "exp(x) / (exp(x) - 1.0);0.01;708;--EIFFEL"
    "(x - sin(x)) / (x - tan(x));0.01;100000;--EIFFEL"
    "exp(x) - 2.0 + exp(-x);0.01;708;--EIFFEL"
    "(4.0 * x) / (x / 1.11 + 1.0);0.1;100000;--EIFFEL"
    "(4.0 * x * x) / (1 + x / 1.11 * x / 1.11);0.1;100000;--EIFFEL"
    "log(exp(x) - 1.0);0.01;708;--EIFFEL"
    "x - (x * x * x) / 6.0 + (x * x * x * x * x) / 120.0 - (x * x * x * x * x * x * x) / 5040.0;0;100000;--EIFFEL"
    "((35000000.0 + ((0.401 * (1000.0 / x)) * (1000.0 / x))) * (x - (1000.0 * 4.27e-5))) - ((1.3806503e-23 * 1000.0) * 300.0);0.01;100000;--EIFFEL"
    "(1.0 - cos(x)) / (x * x);0.01;100000;--EIFFEL"
    "(((x + 1.0) * log(x + 1.0)) - (x * log(x))) - 1.0;0.01;100000;--EIFFEL"
    "cbrt(x + 1) - cbrt(x);0.01;100000;--EIFFEL"
    "((1.0 / (x + 1.0)) - (2.0 / x)) + (1.0 / (x - 1.0));0.01;100000;--EIFFEL"
    "1.0/(x+1) - (1.0/x);0.01;100000;--EIFFEL"
    "1.0/sqrt(x) - 1.0 / sqrt(x+1);0.01;100000;--EIFFEL"
    "1.0 / tan(x + 1) - 1.0 / tan(x);0.01;100000;--EIFFEL"
    "(1.0 - cos(x)) / sin(x);0.01;100000;--EIFFEL"
    "sqrt(x + 1) - sqrt(x);0;100000;--EIFFEL"
    "(x - 1) / (x * x - 1);1.00001;100000;--EIFFEL"
    "1.0 / (x + 1.0);1.00001;100000;--EIFFEL"
    "x / ( x + 1);0;100000;--EIFFEL"
    "(0.954929658551372 * x) - (0.12900613773279798 * ((x * x) * x));0;100000;--EIFFEL"
    "(-x * x * x) / 6.0;0;100000;--EIFFEL"
    "log(1 - x) / log(1 + x);0;100000;--EIFFEL"
    "log((1 - x) / (1 + x));0;100000;--EIFFEL"
    "(x1 * x2 - 1.0) / ((x1 * x2) * (x1 * x2) - 1.0);0.01;100000;--EIFFEL"
    "sqrt(x1 * x1 + x2 * x2);0.01;100000;--EIFFEL"
    "x1 / (x1 + x2);0.01;100000;--EIFFEL"
    "cos(x1 + x2) - cos(x1);0.01;100000;--EIFFEL"
    "tan(x1 + x2) - tan(x1);0.01;100000;--EIFFEL"
    "((-12.0 * x1) - (7.0 * x2)) + x2 * x2;0.01;100000;--EIFFEL"
    "(x1 * x1 + x2 - 11.0) * (x1 * x1 + x2 - 11.0) + (x1 + x2 * x2 - 7.0) * (x1 + x2 * x2 - 7.0);0.01;100000;--EIFFEL"
    "atan(x2 / x1) * (180.0 / 3.14159265359);0.01;100000;--EIFFEL"
    "sin(x1 + x2) - sin(x1);0.01;100000;--EIFFEL"
    "sqrt(x1 + x2 * x2);0.01;100000;--EIFFEL"
    "sin(x1 * x2);0.01;100000;--EIFFEL"
    "(x1 + x2) / (x1 - x2);0.01;100000;--EIFFEL"
    "x1 * cos(x2 * (3.14159265359 / 180.0));0.01;100000;--EIFFEL"
    "x1 * sin(x2 * (3.14159265359 / 180.0));0.01;100000;--EIFFEL"
    "pow((x1 + 1.0), (1.0 / x2)) - pow(x1, (1.0 / x2));0.01;100000;--EIFFEL"
    "0.5 * sqrt((2.0 * (sqrt(x1 * x1 + x2 * x2) + x1)));0.01;100000;--EIFFEL"
    "(0.5 * sin(x1)) * (exp(-x2) - exp(x2));0.01;708;--EIFFEL"
    "x1 + (((((((((2.0 * x1) * (((((3.0 * x1) * x1) + (2.0 * x2)) - x1) / ((x1 * x1) + 1.0))) * ((((((3.0 * x1) * x1) + (2.0 * x2)) - x1) / ((x1 * x1) + 1.0)) - 3.0)) + ((x1 * x1) * ((4.0 * (((((3.0 * x1) * x1) + (2.0 * x2)) - x1) / ((x1 * x1) + 1.0))) - 6.0))) * ((x1 * x1) + 1.0)) + (((3.0 * x1) * x1) * (((((3.0 * x1) * x1) + (2.0 * x2)) - x1) / ((x1 * x1) + 1.0)))) + ((x1 * x1) * x1)) + x1) + (3.0 * (((((3.0 * x1) * x1) - (2.0 * x2)) - x1) / ((x1 * x1) + 1.0))));0.01;100000;--EIFFEL"
    "(((333.75 * pow(b, 6.0)) + (pow(a, 2.0) * (((((11.0 * pow(a, 2.0)) * pow(b, 2.0)) - pow(b, 6.0)) - (121.0 * pow(b, 4.0))) - 2.0))) + (5.5 * pow(b, 8.0))) + (a / (2.0 * b));0.01;100;--EIFFEL"
    "(-(331.4 + (0.6 * x3)) * x2) / (((331.4 + (0.6 * x3)) + x1) * ((331.4 + (0.6 * x3)) + x1));0.01;100000;--EIFFEL"
    "(((x0 + x1) - x2) + ((x1 + x2) - x0)) + ((x2 + x0) - x1);0.01;100000;--EIFFEL"
    "((-(x1 * x2) - ((2.0 * x2) * x3)) - x1) - x3;0.01;100000;--EIFFEL"
    "((((((2.0 * x1) * x2) * x3) + ((3.0 * x3) * x3)) - (((x2 * x1) * x2) * x3)) + ((3.0 * x3) * x3)) - x2;0.01;100000;--EIFFEL"
    "((3.0 + (2.0 / (x3 * x3))) - (((0.125 * (3.0 - (2.0 * x1))) * (((x2 * x2) * x3) * x3)) / (1.0 - x1))) - 4.5;0.01;100000;--EIFFEL"
    "((6.0 * x1) - (((0.5 * x1) * (((x2 * x2) * x3) * x3)) / (1.0 - x1))) - 2.5;0.01;100000;--EIFFEL"
    "((3.0 - (2.0 / (x3 * x3))) - (((0.125 * (1.0 + (2.0 * x1))) * (((x2 * x2) * x3) * x3)) / (1.0 - x1))) - 0.5;0.01;100000;--EIFFEL"
    "(((x0 + x1) - x2) + ((x1 + x2) - x0)) + ((x2 + x0) - x1);0.01;100000;--EIFFEL"
)

# Run all test cases
echo "Starting automated testing..."
echo "Warmup runs per test: $WARMUP_RUNS"
echo "Measured runs per test: $TEST_RUNS"
echo "========================================="

for test_case in "${test_cases[@]}"; do
    IFS=';' read -r expr start end flags <<< "$test_case"
    run_test_case "$expr" "$start" "$end" "$flags"
done

echo "All tests completed!"

# Clean up the temporary output file
rm -f "last_run_output.txt"
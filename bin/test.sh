#!/bin/bash

# exit on error in subcommands
set -e

RIO=./src/cli/rio

# 1. Allocate three memory areas
IFS=";"
TOKEN_A_RESP=(`$RIO memory alloc --size=1024`)
TOKEN_B_RESP=(`$RIO memory alloc --size=512`)
TOKEN_C_RESP=(`$RIO memory alloc --size=256`)
unset IFS

TOKEN_A=${TOKEN_A_RESP[1]}
TOKEN_B=${TOKEN_B_RESP[1]}
TOKEN_C=${TOKEN_C_RESP[1]}

echo Token A = $TOKEN_A
echo Token B = $TOKEN_B
echo Token C = $TOKEN_C

# 2. List memory areas
echo "Control output"
$RIO memory ls

# 3. Remove Memory Area for token B
echo "Remove memory area for Token B"
$RIO memory dealloc --token=$TOKEN_B

# 4. List memory areas again
echo "Control output"
$RIO memory ls

# 5. Create data points on memory area for token C
$RIO data add --datatype=int8 --offset=10 --token=$TOKEN_C test.data.a
$RIO data add --datatype=int16 --offset=11 --token=$TOKEN_C test.data.b
$RIO data add --datatype=int32 --offset=13 --token=$TOKEN_C test.data.c
$RIO data add --datatype=64bit --offset=17 --token=$TOKEN_C test.data.d

# 6. Create anonym data points on memory area for token A
$RIO data add --datatype=int8 --offset=10 test.data.e
$RIO data add --datatype=int16 --offset=11 test.data.f
$RIO data add --datatype=int32 --offset=13 test.data.g
$RIO data add --datatype=64bit --offset=17 test.data.h

# 7. Control output
echo "Control output"
$RIO data ls -l


# X. Dealloc memory areas for token A and C
$RIO memory dealloc --token=$TOKEN_A
$RIO memory dealloc --token=$TOKEN_C

$RIO memory ls
$RIO data ls -l

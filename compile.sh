#!/bin/bash

SAMPLE_DIR="./src/sample"

# Delete sample files that are compiled before
FILES=`ls ./*.cpp 2> /dev/null`
for f in $FILES
do
  	rm -f "$f"
done

if [ "$1" = 1 ]; then
	SAMPLE=1
elif [ "$1" = 2 ]; then
	SAMPLE=2
else
	echo -e "\033[32m Please select sample\033[0m"
	echo -e "\033[32m 1:sensor format (default)\033[0m"
	echo -e "\033[32m 2:send data\033[0m"
	read -p "" SAMPLE
fi

if [ "$SAMPLE" == "2" ]; then
    echo "Compile sample with \"send data\""
    cp $SAMPLE_DIR/senddata_main.cpp .
elif [ "$SAMPLE" == "1" ]; then
    echo "Compile sample with \"sensor format\""
    cp $SAMPLE_DIR/sensorformat_main.cpp .
else
    echo "Compile sample with \"sensor format\""
    cp $SAMPLE_DIR/sensorformat_main.cpp .
fi

mbed compile -m WISE_1540 -t GCC_ARM -c

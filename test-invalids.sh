#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    test-invalids.sh                                   :+:    :+:             #
#                                                      +:+                     #
#    By: fbes <fbes@student.codam.nl>                 +#+                      #
#                                                    +#+                       #
#    Created: 2022/07/14 19:37:28 by fbes          #+#    #+#                  #
#    Updated: 2022/07/14 19:37:29 by fbes          ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

cd "$(dirname "$0")"
make

output=""
linenum=0

echo ""
echo "[TESTER] No server in this test script should ever start."

for filename in examples/configs/invalids/*.ini; do
	echo ""
	echo "Testing $filename..."
	output=$(./webserv $filename 2>&1)
	linenum=$(echo "$output" | tr -dc '0-9')
	echo "Webserv returned:"
	echo "$output"
	echo ""
	if [ ! -z "$linenum" ]; then
		echo "Line it complained about:"
		echo "$(head -n $linenum "$filename" | tail -n 1)"
	fi
	echo ""
done

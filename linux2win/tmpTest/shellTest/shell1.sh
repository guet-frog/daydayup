#!/bin/sh

if [ $# -eq 3 ]; then
	echo "\$# = $#\n\$0 = $0\n\$1 = $1\n\$2 = $2\n\$3 = $3"
else
	echo "param num error"
fi

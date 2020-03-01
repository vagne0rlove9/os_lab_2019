#!/bin/bash

for i in {1..150}
do
    hexdump -n 1 -e '"%d\n"' /dev/random >> numbers.txt
done

echo "Done!"
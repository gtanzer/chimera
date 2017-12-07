#!/bin/bash


for ((i = 0; i < 8192; i++)) do
    wget -q -O /dev/null http://localhost:8080/index.html &
done;
sleep 1; find . -name "index.html*" -type f -delete


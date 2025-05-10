#!/bin/bash

for i in $(seq 1 1000);
do
 shuf -er -n`shuf -i 20-150 -n 1` {A..Z} {a..z} {0..9} | tr -d '\n'; printf "\n"
done


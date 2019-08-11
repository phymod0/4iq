#!/bin/bash

BC=~/dev/4iq/BreachCompilation/

echo "[1/7] Partitioning original data"
./extract.sh $BC \
    | ./remove_whitespace \
    | ./filter_pwds \
    | ./split partitioned/ 64

echo "[2/7] Sorting partitions"
cd partitioned \
    && for part in *; do
        sort $part -o \_$part && mv \_$part $part && echo -en '.';
    done \
    && echo -en "\n" \
    && cd ../

echo "[3/7] Merging partitions and grouping lines"
sort -m --batch-size=64 partitioned/* | uniq -c > grouped \
    && rm partitioned/*

echo "[4/7] Partitioning grouped data"
./split partitioned/ 64 < grouped \
    && rm grouped

echo "[5/7] Sorting grouped partitions"
cd partitioned \
    && for part in *; do
        sort -nr $part -o \_$part && mv \_$part $part && echo -en '.';
    done \
    && echo -en "\n" \
    && cd ../

echo "[6/7] Merging grouped partitions"
sort -nr -m --batch-size=64 partitioned/* > counted \
    && rm partitioned/*

echo "[7/7] Removing count labels"
./remove_counts < counted > wordlist \
    && rm counted

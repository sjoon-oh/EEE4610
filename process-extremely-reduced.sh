#!/bin/bash

json_files=`ls ./*.json`

for eachfile in $json_files; do
    echo "Running script for $eachfile"
    python3 analyze-ard.py --filename $eachfile \
        --rm-key reviewerName reviewText summary reviewerID helpful image description tech1 tech2 similar_item price title feature also_view main_cat rank also_buy fit asin
done

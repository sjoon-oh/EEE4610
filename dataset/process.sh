#!/bin/bash

json_files=`ls ./*.txt`

for eachfile in $json_files
    echo "Running script for $eachfile"
    python3 analyze-ard.py --filename $eachfile \
        --rm-key reviewerName reviewText summary reviewerID helpful image
done

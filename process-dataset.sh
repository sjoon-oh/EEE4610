#!/bin/bash

python3 ./process-amazon-dataset.py \
--dataset "reviews_Electronics_5.json" \
--dataset-rm-key reviewerName reviewText summary \
--metadata "meta_Electronics.json" \
--metadata-rm-key description imUrl
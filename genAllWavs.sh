#!/bin/bash
for i in /home/kof/storage/local/qc_beta/txt/*.txt; do ./genwav -i $i -o wavs/`basename -s .txt $i`.wav ; done

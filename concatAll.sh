#!/bin/bash
for i in movs/*; do echo file $i ; done > concat.txt
ffmpeg -f concat -safe 0 -i concat.txt -c:v copy -c:a copy -y AllBetaCaptures.mov

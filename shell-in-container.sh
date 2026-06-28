#!/bin/bash

set -x

singularity shell -B /scratch3 -B /scratch4 -B /scratch5 \
	/scratch5/purged/Wei.Huang/src/nv/starviewer/starviewer.sif


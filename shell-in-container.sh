#!/bin/bash

set -x

singularity shell -B /scratch3 -B /scratch4 -B /scratch5 \
	/scratch3/NAGAPE/epic/Wei.Huang/src/containers/starviewer.sif


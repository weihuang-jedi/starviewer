#!/bin/bash

set -x

singularity shell -B /lustre \
	/lustre/wei/nv/starviewer/starviewer.sif


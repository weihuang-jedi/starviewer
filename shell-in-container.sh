#!/bin/bash

set -x

singularity shell -B /scratch \
	/scratch/wei/cadre/starviewer/starviewer.sif


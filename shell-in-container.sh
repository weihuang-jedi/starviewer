#!/bin/bash

set -x

#export CONTAINER_BINDINGS="-B /glade -B /gpfs -B /lustre -B /opt/cray -B /run -B /var --bind ${TMPDIR}:/tmp"
 export CONTAINER_DIR=/gpfs/csfs1/work/huangwei/nv
 export CONTAINER_BINDINGS="-B /glade -B /gpfs -B /lustre"
 export CONTAINER_SIF=${CONTAINER_DIR}/starviewer.sif

 singularity shell -e ${CONTAINER_BINDINGS} "${CONTAINER_SIF}"


#!/bin/bash

set -x

#EAGLEhome=/scratch5/purged/Wei.Huang/src/EAGLE

#source ${EAGLEhome}/conda/etc/profile.d/conda.sh

#eval "$(mamba shell hook --shell bash)"

#mamba activate anemoi

#which ffmpeg

moviefile=sphr_bump.mp4

/scratch5/purged/Wei.Huang/src/EAGLE/conda/envs/anemoi/bin/ffmpeg \
	-framerate 10 \
	-pattern_type glob -i '*.png' \
	-vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" -c:v libx264 \
	-pix_fmt yuv420p ${moviefile}

scp ${moviefile} weihuang@hercules-login.hpc.msstate.edu:/work2/noaa/epic/weihuang/nv/starviewer/images/${moviefile}

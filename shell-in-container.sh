#!/bin/bash

set -x

# First detect w/ hostname
case $(hostname -f) in
    ufe0[1-9]) MACHINE_ID=ursa ;; ### ursa01-09
    ufe1[0-6]) MACHINE_ID=ursa ;; ### ursa10-16
    uecflow01) MACHINE_ID=ursa ;; ### ursaecflow01

    [Hh]ercules-login-[1-4].[Hh][Pp][Cc].[Mm]s[Ss]tate.[Ee]du) MACHINE_ID=hercules ;; ### hercules1-4
    *) MACHINE_ID=UNKNOWN ;;                        # Unknown platform
esac

if [[ "${MACHINE_ID}" == "ursa" ]]; then
    singularity shell -B /scratch3 -B /scratch4 -B /scratch5 \
        /scratch5/purged/Wei.Huang/src/nv/starviewer/starviewer.sif
elif [[ "${MACHINE_ID}" == "hercules" ]]; then
    module load apptainer/1.3.3
    singularity shell -B /work -B /work2 \
        /work2/noaa/epic/weihuang/nv/starviewer/starviewer.sif
else
    singularity shell -B /scratch \
        /scratch/wei/cadre/starviewer/starviewer.sif
fi


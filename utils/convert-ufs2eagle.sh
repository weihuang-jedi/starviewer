#!/bin/bash

set -x

for item in controller geometry translator viewer
do
    for suffix in h cpp
    do
	ifile=ufs_${item}.${suffix}
	ofile=eagle_${item}.${suffix}
	sed -e "s/UFS/EAGLE/g" \
	    -e "s/ufs/eagle/g" \
	    ${ifile} > ${ofile}
	rm ${ifile}
    done
done


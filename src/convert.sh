for item in 2dviewer controller geometry translator
do
    sed -e "s/MPAS/MPASGrid/g" \
           "s/mpas/mpasgrid/g" mpas${item}.cpp > mpasgrid${item}.cpp
    sed -e "s/MPAS/MPASGrid/g" \
           "s/mpas/mpasgrid/g" mpas${item}.h > mpasgrid${item}.h
    rm -f mpas${item}.*
done


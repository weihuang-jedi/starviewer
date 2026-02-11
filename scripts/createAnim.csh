#!/bin/csh -f

 set echo

 cp t0.jpg img0000.jpg
 cp t1.jpg img0001.jpg

 set n = 2
 while ($n < 100)
     if($n < 10) then
        set fnew = img000${n}.jpg
     else
        set fnew = img00${n}.jpg
     endif

     @ m = $n - 2

     if($m < 10) then
        set fold = img000${m}.jpg
     else
        set fold = img00${m}.jpg
     endif

    #echo "new file: $fnew, old file: $fold"

     cp $fold $fnew

     @ n = $n + 1
 end


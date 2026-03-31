      FUNCTION ICFELL (MESSG,NERRF)
C
        CHARACTER*(*) MESSG
C
C ICFELL (which stands for "I Check For Errors on Lower Level") is used
C to check for the occurrence of a recoverable error in a lower-level
C routine and (perhaps) to update the current error flag and message.
C
        ICFELL=NERRF
        IF (NERRF.NE.0) THEN
          IF (MESSG.NE.' ') THEN
            PRINT *, 'ERROR MESSAGE: ', TRIM(MESSG)
          END IF
          IF (NERRF.NE.0) IERRF=NERRF
        END IF
        RETURN
      END

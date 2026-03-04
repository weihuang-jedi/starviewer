;Read CAM-SE mapping

 fi = addfile("SEMapping.nc", "r")

;print(f)

 element_corners = fi->element_corners

;printVarSummary(element_corners)

 ds = dimsizes(element_corners)

 new_element_corners = new((/ds(1), ds(0)/), integer)

 system("rm -f reorderedCbasedSEMapping.nc")
 fo = addfile("reorderedCbasedSEMapping.nc", "c")

;===================================================================
; explicitly declare file definition mode. Improve efficiency.
;===================================================================

 fAtt               = True            ; assign file attributes
 fAtt@title         = "reorderedHommeMapping"
 fAtt@source_file   = "HommeMapping.nc"
 fAtt@Conventions   = "None"
 fAtt@creation_date = systemfunc ("date")
 fileattdef(fo, fAtt)

;===================================================================
; predefine the coordinate variables and their dimensionality
; Note: to get an UNLIMITED record dimension, we set the dimensionality
; to -1 (or the actual size) and set the dimension name to True.
;===================================================================
 dimNames = (/"ncenters", "ncorners"/)
 dimSizes = (/ ds(1), ds(0) /)
 dimUnlim = (/ False, False/)
 filedimdef(fo, dimNames, dimSizes, dimUnlim)

 do n = 0, ds(0) - 1
    new_element_corners(:, n) = element_corners(n,:) - 1
 end do

 filevardef(fo, "element_corners", typeof(element_corners), dimNames)
 fo->element_corners = (/new_element_corners/)


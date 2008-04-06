#
# AC_FIND_FILE([$1], [$2], [$3])
#
#
AC_DEFUN([AC_FIND_FILE],
    [
	$3=NO
	for i in $2; do
  	    for j in $1; do
		echo "testing $i/$j"
		if test -r "$i/$j"; then
      		    $3=$i
      		    break 2
    		fi
  	    done
	done
    ]
)

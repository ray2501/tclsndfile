#
# Tcl package index file
#
package ifneeded sndfile 1.0 \
    [list load [file join $dir libsndfile1.0.so] sndfile]

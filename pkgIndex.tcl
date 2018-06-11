#
# Tcl package index file
#
package ifneeded sndfile 0.9 \
    [list load [file join $dir libsndfile0.9.so] sndfile]

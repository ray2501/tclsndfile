#
# Tcl package index file
#
package ifneeded sndfile 0.8 \
    [list load [file join $dir libsndfile0.8.so] sndfile]

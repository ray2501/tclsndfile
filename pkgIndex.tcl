# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded sndfile 1.0 \
	    [list load [file join $dir libtcl9sndfile1.0.so] [string totitle sndfile]]
} else {
    package ifneeded sndfile 1.0 \
	    [list load [file join $dir libsndfile1.0.so] [string totitle sndfile]]
}

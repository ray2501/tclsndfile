tclsndfile
=====

[Libsndfile](http://www.mega-nerd.com/libsndfile/) is a C library for reading
and writing files containing sampled sound (such as MS Windows WAV and the
Apple/SGI AIFF format) through one standard library interface.
It is released in source code format
under the [Gnu Lesser General Public License](http://www.gnu.org/copyleft/lesser.html).

This extension is Tcl bindings for libsndfile.


License
=====

LGPL 2.1, or (at your option) any later version.


Commands
=====

sndfile HANDLE path mode ?-buffersize size? ?-rate samplerate? ?-channels channels? 
?-fileformat format? ?-encoding encoding_type?  
HANDLE buffersize size  
HANDLE read_short  
HANDLE read_int  
HANDLE read_float  
HANDLE read_double  
HANDLE write_short byte_array  
HANDLE write_int byte_array  
HANDLE write_float byte_array   
HANDLE write_double byte_array  
HANDLE seek location whence  
HANDLE get_string str_type  
HANDLE set_string str_type string  
HANDLE close

HANDLE option `mode` have 3 values, READ, WRITE and RDWR.
option `-rate`, `-channels`, `-fileformat` and `-encoding` is only
for WRITE mode and RDWR mode.

`-fileformat` can specify below values:
wav, aiff, au, raw, paf, svx, nist, voc, ircam, w64, mat4, mat5,
pvf, xi, htk, sds, avr, wavex, sd2, flac, caf, wve, ogg, mpc2k, rf64

`-encoding` can specify below values:
pcm_16, pcm_24, pcm_32, pcm_s8, pcm_u8, float, double, ulaw, alaw,
ima_adpcm, ms_adpcm, gsm610, vox_adpcm, g721_32, g723_24, g723_40,
dwvw_12, dwvw_16, dwvw_24, dwvw_n, dpcm_8, dpcm_16, vorbis

seek command option `whence` have 3 values, SET, CUR and END.

`get_string` allow strings to be retrieved from files opened for read where
supported by the given file type.

`set_string` sets the string data. This command allow strings to be set on
files opened for write where supported by the given file type.
It returns zero on success and non-zero on error.

str_type can specify below values:
SF_STR_TITLE, SF_STR_COPYRIGHT, SF_STR_SOFTWARE, SF_STR_ARTIST,
SF_STR_COMMENT, SF_STR_DATE, SF_STR_ALBUM, SF_STR_LICENSE,
SF_STR_TRACKNUMBER, SF_STR_GENRE


UNIX BUILD
=====

I only test tclsndfile under openSUSE LEAP 42.2 and Ubuntu 14.04.

Users need install libsndfile development files.
Below is an example for openSUSE:

	sudo zypper in libsndfile-devel

Below is an example for Ubuntu:

	sudo apt-get install libsndfile1-dev

Building under most UNIX systems is easy, just run the configure script
and then run make. For more information about the build process, see the
tcl/unix/README file in the Tcl src dist. The following minimal example
will install the extension in the /opt/tcl directory.

	$ cd tclsndfle
	$ ./configure --prefix=/opt/tcl
	$ make
	$ make install

If you need setup directory containing tcl configuration (tclConfig.sh),
below is an example:

	$ cd tclsndfile
	$ ./configure --with-tcl=/opt/activetcl/lib
	$ make
	$ make install

WINDOWS BUILD
=====

## MSYS2/MinGW-W64

If users need enable OGG, VORBIS and FLAC support,
[download](https://xiph.org/downloads/) libogg, libvorbis and libflac then build by yourself.

	$ ./configure --prefix=/c/msys64/mingw64
	$ make
	$ make install

Download libsndfile and build.

	$ ./configure --prefix=/c/msys64/mingw64
	$ make
	$ make install

Put libogg-0.dll, libvorbis-0.dll, libvorbisenc-2.dll, libFLAC-8.dll and libsndfile-1.dll
to Windows folder or other available folder.

Next step is to build tclsndfile.

	$ ./configure --with-tcl=/c/tcl/lib
	$ make
	$ make install

Example
=====

Convert a ogg file to a wav file:

    package require sndfile

    if {$argc > 1} {
        set name1 [lindex $argv 0]
        set name2 [lindex $argv 1]
    } else {
        puts "Please input filename."
        exit
    }

    if {[catch {set data [sndfile snd0 $name1 READ]}]} {
        puts "sndfile: read file failed."
        exit
    }
    
    set fileformat [dict get $data fileformat]
    if {[string compare $fileformat "ogg"] != 0} {
        puts "It is not an ogg file, exit!!!"
        exit
    }
    
    sndfile snd1 $name2 WRITE -rate 44100 -channels 2 \
      -fileformat wav -encoding pcm_16

    while {[catch {set buffer [snd0 read_float]}] == 0} {
        snd1 write_float $buffer
    }

    snd0 close
    snd1 close


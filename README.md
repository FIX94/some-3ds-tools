# some-3ds-tools
This collection was made to replace a single sound file in a homebrew .cia banner, so dont expect these to work too well. The tools are pre-compiled in the bin folder.  
ciacontentext expects a title.cia, that'll give you a content.bin you can use in contentexefsext, the created exefs.bin can be extracted with exefsext, then banner then with bannerext.  
after doing your changes you can use bannerpack, exefspack, contentexefsreplace and ciacontentreplace in whatever order you like, all the hashes will be re-calculated of course.  
The tools contain code from polarssl, makerom and dsdecmp.

# Manual Compiling
As of right now this is a windows only code. If you have MinGW installed and gcc referenced in your PATH variable just use the "build.bat".  
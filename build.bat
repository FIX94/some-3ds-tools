gcc -O1 -s -static -Wall ciacontentext.c polarssl/sha2.c -o bin/ciacontentext
gcc -O1 -s -static -Wall ciacontentreplace.c crypto.c polarssl/rsa.c polarssl/bignum.c polarssl/md.c polarssl/md_wrap.c polarssl/md5.c polarssl/sha1.c polarssl/sha2.c polarssl/sha4.c -o bin/ciacontentreplace
gcc -O1 -s -static -Wall contentexefsext.c polarssl/sha2.c -o bin/contentexefsext
gcc -O1 -s -static -Wall contentexefsreplace.c crypto.c polarssl/rsa.c polarssl/bignum.c polarssl/md.c polarssl/md_wrap.c polarssl/md5.c polarssl/sha1.c polarssl/sha2.c polarssl/sha4.c -o bin/contentexefsreplace
gcc -O1 -s -static -Wall exefsext.c polarssl/sha2.c -o bin/exefsext
gcc -O1 -s -static -Wall exefspack.c polarssl/sha2.c -o bin/exefspack
gcc -O1 -s -static -Wall bannerext.c lz11.c -o bin/bannerext
gcc -O1 -s -static -Wall bannerpack.c lz11.c -o bin/bannerpack
pause
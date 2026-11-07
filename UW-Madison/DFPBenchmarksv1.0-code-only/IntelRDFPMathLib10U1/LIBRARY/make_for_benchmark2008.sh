echo "make clean OS_TYPE=LINUX"
make clean OS_TYPE=LINUX
echo "make OS_TYPE=LINUX CC=gcc CALL_BY_REF=1 GLOBAL_RND=0 GLOBAL_FLAGS=0 BID_LIB=libbid_noglbflags.a"
make OS_TYPE=LINUX CC=gcc CALL_BY_REF=1 GLOBAL_RND=0 GLOBAL_FLAGS=0 BID_LIB=libbid_noglbflags.a
touch *.c
touch *.h
rm -f *.o
echo "make OS_TYPE=LINUX CC=gcc CALL_BY_REF=1 GLOBAL_RND=0 GLOBAL_FLAGS=1 BID_LIB=libbid_glbflags.a"
make OS_TYPE=LINUX CC=gcc CALL_BY_REF=1 GLOBAL_RND=0 GLOBAL_FLAGS=1 BID_LIB=libbid_glbflags.a

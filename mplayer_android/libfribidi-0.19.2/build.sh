./configure --target=arm-linux --host=i686-linux host_alias=i686-linux target_alias=arm-linux CC=arm-linux-gcc LD=arm-linux-ld --with-glib=no --enable-static --enable-shared --with-gnu-ld=yes NM=arm-linux-nm AR=arm-linux-ar AS=arm-linux-as RANLIB=arm-linux-ranlib LINK=arm-linux-gcc --prefix=$PWD/pre_install
make

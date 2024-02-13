HOME=/home/engdahl/PDP-11
rm -rf $HOME/cross/src $HOME/cross/obj
cd $HOME/cross
mkdir -p src obj/{binutils-build,gcc-build}
curl https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz | tar -C $HOME/cross/src -xzf -
curl https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz | tar -C $HOME/cross/src -xzf -
cd $HOME/cross/src/gcc-13.2.0
./contrib/download_prerequisites
cd $HOME/cross/obj/binutils-build
../../src/binutils-2.42/configure --prefix $HOME/pdp11-aout-13 --bindir $HOME/pdp11-aout-13/bin --target pdp11-aout
make -j8
make install
cd $HOME/cross/obj/gcc-build
../../src/gcc-13.2.0/configure --prefix $HOME/pdp11-aout-13 --bindir $HOME/pdp11-aout-13/bin --target pdp11-aout --enable-languages=c --with-gnu-as --with-gnu-ld --without-headers --disable-libssp
make -j8
make install

# this script was derived from https://xw.is/wiki/Bare_metal_PDP-11_GCC_9.3.0_cross_compiler_instructions

# redefine the following line for your environment
PREFIX=/home/engdahl/PDP-11

cd $PREFIX
rm -rf cross
mkdir cross
cd cross
mkdir -p src obj/{binutils-build,gcc-build}
curl https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz | tar -C $PREFIX/cross/src -xzf -
curl https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz | tar -C $PREFIX/cross/src -xzf -
cd $PREFIX/cross/src/gcc-13.2.0
./contrib/download_prerequisites
cd $PREFIX/cross/obj/binutils-build
../../src/binutils-2.42/configure --prefix $PREFIX/pdp11-aout-13 --bindir $PREFIX/pdp11-aout-13/bin --target pdp11-aout
make -j8
make install
cd $PREFIX/cross/obj/gcc-build
../../src/gcc-13.2.0/configure --prefix $PREFIX/pdp11-aout-13 --bindir $PREFIX/pdp11-aout-13/bin --target pdp11-aout --enable-languages=c --with-gnu-as --with-gnu-ld --without-headers --disable-libssp
make -j8
make install


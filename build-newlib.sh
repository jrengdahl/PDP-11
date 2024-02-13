mkdir -p build
cd build
../newlib-pdp11/configure --target=pdp11-aout --prefix=/home/engdahl/PDP-11/pdp11-aout-13
make && make install

# You will need to run the following line by hand:
# git clone https://github.com/cptnapalm/newlib-pdp11.git

# Then grep -r for "<<<<", remove the git conflict headers,
# and the lower section for each conflict.

# Edit the prefix path below so that points to where you installed
# the PDP-11 cross compiler..

# This script expects the PDP-11 cross compiler bin directory to be
# in the PATH.

# You can then run this script, and newlib will build and install without errors.


rm -rf build
mkdir build
cd build
../newlib-pdp11/configure --target=pdp11-aout --prefix=/home/engdahl/PDP-11/pdp11-aout-13
make && make install

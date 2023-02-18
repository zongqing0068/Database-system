#! /bin/bash -e
## author:ywh
## install openGauss

echo "# GaussDB Code-
export GAUSSHOME=/opt/software/openGauss #the path for openGauss
export PGDATA=\$GAUSSHOME/data #the path for data
export BINARY_PATH=/opt/software/binarylibs #the path for binarylibs
# GCC 7.3 compiler path
export CMAKEROOT=/usr/local/src/cmake-3.20.0-linux-aarch64/ #the path for cmake
export GCC_PATH=\$BINARY_PATH/buildtools/openeuler_aarch64/gcc7.3
export CC=\$GCC_PATH/gcc/bin/gcc
export CXX=\$GCC_PATH/gcc/bin/g++
export LD_LIBRARY_PATH=$GCC_PATH/gcc/lib64:$GCC_PATH/isl/lib:$GCC_PATH/mpc/lib/:$GCC_PATH/mpfr/lib/:$GCC_PATH/gmp/lib/:$LD_LIBRARY_PATH
export PATH=\$GCC_PATH/bin:\$CMAKEROOT/bin:\$PATH
# Gauss DB install path
export PATH=\$GAUSSHOME/bin:\$PATH
export LD_LIBRARY_PATH=\$GAUSSHOME/lib:\$LD_LIBRARY_PATH
export GAUSS_WARNING_TYPE=2
export GS_CLUSTER_NAME=single" >> ~/.bash_profile

source ~/.bash_profile
cd /opt/software/openGauss-server/
./configure --gcc-version=7.3.0 --prefix=$GAUSSHOME CFLAGS="-O0" --enable-debug --without-readline --without-zlib --enable-cassert --enable-thread-safety CC=g++ --3rd=$BINARY_PATH --with-python
make -j8
make install
exit

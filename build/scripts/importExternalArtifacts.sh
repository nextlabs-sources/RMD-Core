#!/bin/sh

export CODEBASE=Doom
export TARGET_OS=macOS
export PLATFORM=x86_64

cd ../..

#
# Import Boost.
#
mkdir -p external/boost
cp -R ${NLGITEXTERNALDIR}/Boost/boost_1_65_1/boost external/boost
mkdir -p external/boost/lib
cp -R ${S_DRIVE}/release_candidate/artifacts/${CODEBASE}/external/${TARGET_OS}/boost/lib/${PLATFORM} external/boost/lib

#
# Import OpenSSL.
#
mkdir -p external/openssl
cp -R ${S_DRIVE}/release_candidate/artifacts/${CODEBASE}/external/${TARGET_OS}/openssl/x64 external/openssl

#
# Import JSON for Modern C++.
#
mkdir -p external/jsonformodernc++
cp -R ${NLGITEXTERNALDIR}/JSONforModernC++/3.7.3/single_include external/jsonformodernc++

cd build/scripts

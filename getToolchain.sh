#!/bin/bash
set -e

# see packages here: http://energia.nu/packages/package_energia_index.json
if [ "$(uname)" == "Darwin" ]; then
    CL_TOOLCHAIN_TARBALL=https://s3.amazonaws.com/energiaUS/tools/macosx/gcc-arm-none-eabi-6.3.1-20170620-mac.tar.bz2
    DSLITE_TARBALL=https://s3.amazonaws.com/energiaUS/tools/macosx/dslite-8.2.0.1400-x86_64-apple-darwin.tar.bz2
    STM32_TOOLS_TARBALL=https://github.com/gnu-mcu-eclipse/openocd/releases/download/v0.10.0-11-20190118/gnu-mcu-eclipse-openocd-0.10.0-11-20190118-1134-macos.tgz
else
    CL_TOOLCHAIN_TARBALL=https://s3.amazonaws.com/energiaUS/tools/linux64/gcc-arm-none-eabi-6.3.1-20170620-x86_64-pc-linux-gnu.tar.bz2
    DSLITE_TARBALL=https://s3.amazonaws.com/energiaUS/tools/linux64/dslite-8.2.0.1400-i386-x86_64-pc-linux-gnu.tar.bz2
    STM32_TOOLS_TARBALL=https://github.com/gnu-mcu-eclipse/openocd/releases/download/v0.10.0-11-20190118/gnu-mcu-eclipse-openocd-0.10.0-11-20190118-1134-centos64.tgz
fi

mkdir -p toolchain/Connected_Launchpad
mkdir -p toolchain/DSLite
mkdir -p toolchain/downloads

if [ ! -f toolchain/downloads/cl_toolchain.tar.bz2 ]; then
    echo "Downloading Connected Launchpad Toolchain"
    curl $CL_TOOLCHAIN_TARBALL --output toolchain/downloads/cl_toolchain.tar.bz2
fi

if [ ! -f toolchain/downloads/cl_dslite.tar.bz2 ]; then
    echo "Downloading DSLite"
    curl $DSLITE_TARBALL --output toolchain/downloads/cl_dslite.tar.bz2
fi

if [ ! -d toolchain/Connected_Launchpad/bin ]; then
    pushd toolchain/Connected_Launchpad
    echo "Extracting Connected Launchpad toolchain"
    tar --strip-components=1 -xf ../downloads/cl_toolchain.tar.bz2
    popd
fi

if [ ! -d toolchain/DSLite/DebugServer ]; then
    pushd toolchain/DSLite
    echo "Extracting Connected Launchpad DSLite"
    tar --strip-components=1 -xf ../downloads/cl_dslite.tar.bz2
    popd
fi

echo "Done"
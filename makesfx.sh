#!/bin/sh

# 20/02/2007
# Script to create a self extractible binary package including FOX libraries
# Such a package can be used on a system were rpm or deb packages are not available
# The package will be installed in /usr/local
# Do not modify it unless you know what you are doing!

# Xfe version
VERSION=1.32.1

# Version of the FOX libraries
FOX_VERSION=1.6

# Location of the FOX libraries on the building system
FOX_LIB=/usr/lib

# Compile Xfe for installation in /usr/local
./configure --prefix=/usr/local --enable-release
make

# Strip binaries
strip src/xfe
strip src/xfi
strip src/xfw
strip src/xfv
strip src/xfp

# Install to the specified directory
rm -f -r xfe-$VERSION
make install DESTDIR=`pwd`/xfe-$VERSION

# Remove the usr/local prefix
mv xfe-$VERSION/usr/local/* xfe-$VERSION
rmdir xfe-$VERSION/usr/local
rmdir xfe-$VERSION/usr

# Add the FOX libraries
mkdir xfe-$VERSION/lib
cp -v -P $FOX_LIB/libFOX-$FOX_VERSION.so* xfe-$VERSION/lib

# Add the doc files
mkdir xfe-$VERSION/share/doc
mkdir xfe-$VERSION/share/doc/xfe-$VERSION
cp -v -P README TODO BUGS AUTHORS ChangeLog COPYING INSTALL NEWS xfe-$VERSION/share/doc/xfe-$VERSION

# The shell scripts must be executable
chmod +x sfx.sh
chmod +x header.sh

# Create the tar archive
tar zcvf xfe-$VERSION-i386.tar.gz xfe-$VERSION sfx.sh

# Create the self extracting package
cat header.sh xfe-$VERSION-i386.tar.gz > xfe-$VERSION-i386.sh

# Clean up
rm -f -r xfe-$VERSION
rm xfe-$VERSION-i386.tar.gz

# End
echo ""
echo "Self extracting package xfe-$VERSION-i386.sh successfully created!"

#!/bin/sh

# 20/02/2007
# Script to install Xfe from a self extracting package
# This script must be located in the same directory as the makesfx script and must be executable!
# Do not modify it unless you know what you are doing!


# Version of Xfe
VERSION=1.32.1

# Installation directory
INSTALL_DIR=/usr/local

# Version of the FOX libraries
FOX_VERSION=1.6

# Process command line arguments

# Uninstall Xfe from $INSTALL_DIR
if [ "$1" = "--uninstall" ]
then	
	
	rm -v -f $INSTALL_DIR/bin/xfe
	rm -v -f $INSTALL_DIR/bin/xfw
	rm -v -f $INSTALL_DIR/bin/xfv
	rm -v -f $INSTALL_DIR/bin/xfi
	rm -v -f $INSTALL_DIR/bin/xfp
	
	rm -v -f -r $INSTALL_DIR/share/xfe
	
	rm -v -f $INSTALL_DIR/share/applications/xfe.desktop
	rm -v -f $INSTALL_DIR/share/applications/xfw.desktop
	rm -v -f $INSTALL_DIR/share/applications/xfv.desktop
	rm -v -f $INSTALL_DIR/share/applications/xfi.desktop
	rm -v -f $INSTALL_DIR/share/applications/xfp.desktop

	rm -v -f $INSTALL_DIR/share/pixmaps/xfe.*
	rm -v -f $INSTALL_DIR/share/pixmaps/xfw.*
	rm -v -f $INSTALL_DIR/share/pixmaps/xfv.*
	rm -v -f $INSTALL_DIR/share/pixmaps/xfi.*
	rm -v -f $INSTALL_DIR/share/pixmaps/xfp.*
	
	rm -v -f $INSTALL_DIR/share/man/man1/xfe.*
	rm -v -f $INSTALL_DIR/share/man/man1/xfw.*
	rm -v -f $INSTALL_DIR/share/man/man1/xfv.*
	rm -v -f $INSTALL_DIR/share/man/man1/xfi.*
	rm -v -f $INSTALL_DIR/share/man/man1/xfp.*
	
	rm -v -f $INSTALL_DIR/share/locale/*/LC_MESSAGES/xfe.mo
	
	rm -v -f -r $INSTALL_DIR/share/doc/xfe-$VERSION
	
	rm -v -f $INSTALL_DIR/lib/libFOX-$FOX_VERSION.*
	
	echo "==========================================================================================="
	echo "===== X File Explorer (Xfe) version $VERSION was successfully uninstalled from $INSTALL_DIR ====="
	echo "==========================================================================================="
	echo ""
	exit;

# Install to $INSTALL_DIR
elif [ "$1" = "--install" ]
then

	# Installation message
	echo "This script will install X File Explorer and the needed FOX libraries in your $INSTALL_DIR directory"
	echo ""
	
# Install to $INSTALL_DIR
elif [ "$1" = "" ]
then

	# Installation message
	echo "This script will install X File Explorer and the needed FOX libraries in your $INSTALL_DIR directory"
	echo " "

# Print usage
else
	
	echo ""
	echo "Usage : $0 [--install] [--uninstall] "
	echo "--install             Install Xfe to /usr/local"
	echo "--uninstall           Uninstall Xfe from /usr/local"
	echo "If no argument is given, install Xfe to /usr/local"
	echo ""
	exit

fi


# Perform installation

# Only root can install in $INSTALL_DIR
user=`whoami`
if [ "$user" != "root" ]
	then
		echo "Caution! You have to run this installer as root!"
		echo ""
		exit;
	fi

# If user is root
echo "Do you want to proceed with the installation [y/n]?"
read ans

if [ "$ans" != "y" ]
	then
		exit;
	fi

# Proceed with installation
if [  -e $INSTALL_DIR/bin/xfe ]
	then
		echo "Uninstalling previous version of Xfe..."

		rm -v -f $INSTALL_DIR/bin/xfe
		rm -v -f $INSTALL_DIR/bin/xfw
		rm -v -f $INSTALL_DIR/bin/xfv
		rm -v -f $INSTALL_DIR/bin/xfi
		rm -v -f $INSTALL_DIR/bin/xfp
		
		rm -v -f -r $INSTALL_DIR/share/xfe
		
		rm -v -f $INSTALL_DIR/share/pixmaps/xfe.png
		rm -v -f $INSTALL_DIR/share/pixmaps/xfw.png
		rm -v -f $INSTALL_DIR/share/pixmaps/xfv.png
		rm -v -f $INSTALL_DIR/share/pixmaps/xfi.png
		rm -v -f $INSTALL_DIR/share/pixmaps/xfp.png
		
		rm -v -f $INSTALL_DIR/share/man/man1/xfe.*
		rm -v -f $INSTALL_DIR/share/man/man1/xfw.*
		rm -v -f $INSTALL_DIR/share/man/man1/xfv.*
		rm -v -f $INSTALL_DIR/share/man/man1/xfi.*
		rm -v -f $INSTALL_DIR/share/man/man1/xfp.*
		
		rm -v -f $INSTALL_DIR/share/locale/*/LC_MESSAGES/xfe.mo
		
		rm -v -f -r $INSTALL_DIR/share/doc/xfe-$VERSION
			
		rm -v -f $INSTALL_DIR/lib/libFOX-$FOX_VERSION.*
		
		echo ""
	fi

echo "Installing Xfe and FOX libraries, please wait..."
cp -v -P -r xfe-$VERSION/* $INSTALL_DIR

# Update the xferc config file to reflect the new install directory
sed "s|//share|$INSTALL_DIR/share|" < $INSTALL_DIR/share/xfe/xferc > $INSTALL_DIR/share/xfe/xferc.tmp
mv $INSTALL_DIR/share/xfe/xferc.tmp $INSTALL_DIR/share/xfe/xferc


# Update the libraries
/sbin/ldconfig

# Done!
echo ""
echo "========================================================================================="
echo "====== X File Explorer (Xfe) version $VERSION was successfully installed in $INSTALL_DIR ======"
echo "======      Please type $INSTALL_DIR/bin/xfe in a console to start the program!      ======"
echo "====== If something went wrong, first check that $INSTALL_DIR/lib is in your library path, ======"
echo "====== and if necessary, modify your /etc/ld.so.conf file, then launch /sbin/ldconfig as root. ======"  
echo "========================================================================================="
echo ""
echo "[To uninstall Xfe, type the following command as root : sh xfe-$VERSION-i386.sh --uninstall $INSTALL_DIR"]
echo ""





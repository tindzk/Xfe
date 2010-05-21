#!/bin/sh

# 20/02/2007
# This script is the header of the self extracting package
# Do not modify it unless you know what you are doing!

VERSION=1.32.1

echo ""
echo "X File Explorer (Xfe) Version $VERSION"
echo "[Type: sh $0 --help in a console to see the command line options of this installer]"


if [ "$1" = "--uninstall" ]
then	

	echo ""
	

elif [ "$1" = "--install" ]
then

	echo ""
	
	
elif [ "$1" = "" ]
then
	
	echo ""


# Print usage
else
	
	echo ""
	echo "Usage : $0 [--install] [--uninstall] "
	echo "--install               Install Xfe to /usr/local"
	echo "--uninstall             Uninstall Xfe from /usr/local"
	echo "If no argument is given, install Xfe to /usr/local"
	echo ""
	exit

fi

# Create a temp directory to extract to.
export WRKDIR=`mktemp -d /tmp/selfextract.XXXXXX`

SKIP=`awk '/^__ARCHIVE_FOLLOWS__/ { print NR + 1; exit 0; }' $0`

# Take the TGZ portion of this file and pipe it to tar.
tail -n +$SKIP $0 | tar xz -C $WRKDIR

# Execute the installation script

PREV=`pwd`
cd $WRKDIR
./sfx.sh $1 $2


# Delete the temp files
cd $PREV
rm -rf $WRKDIR

exit 0

__ARCHIVE_FOLLOWS__

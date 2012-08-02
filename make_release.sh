#!/bin/sh
BASENAME=libquickmail-`head -n 1 ChangeLog|sed -e 's/\s*$//'`
TARBALL=$BASENAME.tar.xz
BINDIST=$BASENAME-binary.zip
FILELIST='*.c *.h *.am *.in README AUTHORS COPYING NEWS ChangeLog INSTALL INSTALL.msys configure.ac configure autogen.sh autom4te.cache config.guess config.sub depcomp install-sh ltmain.sh *.m4 m4 missing'


# create tarball
echo Creating tarball: $TARBALL
mkdir $BASENAME
cp -rf $FILELIST $BASENAME/
rm $TARBALL &> /dev/null
tar cfJ $TARBALL $BASENAME --remove-files

#clean  up
rm -rf $BASENAME &> /dev/null

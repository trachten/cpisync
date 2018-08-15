#!/bin/bash -x

#
# Generated - do not edit!
#

# Macros
TOP=`pwd`
<<<<<<< HEAD
CND_PLATFORM=GNU-Linux
=======
CND_PLATFORM=brew_gcc-MacOSX
>>>>>>> 15f3fb765e96a2244a98cdd558e4ff1703d4bf5d
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build
CND_DLIB_EXT=so
NBTMPDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tmp-packaging
TMPDIRNAME=tmp-packaging
OUTPUT_PATH=${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcpisync.${CND_DLIB_EXT}
OUTPUT_BASENAME=libcpisync.${CND_DLIB_EXT}
<<<<<<< HEAD
PACKAGE_TOP_DIR=libcpisync.so/
=======
PACKAGE_TOP_DIR=libcpisync.dylib/
>>>>>>> 15f3fb765e96a2244a98cdd558e4ff1703d4bf5d

# Functions
function checkReturnCode
{
    rc=$?
    if [ $rc != 0 ]
    then
        exit $rc
    fi
}
function makeDirectory
# $1 directory path
# $2 permission (optional)
{
    mkdir -p "$1"
    checkReturnCode
    if [ "$2" != "" ]
    then
      chmod $2 "$1"
      checkReturnCode
    fi
}
function copyFileToTmpDir
# $1 from-file path
# $2 to-file path
# $3 permission
{
    cp "$1" "$2"
    checkReturnCode
    if [ "$3" != "" ]
    then
        chmod $3 "$2"
        checkReturnCode
    fi
}

# Setup
cd "${TOP}"
mkdir -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package
rm -rf ${NBTMPDIR}
mkdir -p ${NBTMPDIR}

# Copy files and create directories and links
cd "${TOP}"
<<<<<<< HEAD
makeDirectory "${NBTMPDIR}/libcpisync.so/lib"
=======
makeDirectory "${NBTMPDIR}/libcpisync.dylib/lib"
>>>>>>> 15f3fb765e96a2244a98cdd558e4ff1703d4bf5d
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644


# Generate tar file
cd "${TOP}"
<<<<<<< HEAD
rm -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libcpisync.so.tar
cd ${NBTMPDIR}
tar -vcf ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libcpisync.so.tar *
=======
rm -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libcpisync.dylib.tar
cd ${NBTMPDIR}
tar -vcf ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libcpisync.dylib.tar *
>>>>>>> 15f3fb765e96a2244a98cdd558e4ff1703d4bf5d
checkReturnCode

# Cleanup
cd "${TOP}"
rm -rf ${NBTMPDIR}

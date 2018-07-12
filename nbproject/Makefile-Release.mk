#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc-6
CCC=g++-6
CXX=g++-6
FC=gfortran
AS=as

# Macros
CND_PLATFORM=brew_gcc-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/CPISync.o \
	${OBJECTDIR}/src/CommSocket.o \
	${OBJECTDIR}/src/CommString.o \
	${OBJECTDIR}/src/Communicant.o \
	${OBJECTDIR}/src/DataObject.o \
	${OBJECTDIR}/src/FullSync.o \
	${OBJECTDIR}/src/GenSync.o \
	${OBJECTDIR}/src/InterCPISync.o \
	${OBJECTDIR}/src/Logger.o \
	${OBJECTDIR}/src/SyncMethod.o \
	${OBJECTDIR}/src/UID.o \
	${OBJECTDIR}/src/probCPISync.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f2 \
	${TESTDIR}/TestFiles/f1 \
	${TESTDIR}/TestFiles/f3

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/tests/AuxiliaryTest.o \
	${TESTDIR}/tests/AuxiliaryTestRunner.o \
	${TESTDIR}/tests/FullSyncTest.o \
	${TESTDIR}/tests/FullSyncTestRunner.o \
	${TESTDIR}/tests/cpi_system_test.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/opt/local/lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcpisync.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcpisync.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcpisync.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -dynamiclib -install_name libcpisync.${CND_DLIB_EXT} -fPIC

${OBJECTDIR}/src/CPISync.o: src/CPISync.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CPISync.o src/CPISync.cpp

${OBJECTDIR}/src/CommSocket.o: src/CommSocket.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CommSocket.o src/CommSocket.cpp

${OBJECTDIR}/src/CommString.o: src/CommString.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CommString.o src/CommString.cpp

${OBJECTDIR}/src/Communicant.o: src/Communicant.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Communicant.o src/Communicant.cpp

${OBJECTDIR}/src/DataObject.o: src/DataObject.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/DataObject.o src/DataObject.cpp

${OBJECTDIR}/src/FullSync.o: src/FullSync.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FullSync.o src/FullSync.cpp

${OBJECTDIR}/src/GenSync.o: src/GenSync.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GenSync.o src/GenSync.cpp

${OBJECTDIR}/src/InterCPISync.o: src/InterCPISync.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InterCPISync.o src/InterCPISync.cpp

${OBJECTDIR}/src/Logger.o: src/Logger.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Logger.o src/Logger.cpp

${OBJECTDIR}/src/SyncMethod.o: src/SyncMethod.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SyncMethod.o src/SyncMethod.cpp

${OBJECTDIR}/src/UID.o: src/UID.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UID.o src/UID.cpp

${OBJECTDIR}/src/probCPISync.o: src/probCPISync.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/probCPISync.o src/probCPISync.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f2: ${TESTDIR}/tests/AuxiliaryTest.o ${TESTDIR}/tests/AuxiliaryTestRunner.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/cpi_system_test.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS}   

${TESTDIR}/TestFiles/f3: ${TESTDIR}/tests/FullSyncTest.o ${TESTDIR}/tests/FullSyncTestRunner.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f3 $^ ${LDLIBSOPTIONS}   


${TESTDIR}/tests/AuxiliaryTest.o: tests/AuxiliaryTest.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/AuxiliaryTest.o tests/AuxiliaryTest.cpp


${TESTDIR}/tests/AuxiliaryTestRunner.o: tests/AuxiliaryTestRunner.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/AuxiliaryTestRunner.o tests/AuxiliaryTestRunner.cpp


${TESTDIR}/tests/cpi_system_test.o: tests/cpi_system_test.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/cpi_system_test.o tests/cpi_system_test.cpp


${TESTDIR}/tests/FullSyncTest.o: tests/FullSyncTest.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/FullSyncTest.o tests/FullSyncTest.cpp


${TESTDIR}/tests/FullSyncTestRunner.o: tests/FullSyncTestRunner.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/opt/local/include -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/FullSyncTestRunner.o tests/FullSyncTestRunner.cpp


${OBJECTDIR}/src/CPISync_nomain.o: ${OBJECTDIR}/src/CPISync.o src/CPISync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/CPISync.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CPISync_nomain.o src/CPISync.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/CPISync.o ${OBJECTDIR}/src/CPISync_nomain.o;\
	fi

${OBJECTDIR}/src/CommSocket_nomain.o: ${OBJECTDIR}/src/CommSocket.o src/CommSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/CommSocket.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CommSocket_nomain.o src/CommSocket.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/CommSocket.o ${OBJECTDIR}/src/CommSocket_nomain.o;\
	fi

${OBJECTDIR}/src/CommString_nomain.o: ${OBJECTDIR}/src/CommString.o src/CommString.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/CommString.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CommString_nomain.o src/CommString.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/CommString.o ${OBJECTDIR}/src/CommString_nomain.o;\
	fi

${OBJECTDIR}/src/Communicant_nomain.o: ${OBJECTDIR}/src/Communicant.o src/Communicant.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Communicant.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Communicant_nomain.o src/Communicant.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Communicant.o ${OBJECTDIR}/src/Communicant_nomain.o;\
	fi

${OBJECTDIR}/src/DataObject_nomain.o: ${OBJECTDIR}/src/DataObject.o src/DataObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/DataObject.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/DataObject_nomain.o src/DataObject.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/DataObject.o ${OBJECTDIR}/src/DataObject_nomain.o;\
	fi

${OBJECTDIR}/src/FullSync_nomain.o: ${OBJECTDIR}/src/FullSync.o src/FullSync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FullSync.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FullSync_nomain.o src/FullSync.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FullSync.o ${OBJECTDIR}/src/FullSync_nomain.o;\
	fi

${OBJECTDIR}/src/GenSync_nomain.o: ${OBJECTDIR}/src/GenSync.o src/GenSync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GenSync.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GenSync_nomain.o src/GenSync.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GenSync.o ${OBJECTDIR}/src/GenSync_nomain.o;\
	fi

${OBJECTDIR}/src/InterCPISync_nomain.o: ${OBJECTDIR}/src/InterCPISync.o src/InterCPISync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/InterCPISync.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InterCPISync_nomain.o src/InterCPISync.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/InterCPISync.o ${OBJECTDIR}/src/InterCPISync_nomain.o;\
	fi

${OBJECTDIR}/src/Logger_nomain.o: ${OBJECTDIR}/src/Logger.o src/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Logger.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Logger_nomain.o src/Logger.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Logger.o ${OBJECTDIR}/src/Logger_nomain.o;\
	fi

${OBJECTDIR}/src/SyncMethod_nomain.o: ${OBJECTDIR}/src/SyncMethod.o src/SyncMethod.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/SyncMethod.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SyncMethod_nomain.o src/SyncMethod.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/SyncMethod.o ${OBJECTDIR}/src/SyncMethod_nomain.o;\
	fi

${OBJECTDIR}/src/UID_nomain.o: ${OBJECTDIR}/src/UID.o src/UID.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/UID.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UID_nomain.o src/UID.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/UID.o ${OBJECTDIR}/src/UID_nomain.o;\
	fi

${OBJECTDIR}/src/probCPISync_nomain.o: ${OBJECTDIR}/src/probCPISync.o src/probCPISync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/probCPISync.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I/opt/local/include -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/probCPISync_nomain.o src/probCPISync.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/probCPISync.o ${OBJECTDIR}/src/probCPISync_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f2 || true; \
	    ${TESTDIR}/TestFiles/f1 || true; \
	    ${TESTDIR}/TestFiles/f3 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

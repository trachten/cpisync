cmake_minimum_required(VERSION 3.6)
project(cpisync)
set(CMAKE_CXX_STANDARD 11)

include(CTest)
include(GNUInstallDirs)

# some flags
set(CMAKE_CXX_FLAGS "-DDEFAULT_LOGLEVEL=TEST")

# Set project directory strucuture
set(SRC_DIR src)
set(AUX_DIR ${SRC_DIR}/Aux)
set(DATA_DIR ${SRC_DIR}/Data)
set(COMM_DIR ${SRC_DIR}/Communicants)
set(SYNC_DIR ${SRC_DIR}/Syncs)

set(INCLUDE include/CPISync)
set(AUX_DIR_INC ${INCLUDE}/Aux)
set(DATA_DIR_INC ${INCLUDE}/Data)
set(COMM_DIR_INC ${INCLUDE}/Communicants)
set(SYNC_DIR_INC ${INCLUDE}/Syncs)

#Test directory structure
set(TEST_DIR tests)
set(UNIT_TEST_DIR ${TEST_DIR}/unit)
set(SYSLONG_TEST_DIR ${TEST_DIR}/sys/long)
set(BENCHMARK_TEST_DIR ${TEST_DIR}/sys/benchmark)

# Set location of the test runner
set(TEST_RUNNER ${TEST_DIR}/testRunner.cpp)

# Set file config
set(SOURCE_FILES

        ${AUX_DIR}/Logger.cpp
        ${AUX_DIR}/UID.cpp
        ${AUX_DIR}/SyncMethod.cpp

        ${DATA_DIR}/DataObject.cpp

        ${COMM_DIR}/CommSocket.cpp
        ${COMM_DIR}/CommString.cpp
        ${COMM_DIR}/Communicant.cpp
        ${COMM_DIR}/CommDummy.cpp

        ${SYNC_DIR}/CPISync.cpp
        ${SYNC_DIR}/GenSync.cpp
        ${SYNC_DIR}/InterCPISync.cpp
        ${SYNC_DIR}/probCPISync.cpp
        ${SYNC_DIR}/HashSync.cpp
        ${SYNC_DIR}/IBLT.cpp
        ${SYNC_DIR}/IBLTSync.cpp
        ${SYNC_DIR}/IBLTSetOfSets.cpp
        ${SYNC_DIR}/FullSync.cpp

        ${SRC_DIR}/main.cpp)

set(HEADERS

        ${AUX_DIR_INC}/Auxiliary.h
        ${AUX_DIR_INC}/ConstantsAndTypes.h
        ${AUX_DIR_INC}/Exceptions.h
        ${AUX_DIR_INC}/ForkHandle.h
        ${AUX_DIR_INC}/Logger.h
        ${AUX_DIR_INC}/SyncMethod.h
        ${AUX_DIR_INC}/UID.h

        ${DATA_DIR_INC}/DataFileC.h
        ${DATA_DIR_INC}/DataMemC.h
        ${DATA_DIR_INC}/DataObjC.h
        ${DATA_DIR_INC}/DataObject.h
        ${DATA_DIR_INC}/DataPriorityObject.h

        ${COMM_DIR_INC}/CommSocket.h
        ${COMM_DIR_INC}/CommString.h
        ${COMM_DIR_INC}/Communicant.h
        ${COMM_DIR_INC}/CommDummy.h

        ${SYNC_DIR_INC}/CPISync.h
        ${SYNC_DIR_INC}/CPISync_ExistingConnection.h
        ${SYNC_DIR_INC}/CPISync_HalfRound.h
        ${SYNC_DIR_INC}/CPISync_HalfRound_Hashed.h
        ${SYNC_DIR_INC}/CPISync_OneLessRound.h
        ${SYNC_DIR_INC}/FullSync.h
        ${SYNC_DIR_INC}/GenSync.h
        ${SYNC_DIR_INC}/HashSync.h
        ${SYNC_DIR_INC}/IBLT.h
        ${SYNC_DIR_INC}/IBLTSync.h
        ${SYNC_DIR_INC}/IBLTSetOfSets.h
        ${SYNC_DIR_INC}/IBLTSync_HalfRound.h
        ${SYNC_DIR_INC}/InterCPISync.h
        ${SYNC_DIR_INC}/PrioCPISync.h
        ${SYNC_DIR_INC}/ProbCPISync.h)

include_directories(include)
include_directories(tests)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib64)
include_directories(${CMAKE_INSTALL_INCLUDEDIR})
link_directories(${CMAKE_INSTALL_LIBDIR})

# Add libs and executables
add_library(cpisync STATIC ${SOURCE_FILES} ${HEADERS})
target_include_directories(cpisync PRIVATE ${CMAKE_SOURCE_DIR}/include)
TARGET_LINK_LIBRARIES(cpisync ntl cppunit pthread gmp)

install(CODE "FILE(REMOVE_RECURSE ${CMAKE_INSTALL_INCLUDEDIR}/cpisync)") #Remove the existing CPISync library data and reinstall
install(CODE "FILE(REMOVE_RECURSE ${CMAKE_INSTALL_LIBDIR}/libcpisync.a)") #Remove the existing CPISync library data and reinstall
install(TARGETS cpisync LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CPISync) #create a new folder for CPISync inside of the system include directory
install(DIRECTORY ${AUX_DIR_INC} ${DATA_DIR_INC} ${COMM_DIR_INC} ${SYNC_DIR_INC} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CPISync COMPONENT devel) #copy the contents of the include folder into the system include directory

# Add the TryMe executable
add_executable(TryMe ${SRC_DIR}/TryMe.cpp)
target_link_libraries(TryMe cpisync ntl cppunit pthread gmp)

# Define a macro for adding executables testing multiple files
# @param dir The relative path to the folder containing test files to add
# @param name The executable name
macro(add_group_test dir name)
    FILE(GLOB testPaths ${dir}/*Test.cpp ${dir}/*Tests.cpp)
    ADD_EXECUTABLE(${name} ${TEST_RUNNER} ${testPaths})

    FOREACH(test ${testPaths})
        GET_FILENAME_COMPONENT(testName ${test} NAME_WE)
        TARGET_LINK_LIBRARIES(${name} cpisync ntl cppunit pthread gmp)
        ADD_TEST(${testName} ${name})
    ENDFOREACH(test)
endmacro()

# Add test groups (note: executable will throw errors if there are no tests in the respective folder)
add_group_test(${UNIT_TEST_DIR} UnitTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${SYSLONG_TEST_DIR} SystemLongTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${BENCHMARK_TEST_DIR} Benchmark)

#Packaging
set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library for remote file synchronization research")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README.md)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_NAME "CPISync")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "3")
set(CPACK_PACKAGE_CONTACT "Ari Trachtenberg")
set(CPAKC_PACKAGE_VENDOR "NISLAB")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST /usr/local /usr/local/lib64 /usr/local/lib /usr/local/include)
include(CPack)

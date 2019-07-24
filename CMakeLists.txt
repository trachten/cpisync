cmake_minimum_required(VERSION 3.6)
project(CPISync)
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
set(LONGTERM_TEST_DIR ${TEST_DIR}/sys/longTerm)

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
        ${SYNC_DIR_INC}/IBLTSync_HalfRound.h
        ${SYNC_DIR_INC}/InterCPISync.h
        ${SYNC_DIR_INC}/PrioCPISync.h
        ${SYNC_DIR_INC}/ProbCPISync.h)

include_directories(include)
include_directories(tests)

# Add libs and executables
add_library(CPISync STATIC ${SOURCE_FILES} ${HEADERS})
target_include_directories(CPISync PRIVATE ${CMAKE_SOURCE_DIR}/include)
TARGET_LINK_LIBRARIES(CPISync ntl cppunit pthread gmp)

install(CODE "FILE(REMOVE_RECURSE /usr/local/include/CPISync)") #Remove the existing CPISync library data and reinstall
install(CODE "FILE(REMOVE_RECURSE /usr/local/lib/libCPISync.a)") #Remove the existing CPISync library data and reinstall
install(TARGETS CPISync LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CPISync) #create a new folder for CPISync inside of the system include directory
install(DIRECTORY ${AUX_DIR_INC} ${DATA_DIR_INC} ${COMM_DIR_INC} ${SYNC_DIR_INC} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CPISync COMPONENT devel) #copy the contents of the include folder into the system include directory

# Add the TryMe executable
add_executable(TryMe ${SRC_DIR}/TryMe.cpp)
target_link_libraries(TryMe CPISync ntl cppunit pthread gmp)

# Define a macro for adding executables testing multiple files
# @param dir The relative path to the folder containing test files to add
# @param name The executable name
macro(add_group_test dir name)
    FILE(GLOB testPaths ${dir}/*Test.cpp ${dir}/*Tests.cpp)
    ADD_EXECUTABLE(${name} ${TEST_RUNNER} ${testPaths})

    FOREACH(test ${testPaths})
        GET_FILENAME_COMPONENT(testName ${test} NAME_WE)
        TARGET_LINK_LIBRARIES(${name} CPISync ntl cppunit pthread gmp)
        ADD_TEST(${testName} ${name})
    ENDFOREACH(test)
endmacro()

# Add test groups (note: executable will throw errors if there are no tests in the respective folder)
add_group_test(${UNIT_TEST_DIR} UnitTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${SYSLONG_TEST_DIR} SystemLongTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${BENCHMARK_TEST_DIR} Benchmark)
add_group_test(${LONGTERM_TEST_DIR} LongTerm)
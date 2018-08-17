cmake_minimum_required(VERSION 3.6)
project(cpisynclib)
set(CMAKE_CXX_STANDARD 11)

# some flags
set(CMAKE_CXX_FLAGS "-DDEFAULT_LOGLEVEL=TEST")

# Set project directory strucuture
set(SRC_DIR src)
set(TEST_DIR tests)
set(UNIT_TEST_DIR ${TEST_DIR}/unit)
#set(SYSSHORT_TEST_DIR ${TEST_DIR}/sys/short)
set(SYSLONG_TEST_DIR ${TEST_DIR}/sys/long)

# Set location of the test runner
set(TEST_RUNNER ${TEST_DIR}/testRunner.cpp)

# Set file config
set(SOURCE_FILES
        ${SRC_DIR}/CommSocket.cpp
        ${SRC_DIR}/CommString.cpp
        ${SRC_DIR}/Communicant.cpp
        ${SRC_DIR}/CPISync.cpp
        ${SRC_DIR}/DataObject.cpp
        ${SRC_DIR}/GenSync.cpp
        ${SRC_DIR}/InterCPISync.cpp
        ${SRC_DIR}/Logger.cpp
        ${SRC_DIR}/probCPISync.cpp
        ${SRC_DIR}/SyncMethod.cpp
        ${SRC_DIR}/UID.cpp
		${SRC_DIR}/HashSync.cpp
        ${SRC_DIR}/CommDummy.cpp
        ${SRC_DIR}/IBLT.cpp
        ${SRC_DIR}/IBLTSync.cpp
        ${SRC_DIR}/FullSync.cpp)

#cppunit
include_directories(include)
include_directories(tests)

# Add libs and executables
add_library(cpisynclib ${SOURCE_FILES} )

# Define a macro for adding executables testing multiple files
# @param dir The relative path to the folder containing test files to add
# @param name The executable name
macro(add_group_test dir name)
    FILE(GLOB testPaths ${dir}/*Test.cpp ${dir}/*Tests.cpp)
    ADD_EXECUTABLE(${name} ${TEST_RUNNER} ${testPaths})

    FOREACH(test ${testPaths})
        GET_FILENAME_COMPONENT(testName ${test} NAME_WE)
        TARGET_LINK_LIBRARIES(${name} cpisynclib ntl cppunit gmp)
        ADD_TEST(${testName} ${name})
    ENDFOREACH(test)
endmacro()

# Add test groups (note: executable will throw errors if there are no tests in the respective folder)
add_group_test(${UNIT_TEST_DIR} UnitTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${SYSLONG_TEST_DIR} SystemLongTest)

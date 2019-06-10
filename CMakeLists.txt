cmake_minimum_required(VERSION 3.6)
project(cpisynclib)
set(CMAKE_CXX_STANDARD 11)

include(CTest)

# some flags
set(CMAKE_CXX_FLAGS "-DDEFAULT_LOGLEVEL=TEST")

# Set project directory strucuture
set(SRC_DIR src)
set(AUX_DIR ${SRC_DIR}/Aux)
set(DATA_DIR ${SRC_DIR}/Data)
set(COMM_DIR ${SRC_DIR}/Communicants)
set(SYNC_DIR ${SRC_DIR}/Syncs)

set(TEST_DIR tests)
set(UNIT_TEST_DIR ${TEST_DIR}/unit)
#set(SYSSHORT_TEST_DIR ${TEST_DIR}/sys/short)
set(SYSLONG_TEST_DIR ${TEST_DIR}/sys/long)

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

include_directories(include)
include_directories(tests)


# for external libraries
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Add libs and executables
add_library(cpisynclib ${SOURCE_FILES} ${SRC_DIR}/main.cpp)

# Add the TryMe executable
#add_executable(cpisync ${SRC_DIR}/main.cpp)
#add_executable(tryMe ${SRC_DIR}/TryMe.cpp)
#target_link_libraries(cpisync cpisynclib ntl)
#target_link_libraries(tryMe cpisynclib ntl)

# Add the TryMe executable
add_executable(TryMe ${SRC_DIR}/TryMe.cpp)
target_link_libraries(TryMe cpisynclib ntl cppunit pthread gmp)

# Define a macro for adding executables testing multiple files
# @param dir The relative path to the folder containing test files to add
# @param name The executable name
macro(add_group_test dir name)
    FILE(GLOB testPaths ${dir}/*Test.cpp ${dir}/*Tests.cpp)
    ADD_EXECUTABLE(${name} ${TEST_RUNNER} ${testPaths})

    FOREACH(test ${testPaths})
        GET_FILENAME_COMPONENT(testName ${test} NAME_WE)
        TARGET_LINK_LIBRARIES(${name} cpisynclib ntl cppunit pthread gmp)
        ADD_TEST(${testName} ${name})
    ENDFOREACH(test)
endmacro()

# Add test groups (note: executable will throw errors if there are no tests in the respective folder)
add_group_test(${UNIT_TEST_DIR} UnitTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${SYSLONG_TEST_DIR} SystemLongTest)
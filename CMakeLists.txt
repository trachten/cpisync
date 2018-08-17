cmake_minimum_required(VERSION 3.6)
project(cpisync)
set(CMAKE_CXX_STANDARD 11)


# Set project directory strucuture
set(SRC_DIR src)
set(INCLUDE_DIR include)
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

set(INCLUDE_FILES
        ${INCLUDE_DIR}/Auxiliary.h
        ${INCLUDE_DIR}/CommDummy.h
		${INCLUDE_DIR}/CommSocket.h
		${INCLUDE_DIR}/CommString.h
		${INCLUDE_DIR}/Communicant.h
		${INCLUDE_DIR}/ConstantsAndTypes.h
		${INCLUDE_DIR}/CPISync.h
		${INCLUDE_DIR}/CPISync_ExistingConnection.h
		${INCLUDE_DIR}/CPISync_HalfRound.h
		${INCLUDE_DIR}/CPISync_HalfRound_Hashed.h
		${INCLUDE_DIR}/CPISync_OneLessRound.h
		${INCLUDE_DIR}/DataFileC.h
		${INCLUDE_DIR}/DataMemC.h
		${INCLUDE_DIR}/DataObjC.h
		${INCLUDE_DIR}/DataObject.h
		${INCLUDE_DIR}/DataPriorityObject.h
		${INCLUDE_DIR}/Exceptions.h
		${INCLUDE_DIR}/ForkHandle.h
		${INCLUDE_DIR}/FullSync.h
		${INCLUDE_DIR}/GenSync.h
		${INCLUDE_DIR}/HashSync.h
		${INCLUDE_DIR}/IBLTSync.h
		${INCLUDE_DIR}/IBLTSync_HalfRound.h
		${INCLUDE_DIR}/InterCPISync.h
		${INCLUDE_DIR}/Logger.h
		${INCLUDE_DIR}/PrioCPISync.h
		${INCLUDE_DIR}/ProbCPISync.h
		${INCLUDE_DIR}/SyncMethod.h
		${INCLUDE_DIR}/UID.h)

#cppunit
include_directories(include)
include_directories(tests)

# Add libs and executables
add_library(cpisync STATIC ${SOURCE_FILES})

install(TARGETS cpisync DESTINATION lib)
install(FILES ${INCLUDE_FILES} DESTINATION include/cpisync)

# Define a macro for adding executables testing multiple files
# @param dir The relative path to the folder containing test files to add
# @param name The executable name
macro(add_group_test dir name)
    FILE(GLOB testPaths ${dir}/*Test.cpp)
    ADD_EXECUTABLE(${name} ${TEST_RUNNER} ${testPaths})

    FOREACH(test ${testPaths})
        GET_FILENAME_COMPONENT(testName ${test} NAME_WE)
        TARGET_LINK_LIBRARIES(${name} cpisync ntl cppunit gmp)
        ADD_TEST(${testName} ${name})
    ENDFOREACH(test)
endmacro()

# Add test groups (note: executable will throw errors if there are no tests in the respective folder)
add_group_test(${UNIT_TEST_DIR} UnitTest)
#add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
add_group_test(${SYSLONG_TEST_DIR} SystemLongTest)

cmake_minimum_required(VERSION 3.6)
project(cpisynclib)
set(CMAKE_CXX_STANDARD 11)

# Set project directory strucuture
set(SRC_DIR src)
set(TEST_DIR tests)

# Set file config
set(SOURCE_FILES
        src/CommSocket.cpp
        src/CommString.cpp
        src/Communicant.cpp
        src/CPISync.cpp
        src/DataObject.cpp
        src/FullSync.cpp
        src/GenSync.cpp
        src/InterCPISync.cpp
        src/Logger.cpp
        src/probCPISync.cpp
        src/SyncMethod.cpp
        src/UID.cpp
)

include_directories(include)
include_directories(/opt/local/include)
include_directories(${cppunit_INCLUDE_DIR})
link_directories(/opt/local/lib)

# Add libs and executables
add_library(cpisynclib ${SOURCE_FILES})

#################
### Add tests ###
#################
# Following : https://cmake.org/pipermail/cmake/2007-March/013361.html

ADD_EXECUTABLE(AuxiliaryTest
        ${TEST_DIR}/AuxiliaryTestRunner.cpp
        ${TEST_DIR}/AuxiliaryTest.cpp
)
target_link_libraries(AuxiliaryTest cpisynclib ntl cppunit gmp)
ADD_TEST(AuxiliaryTest AuxiliaryTest)

ADD_EXECUTABLE(SystemTest
        ${TEST_DIR}/cpi_system_test.cpp
)
target_link_libraries(SystemTest cpisynclib ntl cppunit gmp)
ADD_TEST(SystemTest SystemTest)

ADD_EXECUTABLE(FullSyncTest
        ${TEST_DIR}/FullSyncTestRunner.cpp
        ${TEST_DIR}/FullSyncTest.cpp
)
target_link_libraries(FullSyncTest cpisynclib ntl cppunit gmp)
ADD_TEST(FullSyncTest FullSyncTest)

# TODO: Add all testa automagically
# FILE(GLOB UnitTests_SRCS ${TEST_DIR} "*TestRunner.cpp" )
# FOREACH(test ${UnitTests_SRCS})
#         GET_FILENAME_COMPONENT(TestName ${test} NAME_WE)
#         ADD_EXECUTABLE(UnitTester test_runner.cpp ${UnitTests_SRCS})
#         ADD_TEST(${TestName} UnitTester ${TestName})
# ENDFOREACH(test)

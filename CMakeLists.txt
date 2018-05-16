cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/CommSocket.cpp
        src/CommString.cpp
        src/Communicant.cpp
        src/CPISync.cpp
        src/DataObject.cpp
        src/GenSync.cpp
        src/InterCPISync.cpp
        src/Logger.cpp
        src/probCPISync.cpp
        src/SyncMethod.cpp
        src/UID.cpp
        tests/AuxiliaryTest.cpp
        tests/AuxiliaryTest.h
        tests/AuxiliaryTestRunner.cpp
        tests/cpi_system_test.cpp
        )

include_directories(include)
include_directories(/opt/local/include)
include_directories(${cppunit_INCLUDE_DIR})
link_directories(/opt/local/lib)
add_library(cpisynclib ${SOURCE_FILES})
target_link_libraries(cpisynclib ntl cppunit gmp pthread)

add_executable(systemTest tests/cpi_system_test.cpp)
target_link_libraries(systemTest cpisynclib ntl cppunit gmp)

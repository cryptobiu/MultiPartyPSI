cmake_minimum_required(VERSION 3.2)
project(MultiPartyPSI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -maes -mavx -ldl -lpthread -lboost_system -lntl -lgmp -lssl -lcrypto")

SET (BOOST_ROOT /home/naor/boost_1_60_0/)
SET (BOOST_INCLUDEDIR /home/naor/boost_1_60_0/)
SET (BOOST_LIBRARYDIR /home/naor/boost_1_60_0/stage/lib)
find_package(Boost COMPONENTS system thread chrono date_time atomic REQUIRED)

SET(SCAPI_ROOT /home/naor/libscapi/lib/)

INCLUDE_DIRECTORIES(/usr/include/openssl ${SCAPI_ROOT} ${BOOST_INCLUDEDIR})
link_directories(/usr/local/ssl/lib/ ${BOOST_LIBRARYDIR})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/naor/libscapi/MultiPartyPSI/bin)

set(SOURCE_FILES main.cpp PsiParty.cpp common/defs.cpp common/MultiPartyPlayer.cpp PsiServer.cpp PRG/PRG.cpp /home/naor/libscapi/MultiPartyPSI/PSI/src/util/connection.cpp)
add_executable(MultiPartyPSI ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MultiPartyPSI
        /home/naor/libscapi/scapi.a
        /usr/local/ssl/lib/libcrypto.a
        /home/naor/libscapi/install/lib/libOTExtension.so /home/naor/libscapi/install/lib/libmiracl.a
        boost_system boost_thread miracl pthread ssl crypto dl gmpxx gmp z)
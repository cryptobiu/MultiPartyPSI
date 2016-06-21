cmake_minimum_required(VERSION 3.2)
project(MultiPartyPSI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcrypto -ldl -lpthread -lboost_system -lntl -lgmp -lcrypto -lssl")

SET (BOOST_ROOT /home/naor/boost_1_60_0/)
SET (BOOST_INCLUDEDIR /home/naor/boost_1_60_0/)
SET (BOOST_LIBRARYDIR /home/naor/boost_1_60_0/stage/lib)
find_package(Boost COMPONENTS system thread REQUIRED)

SET(SCAPI_ROOT /home/naor/libscapi/lib/)

INCLUDE_DIRECTORIES(/usr/include/openssl ${SCAPI_ROOT} ${BOOST_INCLUDEDIR})
link_directories(/usr/ssl/lib/ ${BOOST_LIBRARYDIR})

set(SOURCE_FILES main.cpp PsiParty.cpp common/defs.cpp common/MultiPartyPlayer.cpp common/statistics.h)
add_executable(MultiPartyPSI ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MultiPartyPSI
        /home/naor/libscapi/scapi.a
        /home/naor/libscapi/install/lib/libOTExtension.so /home/naor/libscapi/install/lib/libmiracl.a
        boost_system boost_thread miracl pthread crypto dl ssl gmpxx gmp z)
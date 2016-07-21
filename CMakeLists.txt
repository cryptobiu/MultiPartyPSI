cmake_minimum_required(VERSION 3.2)
project(MultiPartyPSI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -maes -mavx -ldl -mpclmul -lpthread -lboost_system -lntl -lgmp -lgmpxx -ggdb -lssl -lcrypto -lglib-2.0")

SET (BOOST_ROOT /home/naor/boost_1_60_0/)
SET (BOOST_INCLUDEDIR /home/naor/boost_1_60_0/)
SET (BOOST_LIBRARYDIR /home/naor/boost_1_60_0/stage/lib)
find_package(Boost COMPONENTS system thread chrono date_time atomic REQUIRED)

SET(SCAPI_ROOT /home/naor/libscapi/lib/)

INCLUDE_DIRECTORIES(/usr/include/openssl ${SCAPI_ROOT} ${BOOST_INCLUDEDIR} /usr/include/glib-2.0/ /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/Miracl/include /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib
        /home/naor/libscapi/lib/OTExtensionBristol)
link_directories(/usr/local/ssl/lib/ ${BOOST_LIBRARYDIR} PSI/src/util/crypto/ /home/naor/libscapi/install/lib/ /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib/)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/naor/libscapi/MultiPartyPSI/bin)

AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/externals/Miracl/source MIRACLE_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib MIRACLE_LIB_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/util/crypto CRYPTO_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/util/ot OT_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/hashing HASHING_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/util UTIL_SOURCES)

set(SOURCE_FILES main.cpp NaiveLeader.cpp LeaderFactory.cpp PsiParty.cpp common/defs.cpp common/MultiPartyPlayer.cpp PRG/PRG.cpp /home/naor/libscapi/MultiPartyPSI/PSI/src/util/connection.cpp /home/naor/libscapi/MultiPartyPSI/PSI/src/ot-based/ot-psi.cpp
        NaiveFollower.cpp FollowerFactory.cpp ${MIRACLE_LIB_SOURCES} ${CRYPTO_SOURCES} ${OT_SOURCES} ${HASHING_SOURCES} ${UTIL_SOURCES})
add_executable(MultiPartyPSI ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MultiPartyPSI
        /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib/miracl.a
        /home/naor/libscapi/scapi.a
        /usr/local/ssl/lib/libcrypto.a
        /home/naor/libscapi/install/lib/libOTExtension.so
        boost_system boost_thread miracl pthread ssl crypto glib-2.0 dl gmpxx gmp z)

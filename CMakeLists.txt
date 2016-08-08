cmake_minimum_required(VERSION 2.8)
project(MultiPartyPSI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -maes -mavx -ldl -mpclmul -lpthread -lboost_system -lntl -lgmp -lgmpxx -ggdb -lssl -lcrypto -lglib-2.0")

SET (BOOST_ROOT /home/naor/boost_1_60_0/)
SET (BOOST_INCLUDEDIR /home/naor/boost_1_60_0/)
SET (BOOST_LIBRARYDIR /home/naor/boost_1_60_0/stage/lib)
find_package(Boost COMPONENTS system thread chrono date_time atomic REQUIRED)

SET(SCAPI_ROOT /home/naor/libscapi/lib/)

INCLUDE_DIRECTORIES(/usr/include/openssl ${SCAPI_ROOT} ${BOOST_INCLUDEDIR} /usr/include/glib-2.0/ /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/Miracl/include /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib
        /home/naor/libscapi/MultiPartyPSI/PSI_C_0.1/PSI/headers /home/naor/libscapi/build/NTL/include
        "/home/naor/libscapi/MultiPartyPSI/Header Files" /home/naor/libscapi/MultiPartyPSI/PSI/src/ot-based /home/naor/libscapi/MultiPartyPSI/PSI/src/util
        /home/naor/libscapi/MultiPartyPSI/PSI/src/hashing)
link_directories(/usr/local/ssl/lib/ ${BOOST_LIBRARYDIR} PSI/src/util/crypto/ /home/naor/libscapi/install/lib/ /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib/)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/naor/libscapi/MultiPartyPSI/bin)

AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/externals/Miracl/source MIRACLE_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib MIRACLE_LIB_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/util/crypto CRYPTO_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/util/ot OT_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/hashing HASHING_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI/src/util UTIL_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI_C_0.1/PSI/PSI BF_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI_C_0.1/PSI/OT BF_OT_SOURCES)
AUX_SOURCE_DIRECTORY(/home/naor/libscapi/MultiPartyPSI/PSI_C_0.1/PSI/Threads BF_THREADS_SOURCES)
AUX_SOURCE_DIRECTORY("/home/naor/libscapi/MultiPartyPSI/Source Files" SOURCES)

set(SOURCE_FILES ${SOURCES} "Paillier/PaillierParty.cpp" "Paillier/KissnerParty.cpp" "Source Files/BaseMPSIParty.cpp" "Source Files/Leader.cpp" "Source Files/SimpleHashingPolynomialFollower.cpp"
        "Source Files/SimpleHashingPolynomialLeader.cpp" "Source Files/SimpleHashingNaiveFollower.cpp"
        "Source Files/SimpleHashingNaiveLeader.cpp"
        "Source Files/GarbledBloomFilter.cpp" PRG/PRG.cpp /home/naor/libscapi/MultiPartyPSI/PSI/src/ot-based/ot-psi.cpp
        ${MIRACLE_LIB_SOURCES} ${CRYPTO_SOURCES} ${OT_SOURCES} ${HASHING_SOURCES} ${UTIL_SOURCES} ${BF_SOURCES} ${BF_OT_SOURCES} ${BF_THREADS_SOURCES})
add_executable(MultiPartyPSI ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MultiPartyPSI
        /home/naor/libscapi/scapi.a
        /home/naor/libscapi/install/lib/libntl.a
        /home/naor/libscapi/install/lib/libOTExtension.so
        /usr/local/ssl/lib/libcrypto.a
        /home/naor/libscapi/MultiPartyPSI/PSI/src/externals/miracl_lib/miracl.a
        boost_system boost_thread miracl pthread ssl crypto glib-2.0 dl gmpxx gmp z)

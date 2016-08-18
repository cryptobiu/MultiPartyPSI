cmake_minimum_required(VERSION 2.8)
project(MultiPartyPSI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -maes -mavx -ldl -mpclmul -lpthread -lboost_system -lntl -lgmp -lgmpxx -ggdb -lssl -lcrypto -lglib-2.0")

SET (BOOST_ROOT $ENV{HOME}/boost_1_60_0/)
SET (BOOST_INCLUDEDIR $ENV{HOME}/boost_1_60_0/)
SET (BOOST_LIBRARYDIR $ENV{HOME}/boost_1_60_0/stage/lib)

find_package(Boost COMPONENTS system thread chrono date_time atomic REQUIRED)

SET(SCAPI_ROOT $ENV{HOME}/libscapi/include/)

INCLUDE_DIRECTORIES(/usr/include/openssl ${SCAPI_ROOT} ${BOOST_INCLUDEDIR} /usr/include/glib-2.0/ /usr/lib/x86_64-linux-gnu/glib-2.0/include
        $ENV{HOME}/MultiPartyPSI/PSI/src/externals/Miracl/include $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib
        $ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/headers $ENV{HOME}/libscapi/build/NTL/include
        "$ENV{HOME}/MultiPartyPSI/Header Files" $ENV{HOME}/MultiPartyPSI/PSI/src/ot-based $ENV{HOME}/MultiPartyPSI/PSI/src/util
        $ENV{HOME}/MultiPartyPSI/PSI/src/hashing)
link_directories(${SSL_LIBDIR} ${BOOST_LIBRARYDIR} PSI/src/util/crypto/ $ENV{HOME}/libscapi/install/lib/ $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib/)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/MultiPartyPSI/bin)

AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/externals/Miracl/source MIRACLE_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib MIRACLE_LIB_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/util/crypto CRYPTO_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/util/ot OT_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/hashing HASHING_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/util UTIL_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/PSI BF_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/OT BF_OT_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/Threads BF_THREADS_SOURCES)
AUX_SOURCE_DIRECTORY("$ENV{HOME}/MultiPartyPSI/Source Files" SOURCES)

set(SOURCE_FILES ${SOURCES} "Source Files/NaiveInverseLeader.cpp" "Source Files/PaillierParty.cpp" "Source Files/KissnerParty.cpp" "Source Files/BaseMPSIParty.cpp" "Source Files/Leader.cpp" "Source Files/SimpleHashingPolynomialFollower.cpp"
        "Source Files/SimpleHashingPolynomialLeader.cpp" "Source Files/SimpleHashingNaiveFollower.cpp"
        "Source Files/SimpleHashingNaiveLeader.cpp"
        "Source Files/GarbledBloomFilter.cpp" $ENV{HOME}/MultiPartyPSI/PSI/src/ot-based/ot-psi.cpp
        ${MIRACLE_LIB_SOURCES} ${CRYPTO_SOURCES} ${OT_SOURCES} ${HASHING_SOURCES} ${UTIL_SOURCES} ${BF_SOURCES} ${BF_OT_SOURCES} ${BF_THREADS_SOURCES} "Header Files/NaiveInverseFollower.h" "Source Files/NaiveInverseLeader.cpp" "Header Files/NaiveInverseLeader.h" "Header Files/SimpleHashingNaiveInverseFollower.h" "Source Files/SimpleHashingNaiveInverseLeader.cpp" "Header Files/SimpleHashingNaiveInverseLeader.h" "Source Files/GF2MatrixUtils.cpp" "Header Files/GF2MatrixUtils.h")
add_executable(MultiPartyPSI ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MultiPartyPSI
        $ENV{HOME}/libscapi/scapi.a
        $ENV{HOME}/libscapi/install/lib/libntl.a
        $ENV{HOME}/libscapi/install/lib/libOTExtension.so
        $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib/miracl.a
        boost_system boost_thread miracl pthread ssl crypto glib-2.0 dl gmpxx gmp z)

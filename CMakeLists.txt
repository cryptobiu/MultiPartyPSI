cmake_minimum_required(VERSION 2.8)
project(MultiPartyPSI)

add_custom_target(miracl
        COMMAND make
        WORKING_DIRECTORY $ENV{HOME}/MultiPartyPSI/PSI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DUSECXXPROF -D_REENTRANT -O2 -lntl -lgf2x -lgmp -lgmpxx -lpthread -ggdb -lssl -lcrypto -lglib-2.0 -maes -mavx -ldl -mpclmul -lboost_system")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D DEBUG")
endif()

SET (BOOST_ROOT $ENV{HOME}/boost_1_60_0/)
SET (BOOST_INCLUDEDIR $ENV{HOME}/boost_1_60_0/)
SET (BOOST_LIBRARYDIR $ENV{HOME}/boost_1_60_0/stage/lib)

find_package(Boost COMPONENTS system thread chrono date_time atomic REQUIRED)

SET(SCAPI_ROOT $ENV{HOME}/libscapi/include/)

INCLUDE_DIRECTORIES(/usr/include/openssl /usr/include/glib-2.0/ /usr/lib/x86_64-linux-gnu/glib-2.0/include
        $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib ${SCAPI_ROOT} ${BOOST_INCLUDEDIR}
        $ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/headers $ENV{HOME}/libscapi/build/NTL/include
        "$ENV{HOME}/MultiPartyPSI/Header Files" $ENV{HOME}/MultiPartyPSI/PSI/src/ot-based $ENV{HOME}/MultiPartyPSI/PSI/src/util
        $ENV{HOME}/MultiPartyPSI/PSI/src/hashing)
link_directories(/usr/local/lib $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib ${SSL_LIBDIR} ${BOOST_LIBRARYDIR})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/MultiPartyPSI/bin)

AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/util/crypto CRYPTO_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/util/ot OT_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/hashing HASHING_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI/src/util UTIL_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/PSI BF_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/OT BF_OT_SOURCES)
AUX_SOURCE_DIRECTORY($ENV{HOME}/MultiPartyPSI/PSI_C_0.1/PSI/Threads BF_THREADS_SOURCES)
AUX_SOURCE_DIRECTORY("$ENV{HOME}/MultiPartyPSI/Source Files" SOURCES)
AUX_SOURCE_DIRECTORY("$ENV{HOME}/MultiPartyPSI/Source Files/Leaders" LEADERS_SOURCES)
AUX_SOURCE_DIRECTORY("$ENV{HOME}/MultiPartyPSI/Source Files/Followers" FOLLOWERS_SOURCES)

set(SOURCE_FILES ${SOURCES} ${LEADERS_SOURCES} ${FOLLOWERS_SOURCES} $ENV{HOME}/MultiPartyPSI/PSI/src/ot-based/ot-psi.cpp
        ${CRYPTO_SOURCES} ${OT_SOURCES} ${HASHING_SOURCES} ${UTIL_SOURCES} ${BF_SOURCES} ${BF_OT_SOURCES} ${BF_THREADS_SOURCES} "Header Files/Followers/NaiveInverseFollower.h" "Source Files/Leaders/NaiveInverseLeader.cpp" "Header Files/Leaders/NaiveInverseLeader.h" "Header Files/Followers/SimpleHashingNaiveInverseFollower.h" "Source Files/Leaders/SimpleHashingNaiveInverseLeader.cpp" "Header Files/Leaders/SimpleHashingNaiveInverseLeader.h" "Source Files/GF2MatrixUtils.cpp" "Header Files/GF2MatrixUtils.h")
add_executable(MultiPartyPSI ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MultiPartyPSI
        $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib/big.o
        $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib/ec2.o
        $ENV{HOME}/MultiPartyPSI/PSI/src/externals/miracl_lib/miracl.a
        $ENV{HOME}/libscapi/scapi.a
        $ENV{HOME}/libscapi/install/lib/libntl.a
        $ENV{HOME}/libscapi/install/lib/libOTExtension.so
        boost_system boost_thread pthread ssl crypto glib-2.0 dl gmpxx gmp z gf2x)

add_dependencies(MultiPartyPSI miracl)
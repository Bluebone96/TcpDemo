cmake_minimum_required(VERSION 2.8)
#SET (CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES /usr/local/bin/gcc)
#SET (CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES /usr/local/bin/g++)
#SET (CMAKE_CXX_FLAGS -I /usr/local/inclue)
project(TcpDemo)
add_definitions(-std=c++11)


# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O2 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations -fno-strict-aliasing -fprofile-arcs -ftest-coverage")
# set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O2 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations -fno-strict-aliasing  -fprofile-arcs -ftest-coverage")
# SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O2 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations -fno-strict-aliasing")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O2 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations -fno-strict-aliasing")
SET(CMAKE_EXE_LINKER_FLAGS)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# find_package(Protobuf)
# if(Protobuf_FOUND)
#     include_directories(${Protobuf_INCLUDE_DIRS})
# endif()

# set(PROTO_FILES Proto/PlayerInfo.proto)
# PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# add_library(proto ${PROTO_SRCS})

ADD_SUBDIRECTORY(Proto)
ADD_SUBDIRECTORY(SQL)
ADD_SUBDIRECTORY(Player)
ADD_SUBDIRECTORY(Config)
ADD_SUBDIRECTORY(Net)

LINK_DIRECTORIES(/usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 ${CMAKE_SOURCE_DIR}/Config)

SET(EXTRA_LIBS ${EXTRA_LIBS} proto sql player net config tinyxml2 -lprotobuf -lmysqlclient -lmysqlpp -lhiredis -lpthread)


include_directories(.)

include_directories(
    ${PROJECT_SOURCE_DIR}/Proto 
    ${PROJECT_SOURCE_DIR}/Common  
    ${PROJECT_SOURCE_DIR}/Player  
    ${PROJECT_SOURCE_DIR}/Handler 
    ${PROJECT_SOURCE_DIR}/SQL
    ${PROJECT_SOURCE_DIR}/Server
)


AUX_SOURCE_DIRECTORY(./ServerLogin SRC_LOGIN)
AUX_SOURCE_DIRECTORY(./ServerGate SRC_GATE)
AUX_SOURCE_DIRECTORY(./ServerGame SRC_GAME)
AUX_SOURCE_DIRECTORY(./ServerDB SRC_DB)

add_executable(db_server ${SRC_DB})
add_executable(login_server ${SRC_LOGIN})
add_executable(game_server ${SRC_GAME})
add_executable(gate_server ${SRC_GATE})

target_link_libraries(db_server -Wl,--start-group  ${EXTRA_LIBS}  -Wl,--end-group ${CMAKE_EXE_LINKER_FLAGS})
target_link_libraries(login_server -Wl,--start-group  ${EXTRA_LIBS}  -Wl,--end-group ${CMAKE_EXE_LINKER_FLAGS})
target_link_libraries(gate_server -Wl,--start-group  ${EXTRA_LIBS}  -Wl,--end-group ${CMAKE_EXE_LINKER_FLAGS})
target_link_libraries(game_server -Wl,--start-group  ${EXTRA_LIBS}  -Wl,--end-group ${CMAKE_EXE_LINKER_FLAGS})

# set(CLIENT_LIB 
# 	client/client.ccsudo
# )

# add_executable(client client/main.cc ${CLIENT_LIB})
# target_link_libraries(client demo_lib ${Protobuf_LIBRARIES})

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Bin)
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Lib)
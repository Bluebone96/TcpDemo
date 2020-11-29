cmake_minimum_required(VERSION 2.8)
#SET (CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES /usr/local/bin/gcc)
#SET (CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES /usr/local/bin/g++)
#SET (CMAKE_CXX_FLAGS -I /usr/local/inclue)
project(TcpDemo)
add_definitions(-std=c++11)


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations -fno-strict-aliasing")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations -fno-strict-aliasing")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

find_package(Protobuf)
if(Protobuf_FOUND)
    include_directories(${Protobuf_INCLUDE_DIRS})
endif()

set(PROTO_FILES Proto/PlayerInfo.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(proto ${PROTO_SRCS})

#ADD_SUBDIRECTORY(Proto)
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(SQL)
ADD_SUBDIRECTORY(Player)
ADD_SUBDIRECTORY(Handler)

LINK_DIRECTORIES(/usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)

SET(EXTRA_LIBS ${EXTRA_LIBS}  common sql player handle  )


include_directories(.)

include_directories(
    ${PROJECT_SOURCE_DIR}/Proto 
    ${PROJECT_SOURCE_DIR}/Common  
    ${PROJECT_SOURCE_DIR}/Player  
    ${PROJECT_SOURCE_DIR}/Handler 
    ${PROJECT_SOURCE_DIR}/SQL
    ${PROJECT_SOURCE_DIR}/Server
)


AUX_SOURCE_DIRECTORY(./Server SRC_SERVER)

add_executable(server ${SRC_SERVER})

target_link_libraries(server -Wl,--start-group proto ${Protobuf_LIBRARIES} ${EXTRA_LIBS} -lmysqlclient -lmysqlpp -lhiredis -Wl,--end-group)

# set(CLIENT_LIB 
# 	client/client.ccsudo
# )

# add_executable(client client/main.cc ${CLIENT_LIB})
# target_link_libraries(client demo_lib ${Protobuf_LIBRARIES})

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
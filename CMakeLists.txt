# cmake_minimum_required(VERSION 3.10)

# set (PROJECT_NAME TcpDemo)

# project(${PROJECT_NAME})

# aux_source_directory(src DIR_SRCS)

# add_subdirectory(Common)

# add_subdirectory(Handler)

# add_subdirectory(Player)

# add_subdirectory(Server)

# add_subdirectory(Proto)

# add_executable(${PROJECT_NAME} ${DIR_SRCS})

# target_link_libraries(${PROJECT_NAME} )




cmake_minimum_required(VERSION 3.1)
project(TcpDemo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

find_package(Protobuf)
if(Protobuf_FOUND)
    include_directories(${Protobuf_INCLUDE_DIRS})
endif()

include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/Proto ${PROJECT_SOURCE_DIR}/Player  ${PROJECT_SOURCE_DIR}/Common  ${PROJECT_SOURCE_DIR}/Handler ${PROJECT_SOURCE_DIR}/Server
		    ${PROJECT_SOURCE_DIR}/build)
set(LIB_SRC
	Common/Epoll.cpp
    Common/TcpSocket.cpp
    Common/MsgRecord.cpp
    )

set(PROTO_FILES Proto/PlayerInfo.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

set(PROTOLIB ${PROTO_SRCS})

add_library(DemoLib ${LIB_SRC} ${PROTOLIB})
#add_library(Hook_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (Hook_static PROPERTIES OUTPUT_NAME "Hook_lib")

# set(PLAYER_LIB
#     Player/Inventory.hpp
#     Player/PlayerStatus.hpp
#     Player/Player.cpp
# )

set(SERVER_LIB
    Player/Inventory.hpp
    Player/PlayerStatus.hpp
    Player/Player.cpp
    Server/Server.cpp
    Handler/Handle.hpp
    Handler/EventHandler.cpp
    Handler/Dispatcher.cpp
)

# set(HANDLE_LIB
#     Handler/Handle.hpp
#     Handler/EventHandler.cpp
#     Handler/Dispatcher.cpp
# )





add_executable(Server Server/main.cpp   ${SERVER_LIB} )
target_link_libraries(Server DemoLib ${Protobuf_LIBRARIES})

# set(CLIENT_LIB 
# 	client/client.cc
# )

# add_executable(client client/main.cc ${CLIENT_LIB})
# target_link_libraries(client demo_lib ${Protobuf_LIBRARIES})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
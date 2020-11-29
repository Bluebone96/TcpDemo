#
##find_package(Protobuf)
##if(Protobuf_FOUND)
##    include_directories(${Protobuf_INCLUDE_DIRS})
##endif()
##
##PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ./PlayerInfo.proto)
#
#aux_source_directory(. SRC_PROTO)
#
#add_library(proto ${SRC_PROTO})
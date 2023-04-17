cmake_minimum_required(VERSION 2.8)

project(server)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
SET(PROTO_FILE ${CMAKE_SOURCE_DIR}/proto/esw_server.proto)
SET(PROTO_GENERATED_PATH ${CMAKE_SOURCE_DIR}/proto)

INCLUDE(FindProtobuf)

# genetae proto files
FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${PROTO_FILE})

ADD_LIBRARY(proto ${PROTO_HEADER} ${PROTO_SRC})
message("${PROTO_HEADER}")
INCLUDE_DIRECTORIES(${PROTO_GENERATED_PATH})

find_package(PkgConfig REQUIRED)
FIND_PACKAGE(Boost 1.42.0 REQUIRED COMPONENTS system thread regex iostreams)
find_package( Threads )
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

find_package(TBB REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "session.cpp" "tcp_server.cpp")
target_link_libraries(${PROJECT_NAME} proto ${Boost_LIBRARIES} ${Protobuf_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${TBB_IMPORTED_TARGETS})

# move proto files from build to the separate directory
get_filename_component(PROTO_HEADER_NAME ${PROTO_HEADER} NAME)
get_filename_component(PROTO_SRC_NAME ${PROTO_SRC} NAME)
add_custom_target(move-protos ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROTO_GENERATED_PATH}
    COMMAND ${CMAKE_COMMAND} -E rename ${PROTO_HEADER} ${PROTO_GENERATED_PATH}/${PROTO_HEADER_NAME}
    COMMAND ${CMAKE_COMMAND} -E rename ${PROTO_SRC} ${PROTO_GENERATED_PATH}/${PROTO_SRC_NAME}
    )

add_dependencies(move-protos proto)
add_dependencies(server move-protos)

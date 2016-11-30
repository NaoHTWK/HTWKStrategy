#
# Add Protobuf as dependency
#
include(FindProtobuf)

#We already have set all directories we need.
#find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

#
# Find the protoc Executable. It's copied from FindProtobuf.cmake
# which does not find it.
#

#find_program(PROTOBUF_PROTOC_EXECUTABLE                                            
#    NAMES protoc PATHS ${PROTOBUF_ROOT}/bin                                                                   
#    DOC "The Google Protocol Buffers Compiler"                                     
#)                                                                                  
mark_as_advanced(PROTOBUF_PROTOC_EXECUTABLE) 

#
# Generate source files
#
file(GLOB ProtoFiles "${PROTO_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

#
# Build a static library which can be linked against our executables.
#
add_library(naomessages STATIC ${ProtoSources} ${ProtoHeaders})
target_link_libraries(naomessages protobuf)
target_include_directories(naomessages PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

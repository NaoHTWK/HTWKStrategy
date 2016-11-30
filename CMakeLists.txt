project(TeamStrategyDebug)
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -msse4 -g")

add_definitions(-DFIELD_SIZE=9 -DTS_DEBUG)
set(NAO_HW_VERSION 5)

find_package(Qt5Widgets)
find_package(Qt5Core)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine/nullmq
                 ${CMAKE_CURRENT_BINARY_DIR}/nullmq)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine/messages
                 ${CMAKE_CURRENT_BINARY_DIR}/messages)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine/teamstrategy
                 ${CMAKE_CURRENT_BINARY_DIR}/teamstrategy)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tactics
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
     
message("current source dir: " ${CMAKE_CURRENT_SOURCE_DIR})
     
include_directories(
  ./includes/include
  ./includes/3rdparty/spl
  ./includes/firmware/gamecontroller
  ./includes/firmware/include
  ./includes/firmware/vision_primitives
  ./includes/firmware/worldmodel
)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "--------------------- dir='${dir}'")
endforeach()


aux_source_directory(. SRC_LIST)
FILE(GLOB_RECURSE HeaderFiles "*.h")
set(RESOURCE ${CMAKE_SOURCE_DIR}/resources/images.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

message("src_list: " ${SRC_LIST})
message("headerfiles: " ${HeaderFiles})
message("resources: " ${RESOURCE_ADDED})

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HeaderFiles} ${RESOURCE_ADDED})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core nullmq TeamStrategy naomessages) #HTWKVision

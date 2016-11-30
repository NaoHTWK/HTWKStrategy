add_library(nullmq STATIC nullmq.cpp)
target_link_libraries(nullmq pthread)
target_include_directories(nullmq PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

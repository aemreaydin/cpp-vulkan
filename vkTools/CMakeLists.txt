file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(NAME VkTools)

add_library(${NAME} ${SRC})
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
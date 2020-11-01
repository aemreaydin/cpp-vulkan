cmake_minimum_required(VERSION 3.17)
project(vulkan00)

set(CMAKE_CXX_STANDARD 17)

set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/Include")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(VULKAN_LIBRARY "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")
    set(SHADERC_LIBRARY "$ENV{VULKAN_SDK}/Lib/shaderc_shared.lib")
    set(VULKAN_STATIC_LIBRARY "$ENV{VULKAN_SDK}/Lib/vkstatic.1.lib")
else ()
    set(VULKAN_LIBRARY "$ENV{VULKAN_SDK}/Lib32/vulkan-1.lib")
    set(SHADERC_LIBRARY "$ENV{VULKAN_SDK}/Lib32/shaderc_shared.lib")
    set(VULKAN_STATIC_LIBRARY "$ENV{VULKAN_SDK}/Lib32/vkstatic.1.lib")
endif ()

include_directories(${Vulkan_INCLUDE_DIRS})
list(APPEND LIBS ${VULKAN_LIBRARY})
list(APPEND LIBS ${SHADERC_LIBRARY})
message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${LIBS}, $ENV{VULKAN_SDK}")

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(ThirdParty/glfw)
include_directories(ThirdParty/glfw/include)
list(APPEND LIBS glfw)

include_directories(ThirdParty/glm)
include_directories(ThirdParty/loaders)

# Copy assets to build

include_directories(src)
file(GLOB SRC "src/*.cpp")
add_executable(vulkan00 main.cpp ${SRC})
target_link_libraries(vulkan00 ${LIBS})
#
#set(ASSETS ${CMAKE_SOURCE_DIR}/assets)
#set(DESTINATION_ASSETS ${CMAKE_CURRENT_BINARY_DIR}/assets)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS} ${DESTINATION_ASSETS}
#        COMMENT "copy folder from ${ASSETS} => ${DESTINATION_ASSETS}")

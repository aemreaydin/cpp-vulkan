cmake_minimum_required(VERSION 3.17)
project(vulkan_starter)

set(CMAKE_CXX_STANDARD 17)

set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/Include")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(VULKAN_LIBRARY "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")
    set(VULKAN_STATIC_LIBRARY "$ENV{VULKAN_SDK}/Lib/vkstatic.1.lib")
else ()
    set(VULKAN_LIBRARY "$ENV{VULKAN_SDK}/Lib32/vulkan-1.lib")
    set(VULKAN_STATIC_LIBRARY "$ENV{VULKAN_SDK}/Lib32/vkstatic.1.lib")
endif ()

include_directories(${Vulkan_INCLUDE_DIRS})
list(APPEND LIBS ${VULKAN_LIBRARY})
message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${VULKAN_LIBRARY}, $ENV{VULKAN_SDK}")

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(ThirdParty/glfw)
include_directories(ThirdParty/glfw/include)
list(APPEND LIBS glfw)

include_directories(ThirdParty/glm)
include_directories(ThirdParty/loaders)

add_executable(vulkan00 main.cpp window.cpp validationLayer.cpp app.cpp vulkanHelpers.hpp)
target_link_libraries(vulkan00 ${LIBS})

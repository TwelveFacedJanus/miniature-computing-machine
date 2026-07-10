cmake_minimum_required(VERSION 3.11)
project(MentalComputingEngine LANGUAGES CXX)

# Force CMake to look in /usr/local FIRST
set(CMAKE_PREFIX_PATH /usr/local ${CMAKE_PREFIX_PATH})

# Explicitly find GLFW - use glfw3, not glfw
find_package(glfw3 3.4 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

# Find Lua
find_package(PkgConfig REQUIRED)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LUA lua5.3)
    if(NOT LUA_FOUND)
        pkg_check_modules(LUA lua5.4)
    endif()
    if(NOT LUA_FOUND)
        pkg_check_modules(LUA lua)
    endif()
endif()

# Fallback to manual finding
if(NOT LUA_FOUND)
    find_path(LUA_INCLUDE_DIR NAMES lua.h lua.hpp PATHS /usr/include/lua5.3 /usr/include/lua5.4 /usr/include)
    find_library(LUA_LIBRARY NAMES lua lua5.3 lua5.4 PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
    if(LUA_INCLUDE_DIR AND LUA_LIBRARY)
        set(LUA_FOUND TRUE)
        set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
        set(LUA_LIBRARIES ${LUA_LIBRARY})
    endif()
endif()

if(NOT LUA_FOUND)
    message(FATAL_ERROR "Lua not found! Please install liblua5.3-dev")
endif()

message(STATUS "Lua found:")
message(STATUS "  Include: ${LUA_INCLUDE_DIRS}")
message(STATUS "  Libraries: ${LUA_LIBRARIES}")

# Print GLFW info
message(STATUS "GLFW_FOUND: ${GLFW_FOUND}")
message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")

set(SOURCES
  SourceFiles/mental.cpp
  SourceFiles/Graphics/CMentalWindow.cpp
  SourceFiles/Graphics/CMentalRenderer.cpp
  SourceFiles/Graphics/Objects/LuaScript.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link with GLFW and Vulkan
target_link_libraries(${PROJECT_NAME} PRIVATE
  glfw  # This should work if glfw3 is found
  Vulkan::Vulkan
  glm::glm
  ${LUA_LIBRARIES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  ${GLFW_INCLUDE_DIRS}
  ${Vulkan_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIRS}
  SourceFiles/Graphics/Objects
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

# ============================================
# Shader Compilation
# ============================================
find_program(GLSLANG_VALIDATOR glslangValidator)
if(GLSLANG_VALIDATOR)
    message(STATUS "Found shader compiler: ${GLSLANG_VALIDATOR}")
else()
    message(WARNING "No shader compiler found!")
endif()

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

file(GLOB SHADER_SOURCES ${SHADER_SOURCE_DIR}/*.vert ${SHADER_SOURCE_DIR}/*.frag)
set(SHADER_OUTPUTS "")

foreach(SHADER_SOURCE ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)
    get_filename_component(SHADER_EXT ${SHADER_SOURCE} EXT)
    set(SHADER_OUTPUT ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv)
    
    if(${SHADER_EXT} STREQUAL ".vert")
        set(SHADER_STAGE "vert")
    elseif(${SHADER_EXT} STREQUAL ".frag")
        set(SHADER_STAGE "frag")
    else()
        continue()
    endif()
    
    if(GLSLANG_VALIDATOR)
        add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            COMMAND ${GLSLANG_VALIDATOR} -V -S ${SHADER_STAGE} ${SHADER_SOURCE} -o ${SHADER_OUTPUT}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Compiling ${SHADER_STAGE} shader: ${SHADER_NAME} -> ${SHADER_NAME}.spv"
            VERBATIM
        )
        list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
    endif()
endforeach()

if(SHADER_OUTPUTS)
    add_custom_target(CompileShaders ALL DEPENDS ${SHADER_OUTPUTS})
    add_dependencies(${PROJECT_NAME} CompileShaders)
endif()

# ============================================
# Copy Shaders and Scripts to Build Directory
# ============================================

# Copy Shaders
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders 
        ${CMAKE_CURRENT_BINARY_DIR}/Shaders
    COMMENT "Copying shaders to build directory..."
)

# Copy Scripts
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/Scripts 
        ${CMAKE_CURRENT_BINARY_DIR}/Scripts
    COMMENT "Copying scripts to build directory..."
)

# Optional: Copy both at once
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets 
        ${CMAKE_CURRENT_BINARY_DIR}/Assets
    COMMENT "Copying assets to build directory..."
)
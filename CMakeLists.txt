cmake_minimum_required(VERSION 3.15)
project(foo_autostop VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release for single-config generators if not provided
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the configuration to build" FORCE)
endif()

# --- Change to your SDK location ---
set(FOOBAR2000_SDK_PATH "SDK")

# Resource files (včetně resource .rc)
set(SRC_FILES
    src/foo_autostop.cpp
    src/component_version.cpp
    src/foo_autostop.rc
    resources/resource.rc
)

add_library(foo_autostop SHARED ${SRC_FILES})

# Include directories (must include folders pfc, foobar2000, foobar2000/SDK, shared)
target_include_directories(foo_autostop PRIVATE
    "${FOOBAR2000_SDK_PATH}"
    "${FOOBAR2000_SDK_PATH}/foobar2000"
    "${FOOBAR2000_SDK_PATH}/foobar2000/SDK"
    "${FOOBAR2000_SDK_PATH}/foobar2000/shared"
    "${FOOBAR2000_SDK_PATH}/pfc"
    "${FOOBAR2000_SDK_PATH}/foobar2000/SDK/ui"

)

# Compile definitions
target_compile_definitions(foo_autostop PRIVATE
    UNICODE
    _UNICODE
)

# MSVC: Dynamic CRT (/MD pro Release, /MDd pro Debug) and iterator debug level
if(MSVC)
  # Explicit /MD or /MDd
  target_compile_options(foo_autostop PRIVATE
    $<$<CONFIG:Release>:/MD>
    $<$<CONFIG:Debug>:/MDd>
  )

  # iterator debug level per configuration
  target_compile_definitions(foo_autostop PRIVATE
    $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
    $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
  )
endif()

# path to import .lib
set(FOOBAR_SDK_LIB_DIR "${FOOBAR2000_SDK_PATH}/foobar2000/SDK")
set(FOOBAR_LIB_PFC     "${FOOBAR_SDK_LIB_DIR}/pfc.lib")
set(FOOBAR_LIB_SHARED  "${FOOBAR_SDK_LIB_DIR}/shared.lib")
set(FOOBAR_LIB_HELPERS "${FOOBAR_SDK_LIB_DIR}/foobar2000_sdk_helpers.lib")
set(FOOBAR_LIB_SDK     "${FOOBAR_SDK_LIB_DIR}/foobar2000_SDK.lib")
set(FOOBAR_LIB_CLIENT  "${FOOBAR_SDK_LIB_DIR}/foobar2000_component_client.lib")

# Check existence of libraries (informative)
foreach(L ${FOOBAR_LIB_CLIENT} ${FOOBAR_LIB_HELPERS} ${FOOBAR_LIB_SDK} ${FOOBAR_LIB_SHARED} ${FOOBAR_LIB_PFC})
  if(EXISTS "${L}")
    message(STATUS "Found lib: ${L}")
  else()
    message(WARNING "Lib not found: ${L}")
  endif()
endforeach()

# Linking libraries (in order: client, helpers, sdk, shared, pfc)
target_link_libraries(foo_autostop PRIVATE
  "${FOOBAR_LIB_CLIENT}"
  "${FOOBAR_LIB_HELPERS}"
  "${FOOBAR_LIB_SDK}"
  "${FOOBAR_LIB_SHARED}"
  "${FOOBAR_LIB_PFC}"
)

# Cancel prefix "lib" for Windows DLL
set_target_properties(foo_autostop PROPERTIES PREFIX "")

# Info
message(STATUS "Using FOOBAR2000 SDK root: ${FOOBAR2000_SDK_PATH}")
message(STATUS "Linking libs:")
message(STATUS "  ${FOOBAR_LIB_CLIENT}")
message(STATUS "  ${FOOBAR_LIB_HELPERS}")
message(STATUS "  ${FOOBAR_LIB_SDK}")
message(STATUS "  ${FOOBAR_LIB_SHARED}")
message(STATUS "  ${FOOBAR_LIB_PFC}")

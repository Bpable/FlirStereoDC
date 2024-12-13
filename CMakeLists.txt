cmake_minimum_required(VERSION 3.16)

project(FlirStereoMDC LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

# Set the path to Spinnaker SDK
set(SPINNAKER_INCLUDE_DIR "/opt/spinnaker/include")
set(SPINNAKER_LIB_DIR "/opt/spinnaker/lib")

# Add include directories
include_directories(
    ${SPINNAKER_INCLUDE_DIR}
    /usr/include
)

# Link directories
link_directories(
    ${SPINNAKER_LIB_DIR}
    /usr/lib/x86_64-linux-gnu
)

option(LINK_SHARED_ZED "Link with the ZED SDK shared executable" ON)

if (NOT LINK_SHARED_ZED AND MSVC)
    message(FATAL_ERROR "LINK_SHARED_ZED OFF : ZED SDK static libraries not available on Windows")
endif()

find_package(ZED 3 REQUIRED)
find_package(OpenCV REQUIRED core imgproc highgui)
find_package(CUDA ${ZED_CUDA_VERSION} REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FreeImage REQUIRED freeimage)

include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${ZED_LIBRARY_DIR})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

# Manually locate Spinnaker shared library
find_library(Spinnaker_LIB Spinnaker REQUIRED PATHS ${SPINNAKER_LIB_DIR})

set(SOURCES
    connectflir.cpp
    connectdrive.cpp
    connectzed.cpp
    main.cpp
)

set(HEADERS
    connectflir.h
    connectzed.h
    connectdrive.h
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if (LINK_SHARED_ZED)
    SET(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY})
else()
    SET(ZED_LIBS ${ZED_STATIC_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_LIBRARY})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ZED_LIBS} ${OpenCV_LIBRARIES} ${FreeImage_LIBRARIES} ${Spinnaker_LIB})

if(INSTALL_SAMPLES)
    LIST(APPEND SAMPLE_LIST ${PROJECT_NAME})
    SET(SAMPLE_LIST "${SAMPLE_LIST}" PARENT_SCOPE)
endif()

include(GNUInstallDirs)
install(TARGETS FlirStereoMDC
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

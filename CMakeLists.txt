cmake_minimum_required(VERSION 3.16)

project("PSCMScheduler" VERSION 1.0)	

set(CMAKE_CXX_STANDARD 14 REQUIRED ON)

# Request the required wxWidgets libs
find_package(wxWidgets 3.1 COMPONENTS core base aui adv xml REQUIRED)

# Include the wxWidgets use file to initialize various settings
include(${wxWidgets_USE_FILE})

#set(wxWidgets_CONFIGURATION "mswd")

message("wxWidgets_ROOT_DIR: " ${wxWidgets_ROOT_DIR})
message("wxWidgets_LIB_DIR: " ${wxWidgets_LIB_DIR})
message("wxWidgets_CONFIGURATION: " ${wxWidgets_CONFIGURATION})
message("wxWidgets_EXCLUDE_COMMON_LIBRARIES: " ${wxWidgets_EXCLUDE_COMMON_LIBRARIES})

# wxFreeChart libs
set(wxFreeChart_DIR "external/wxFreeChart")
include_directories(
    include
    ${wxFreeChart_DIR}/include/ 
    ${wxFreeChart_DIR}/include/wx 
    ${wxFreeChart_DIR}/include/wx/**)
file(GLOB LIBRARY_SRCS 
    ${wxFreeChart_DIR}/src/*.cpp 
    ${wxFreeChart_DIR}/src/**/*.cpp 
    ${wxFreeChart_DIR}/src/**/**/*.cpp)

# Extract the common parts in a (internal) static library
add_library(wxFreeChart STATIC ${LIBRARY_SRCS})
target_include_directories(wxFreeChart PUBLIC include)

# Project includes and sources
file(GLOB INCLUDE_FILES include/*.h)
file(GLOB SRC_FILES src/*.cpp src/*.rc)

# On Windows include additional resources
if(WIN32)
    # standard manifest file generation is surpressed
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
    # and alternative manifest file is added (dpi-awareness)
    list(APPEND SRC_FILES "${wxWidgets_ROOT_DIR}/include/wx/msw/wx_dpi_aware_pmv2.manifest")
    # include a RC file to add an icon
    list(APPEND SRC_FILES rc/UDE.rc)
endif()

# WIN32 because WinMain is called instead of main at startup on Windows
add_executable(${PROJECT_NAME} WIN32 EXCLUDE_FROM_ALL ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} wxFreeChart)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# On Windows statically link runtime libs (depends on the way wxWidgets was compiled!)
if (WIN32)
    set_target_properties(wxFreeChart ${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

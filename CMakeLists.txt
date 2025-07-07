cmake_minimum_required(VERSION 3.16)
project(SPOBX8Edit VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add CLAP headers
include_directories(include/clap/include)

# Create the plugin library
add_library(SPOBX8Edit SHARED
    src/obx8_plugin.cpp
    src/obx8_parameters.cpp
    src/midi_handler.cpp
    src/midi_device_manager.cpp
    src/plugin_entry.cpp
)

# Set plugin properties
set_target_properties(SPOBX8Edit PROPERTIES
    PREFIX ""
    SUFFIX ".clap"
)

# Platform-specific settings
if(APPLE)
    set_target_properties(SPOBX8Edit PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "clap"
    )
    
    # Link CoreMIDI framework on macOS
    target_link_libraries(SPOBX8Edit 
        "-framework CoreMIDI"
        "-framework CoreFoundation"
    )
endif()

if(WIN32)
    set_target_properties(SPOBX8Edit PROPERTIES
        SUFFIX ".clap"
    )
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(SPOBX8Edit PROPERTIES
        SUFFIX ".clap"
    )
endif()
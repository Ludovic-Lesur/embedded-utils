#
# CMakeLists.txt
#
#  Created on: 13 dec. 2025
#      Author: Ludo
#

# Minimum CMake version.
cmake_minimum_required(VERSION 3.23)

# Project creation.
project(embedded-utils)

# Build mode.
if(NOT DEFINED BUILD_MODE)
    set(BUILD_MODE "STATIC")
endif()

# Create library.
add_library(${PROJECT_NAME} ${BUILD_MODE})

# Static library.
if(${BUILD_MODE} STREQUAL STATIC)

    # Macro to convert options to variables.
    macro(add_compilation_flag FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
        option(FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
        set(${FLAG_NAME} ${FLAG_DEFAULT_VALUE} CACHE STRING ${FLAG_DESCRIPTION})
        list(APPEND COMPILATION_FLAGS_LIST ${FLAG_NAME})
    endmacro()
    
    # Compilation flags.
    add_compilation_flag(EMBEDDED_UTILS_HW_INTERFACE_ERROR_BASE_LAST "Last error base of the low level terminal interface driver." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_DRIVER_DISABLE "Disable the AT driver." OFF)
    add_compilation_flag(EMBEDDED_UTILS_AT_BAUD_RATE "Use a fixed baud rate if defined, otherwise the value is dynamically given." OFF)
    add_compilation_flag(EMBEDDED_UTILS_AT_REPLY_END "Reply ending string." "\"\"")
    add_compilation_flag(EMBEDDED_UTILS_AT_FORCE_OK "Force status printing after command success." ON)
    add_compilation_flag(EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE "Enable AT basic commands (ping, echo, etc.)." ON)
    add_compilation_flag(EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE "Maximum number of commands that can be registered." 32)
    add_compilation_flag(EMBEDDED_UTILS_AT_BUFFER_SIZE "Internal RX buffer size of the AT driver." 64)
    add_compilation_flag(EMBEDDED_UTILS_AT_BOARD_NAME "Board name." "\"name\"")
    add_compilation_flag(EMBEDDED_UTILS_AT_HW_VERSION_MAJOR "Hardware major version." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_HW_VERSION_MINOR "Hardware minor version." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_SW_VERSION_MAJOR "Software major version." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_SW_VERSION_MINOR "Software minor version." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_SW_VERSION_INDEX "Software version index." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_SW_VERSION_DIRTY_FLAG "Software version dirty flag." 0)
    add_compilation_flag(EMBEDDED_UTILS_AT_SW_VERSION_ID "Software version ID." 0)
    add_compilation_flag(EMBEDDED_UTILS_ERROR_STACK_DEPTH "Maximum number of errors stored in stack." 32)    
    add_compilation_flag(EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE "Default value to store in stack when there is no error." 0)
    add_compilation_flag(EMBEDDED_UTILS_ERROR_STACK_SIGFOX "Enable specific function to import Sigfox EP library errors in stack." OFF)
    add_compilation_flag(EMBEDDED_UTILS_MATH_DRIVER_DISABLE "Disable the MATH driver." OFF)
    add_compilation_flag(EMBEDDED_UTILS_MATH_PRECISION "Math computation mode: 0 = using integer 1 = using float 2 = using double." 0)
    add_compilation_flag(EMBEDDED_UTILS_MATH_COS_TABLE "Enable cosine table declaration." ON)
    add_compilation_flag(EMBEDDED_UTILS_MATH_SIN_TABLE "Enable sine table declaration." ON)
    add_compilation_flag(EMBEDDED_UTILS_MATH_ATAN2 "Enable atan2 function." ON)
    add_compilation_flag(EMBEDDED_UTILS_PARSER_DRIVER_DISABLE "Disable the PARSER driver." OFF)
    add_compilation_flag(EMBEDDED_UTILS_STRING_DRIVER_DISABLE "Disable the STRING driver." OFF)
    add_compilation_flag(EMBEDDED_UTILS_STRING_HEXADECIMAL_UPPER_CASE "Select format when converting hexadecimal numbers into string." OFF)
    add_compilation_flag(EMBEDDED_UTILS_SWREG_DRIVER_DISABLE "Disable the SWREG driver." OFF)
    add_compilation_flag(EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE "Disable the TERMINAL driver." OFF)
    add_compilation_flag(EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER "Number of terminals to use." 1)
    add_compilation_flag(EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE "Internal TX buffer size of the terminal driver." 64)
    add_compilation_flag(EMBEDDED_UTILS_TERMINAL_MODE_BUS "Enable destination address setting in terminal driver." OFF)
    
    # Remove OFF flags from list and keep flags set to value 0.
    foreach(FLAG ${COMPILATION_FLAGS_LIST} )
        if((NOT ${FLAG} STREQUAL OFF) AND (NOT ${FLAG} STREQUAL ON))
            set(${FLAG} "(${${FLAG}})")
        endif()
    endforeach()   
    
    # Create flags file.
    configure_file(embedded_utils_flags.h.in embedded_utils_flags.h @ONLY)
    
    # Dependencies folders.
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}
            ${TYPES_PATH}
    )
    
    # Print archive size.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_SIZE_UTIL} -t lib${PROJECT_NAME}.a
    )
    
endif()

# Source files list.
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/at.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/maths.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strings.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/swreg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_hw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal.c
)

# Header files folder.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
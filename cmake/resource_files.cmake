# Function to generate Windows resources with version info
function(add_windows_resources target)
    if(NOT WIN32)
        return()
    endif()

    # Ensure version info is available
    get_target_property(VERSION_STRING ${target} VERSION_STRING)
    if(NOT VERSION_STRING)
        message(WARNING "No version string found for ${target} - call add_git_version_info() first")
        return()
    endif()

    # Parse semantic version from git version string
    if(VERSION_STRING MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
        set(VERSION_MAJOR ${CMAKE_MATCH_1})
        set(VERSION_MINOR ${CMAKE_MATCH_2})
        set(VERSION_PATCH ${CMAKE_MATCH_3})

        # Extract build number from git version format
        set(VERSION_BUILD 0)
        if(VERSION_STRING MATCHES "-dev\\.([0-9]+)")
            # "1.2.3-dev.4" -> build = 4
            set(VERSION_BUILD ${CMAKE_MATCH_1})
        elseif(VERSION_STRING MATCHES "-[a-zA-Z]+\\.[0-9]+\\.([0-9]+)")
            # "1.2.3-alpha.1.2" -> build = 2  
            set(VERSION_BUILD ${CMAKE_MATCH_1})
        endif()
    else()
        message(WARNING "Could not parse version from '${VERSION_STRING}' - using defaults")
        set(VERSION_MAJOR 0)
        set(VERSION_MINOR 0)
        set(VERSION_PATCH 0)
        set(VERSION_BUILD 0)
    endif()

    # Set up resource compiler
    if(CMAKE_RC_COMPILER)
        # Already found (MinGW case)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        # clang-cl case - use MSVC rc.exe
        find_program(CMAKE_RC_COMPILER rc.exe HINTS ${MSVC_TOOLSET_DIR})
        if(NOT CMAKE_RC_COMPILER)
            find_program(CMAKE_RC_COMPILER rc.exe)
        endif()
    endif()

    if(NOT CMAKE_RC_COMPILER)
        message(WARNING "Resource compiler not found - ${target} will not have icon/version info")
        return()
    endif()

    enable_language(RC)

    # Generate the resource file
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target}_resources.rc")

    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/resources.rc.in"
        "${RC_FILE}"
        @ONLY
    )

    # Add to target
    target_sources(${target} PRIVATE ${RC_FILE})
endfunction()

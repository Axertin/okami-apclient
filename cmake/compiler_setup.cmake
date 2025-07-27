# Static Analysis Options
option(ENABLE_CLANG_TIDY "Enable clang-tidy during build" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck during build" OFF)
option(STATIC_ANALYSIS_AS_ERRORS "Treat static analysis warnings as errors" OFF)

# Find static analysis tools
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
find_program(CPPCHECK_EXE NAMES "cppcheck")
find_program(SCAN_BUILD_EXE NAMES "scan-build")

function(enable_strict_warnings target)
    set_property(GLOBAL PROPERTY ENABLE_STRICT_WARNINGS_DEFINED TRUE)
    get_target_property(target_type ${target} TYPE)

    if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
        set(extra_warnings TRUE)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        target_compile_options(${target} PRIVATE -Wall
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-c++17-extensions
            -Wno-unused-lambda-capture
            -Wno-missing-field-initializers)

        if(extra_warnings)
            target_compile_options(${target} PRIVATE -Wextra -Wshadow -Wconversion)
        endif()

        # Add threading-specific warnings for GCC
        # target_compile_options(${target} PRIVATE
        #     -Wthread-safety
        #     -Wno-thread-safety-analysis # Can be noisy, enable selectively
        # )

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE /W4 /permissive-)

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target} PRIVATE
                /clang:-Wno-c++98-compat
                /clang:-Wno-c++98-compat-pedantic
                /clang:-Wno-c++17-extensions
                /clang:-Wno-unused-lambda-capture
                /clang:-Wno-missing-field-initializers
            )

            if(extra_warnings)
                target_compile_options(${target} PRIVATE
                    /clang:-Wold-style-cast
                    /clang:-Wextra-semi
                    /clang:-Wmicrosoft-include
                    /clang:-Wshadow
                    /clang:-Wunused-parameter
                    /clang:-Wnon-virtual-dtor
                    /clang:-Wmissing-variable-declarations
                    /clang:-Wheader-hygiene
                    /clang:-Wnonportable-include-path
                    /clang:-Wpragma-pack
                )
            endif()

            # Add threading-specific warnings for Clang
            target_compile_options(${target} PRIVATE
                /clang:-Wthread-safety
                /clang:-Wthread-safety-analysis
                /clang:-Wthread-safety-precise
                /clang:-Wthread-safety-reference
                /clang:-Wthread-safety-beta
            )
        endif()

        if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
            # target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()

    # Apply static analysis tools if enabled
    apply_static_analysis_to_target(${target})
endfunction()

# Function to apply static analysis to targets
function(apply_static_analysis_to_target target_name)
    get_target_property(target_type ${target_name} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    # Clang-Tidy setup
    if(ENABLE_CLANG_TIDY AND CLANG_TIDY_EXE)
        # Create proper clang-tidy command with semicolon-separated arguments
        set(CLANG_TIDY_COMMAND
            "${CLANG_TIDY_EXE}"
            "-header-filter=.*src/.*"
            "--extra-arg-before=--driver-mode=cl"
            "--extra-arg=/EHsc"
        )

        # Add error handling if requested
        if(STATIC_ANALYSIS_AS_ERRORS)
            list(APPEND CLANG_TIDY_COMMAND "--warnings-as-errors=*")
        endif()

        set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
        )
    endif()

    # Cppcheck setup
    if(ENABLE_CPPCHECK AND CPPCHECK_EXE)
        set_target_properties(${target_name} PROPERTIES
            CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--inconclusive;--force;--inline-suppr;--suppress=missingIncludeSystem;--error-exitcode=$<IF:$<BOOL:${STATIC_ANALYSIS_AS_ERRORS}>,1,0>"
        )
    endif()
endfunction()

# Global scan-build target for comprehensive analysis
if(SCAN_BUILD_EXE)
    add_custom_target(scan-build
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/scan-results
        COMMAND ${SCAN_BUILD_EXE}
        --use-analyzer=clang
        --status-bugs
        -o ${CMAKE_BINARY_DIR}/scan-results
        -enable-checker deadcode
        -enable-checker security.insecureAPI
        -enable-checker unix.Malloc
        -enable-checker core
        -enable-checker cplusplus
        -enable-checker threading
        ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config $<CONFIG> --parallel
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running comprehensive static analysis with scan-build"
    )

    # Convenience target to view results
    if(WIN32)
        add_custom_target(scan-build-view
            COMMAND ${CMAKE_COMMAND} -E echo "Opening scan-build results..."
            COMMAND cmd /c start ${CMAKE_BINARY_DIR}/scan-results/*/index.html
            COMMENT "Opening scan-build results in browser"
            DEPENDS scan-build
        )
    else()
        add_custom_target(scan-build-view
            COMMAND xdg-open ${CMAKE_BINARY_DIR}/scan-results/*/index.html
            COMMENT "Opening scan-build results in browser"
            DEPENDS scan-build
        )
    endif()
endif()

# Add static analysis presets
if(ENABLE_CLANG_TIDY OR ENABLE_CPPCHECK)
    message(STATUS "Static analysis enabled:")
    if(ENABLE_CLANG_TIDY AND CLANG_TIDY_EXE)
        message(STATUS "  - clang-tidy: ${CLANG_TIDY_EXE}")
    endif()
    if(ENABLE_CPPCHECK AND CPPCHECK_EXE)
        message(STATUS "  - cppcheck: ${CPPCHECK_EXE}")
    endif()
    if(STATIC_ANALYSIS_AS_ERRORS)
        message(STATUS "  - Treating warnings as errors")
    endif()
endif()

if(SCAN_BUILD_EXE)
    message(STATUS "scan-build available: Use 'cmake --build . --target scan-build' for comprehensive analysis")
endif()

function(apply_release_optimizations target)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
            target_compile_options(${target} PRIVATE /O2 /Ob2 /GL /Gy)
            target_link_options(${target} PRIVATE /LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO)
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE -O3 -flto)
            target_link_options(${target} PRIVATE -flto LINKER:--gc-sections)
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target} PRIVATE -flto=thin)
        endif()
    endif()
endfunction()

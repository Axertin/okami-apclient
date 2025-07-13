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

        if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
            # target_compile_options(${target} PRIVATE -Werror)
        endif()

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
        endif()

        if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
            # target_compile_options(${target} PRIVATE /WX)
        endif()

        # Prevent Edit-and-Continue even in Debug
        if(POLICY CMP0141)
            set_property(TARGET ${target} PROPERTY MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
        endif()

    endif()
endfunction()

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

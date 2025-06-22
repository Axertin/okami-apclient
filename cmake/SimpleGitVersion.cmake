# SimpleGitVersion.cmake - GitHub Flow friendly versioning

function(add_git_version_info target)
  find_package(Git QUIET)

  if(NOT GIT_FOUND)
    set(VERSION_STRING "0.0.0-unknown")
    set(VERSION_HASH "unknown")
    set(VERSION_BRANCH "unknown")
  else()
    # Get version from git describe
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_DESCRIBE
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # Get short hash
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # Get branch name
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # Parse version from git describe (with prerelease support)
    if(GIT_DESCRIBE MATCHES "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+)(-([a-zA-Z][a-zA-Z0-9.-]*))?")
      set(VERSION_MAJOR ${CMAKE_MATCH_1})
      set(VERSION_MINOR ${CMAKE_MATCH_2})
      set(VERSION_PATCH ${CMAKE_MATCH_3})
      set(VERSION_PRERELEASE ${CMAKE_MATCH_5})

      # Build base version
      set(BASE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
      if(VERSION_PRERELEASE)
        set(BASE_VERSION "${BASE_VERSION}-${VERSION_PRERELEASE}")
      endif()

      # Check if we're exactly on a tag
      if(GIT_DESCRIBE MATCHES "^v?[0-9]+\\.[0-9]+\\.[0-9]+(-[a-zA-Z][a-zA-Z0-9.-]*)?$")
        set(VERSION_STRING "${BASE_VERSION}")
      else()
        # We have commits since tag - extract just the commit count
        if(GIT_DESCRIBE MATCHES "^v?[0-9]+\\.[0-9]+\\.[0-9]+(-[a-zA-Z][a-zA-Z0-9.-]*)?-([0-9]+)-g[a-f0-9]+")
          set(COMMITS_SINCE_TAG ${CMAKE_MATCH_2})

          # Append commits to prerelease or create new prerelease section
          if(VERSION_PRERELEASE)
            set(VERSION_STRING "${BASE_VERSION}.${COMMITS_SINCE_TAG}")
          else()
            set(VERSION_STRING "${BASE_VERSION}-dev.${COMMITS_SINCE_TAG}")
          endif()
        else()
          # Fallback case
          if(VERSION_PRERELEASE)
            set(VERSION_STRING "${BASE_VERSION}")
          else()
            set(VERSION_STRING "${BASE_VERSION}-dev")
          endif()
        endif()

        if(GIT_DESCRIBE MATCHES "dirty$")
          set(VERSION_STRING "${VERSION_STRING}-dirty")
        endif()
      endif()
    else()
      # No version tags found, use commit count as patch
      execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
      )
      set(VERSION_STRING "0.0.${COMMIT_COUNT}-dev")
    endif()

    set(VERSION_HASH ${GIT_HASH})
    set(VERSION_BRANCH ${GIT_BRANCH})
  endif()

  # Generate header file
  set(VERSION_HEADER_DIR "${CMAKE_BINARY_DIR}/generated")
  set(VERSION_HEADER "${VERSION_HEADER_DIR}/version.h")

  file(MAKE_DIRECTORY ${VERSION_HEADER_DIR})
  file(WRITE ${VERSION_HEADER}
    "#pragma once

namespace version {
    const char* string() { return \"${VERSION_STRING}\"; }
    const char* hash() { return \"${VERSION_HASH}\"; }
    const char* branch() { return \"${VERSION_BRANCH}\"; }
}
")

  # Add to target
  target_include_directories(${target} PRIVATE ${VERSION_HEADER_DIR})

  # Set target properties for version info
  set_target_properties(${target} PROPERTIES
    VERSION_STRING "${VERSION_STRING}"
    VERSION_HASH "${VERSION_HASH}"
    VERSION_BRANCH "${VERSION_BRANCH}"
  )

  # Write version to file for CI
  file(WRITE ${CMAKE_BINARY_DIR}/version.txt "${VERSION_STRING}")

  message(STATUS "Version: ${VERSION_STRING} (${VERSION_HASH})")
endfunction()

cmake_minimum_required(VERSION 3.10)

project( Ponca VERSION 0.1)


####################################################################################################
## USER OPTIONS
## Do not edit this file, but rather configure cmake according to your need
##

OPTION( PONCA_CONFIGURE_EXAMPLES "Include compilation rules for built-in examples"      ON)
OPTION( PONCA_CONFIGURE_DOC      "Include compilation rules for built-in documentation" ON)
OPTION( PONCA_CONFIGURE_TESTS    "Include compilation rules for built-in tests"         ON)

##
## END USER OPTIONS
####################################################################################################

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

# add this options before PROJECT keyword
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Ponca LANGUAGES CXX VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# guard against bad build-type strings (source: Eigen)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
if(    NOT cmake_build_type_tolower STREQUAL "debug"
   AND NOT cmake_build_type_tolower STREQUAL "release"
   AND NOT cmake_build_type_tolower STREQUAL "relwithdebinfo")
  message(FATAL_ERROR "Unknown build type \"${CMAKE_BUILD_TYPE}\". Allowed values are Debug, Release, RelWithDebInfo (case-insensitive).")
endif()


# get changeset id
find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${Ponca_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_CHANGESET)
endif()

################################################################################
## Dependencies
################################################################################

find_package(Eigen3 QUIET)
if( NOT Eigen3_FOUND )
    message(WARNING "Cannot find Eigen, use submodule (disable transitive linking)")

    get_directory_property(hasParent PARENT_DIRECTORY)
    if(hasParent)
        set(EIGEN3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/eigen" PARENT_SCOPE)
        message(STATUS "Make EIGEN3_INCLUDE_DIRS available to parent project.")
    else()
        set(EIGEN3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/eigen")
    endif()

    if( NOT EXISTS ${EIGEN_SUBMODULE_ROOT_PATHv}/signature_of_eigen3_matrix_library )
        execute_process(COMMAND git submodule update --init -- ${EIGEN_SUBMODULE_ROOT_PATH}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    include_directories(${EIGEN3_INCLUDE_DIRS})
endif()

################################################################################
# Generate install target                                                      #
################################################################################
set(Ponca_EXPORT_TARGET_DIR \${CMAKE_CURRENT_LIST_DIR})

set(config_install_dir "${CMAKE_INSTALL_PREFIX}/lib/cmake/")
set(include_install_dir "${CMAKE_INSTALL_PREFIX}/include")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

## Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
#set(targets_export_name "${PROJECT_NAME}Targets")
#et(namespace "${PROJECT_NAME}::")

## Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}"
                VERSION ${Ponca_VERSION}
                COMPATIBILITY AnyNewerVersion
)

## Configure '<PROJECT-NAME>Config.cmake'
## version to be installed
## Use variables:
##   * targets_export_name
##   * PROJECT_NAME
## Used by Config.cmake.in
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)


################################################################################
# Add code source, so it is visible in dev environements                       #
################################################################################

set(PONCA_src_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/")
include(PoncaConfigureFitting)
include(PoncaConfigureCommon)

install(DIRECTORY ${PONCA_src_ROOT}/Ponca
    DESTINATION include/
    PATTERN "*~" EXCLUDE)

################################################################################
# API documentation with Doxygen                                               #
################################################################################
if(PONCA_CONFIGURE_DOC)
    add_subdirectory(doc)
endif()

################################################################################
# tests and examples                                                           #
################################################################################
if(PONCA_CONFIGURE_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()

################################################################################
# Tests                                                                        #
################################################################################
if(PONCA_CONFIGURE_TESTS)
    include(CTest)
    set (BUILD_TESTING ON)
    option(PONCA_COVERAGE_TESTING "Enable coverage reporting" OFF)
    add_custom_target(buildtests)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
endif(PONCA_CONFIGURE_TESTS)

################################################################################
# Add custom target for Continuous Integration scripts                         #
################################################################################
add_custom_target(ponca-ci SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/.travis.yml
    ${CMAKE_CURRENT_SOURCE_DIR}/.github/workflows/ContinuousTest.yml
    ${CMAKE_CURRENT_SOURCE_DIR}/.github/workflows/stale.yml)

cmake_minimum_required(VERSION 3.0)
project(MariaDBConnectorCpp)

# for unix platform, define install directories.
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # MACOSX_RPATH
    set(CMAKE_MACOSX_RPATH TRUE)
endif()

set(MARIADB_CONNECTOR_CPP_SOURCE_DIR                "src")
set(MARIADB_CONNECTOR_CPP_INCLUDE_INSTALL_DIR       "include")
set(MARIADB_CONNECTOR_CPP_HEADER_INSTALL_DIR        "${MARIADB_CONNECTOR_CPP_INCLUDE_INSTALL_DIR}/MariaDBConnectorCpp")
################################
# set lib version here

set(GENERIC_LIB_VERSION "1.0.1")
set(GENERIC_LIB_SOVERSION "1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")


message(STATUS "-----------------------")
message(STATUS "..... MariaDB .........")
message(STATUS "-----------------------")
find_package(MariaDB REQUIRED)

if(MARIADB_FOUND)
    message(STATUS "Using MARIADB include dir(s): ${MARIADB_INCLUDE_DIR}")
    message(STATUS "Using MARIADB lib(s): ${MARIADB_LIBRARY}")
else()
    message(FATAL_ERROR "Could not find MARIADB")
endif()


set(SOURCE_FILES ${MARIADB_CONNECTOR_CPP_SOURCE_DIR}/Connection.cpp
        ${MARIADB_CONNECTOR_CPP_SOURCE_DIR}/Result.cpp
        ${MARIADB_CONNECTOR_CPP_SOURCE_DIR}/Statement.cpp
        ${MARIADB_CONNECTOR_CPP_SOURCE_DIR}/Types.cpp
        ${MARIADB_CONNECTOR_CPP_SOURCE_DIR}/Transaction.cpp
        ${MARIADB_CONNECTOR_CPP_SOURCE_DIR}/SavePoint.cpp
        )

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)


# All following targets should search these directories for headers
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${MARIADB_INCLUDE_DIR}
)

option(MARIADB_CONNECTOR_CPP_BUILD_SHARED_LIBS "build as shared library" OFF)
option(MARIADB_CONNECTOR_CPP_BUILD_STATIC_LIBS "build as static library" ON)

if(MARIADB_CONNECTOR_CPP_BUILD_SHARED_LIBS)

    message(STATUS "-----------------------")
    message(STATUS "MariaDBConnectorCpp building shared lib")
    message(STATUS "-----------------------")

    add_library(MariaDBConnectorCpp SHARED ${SOURCE_FILES})

    set_target_properties(MariaDBConnectorCpp PROPERTIES
            COMPILE_DEFINITIONS "MARIADB_CONNECTOR_CPP_EXPORT"
            VERSION "${GENERIC_LIB_VERSION}"
            SOVERSION "${GENERIC_LIB_SOVERSION}")

    target_link_libraries(MariaDBConnectorCpp ${MARIADB_LIBRARY})

    if(DEFINED CMAKE_VERSION AND NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.11")
        target_include_directories(MariaDBConnectorCpp PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

        if(MSVC)
            target_compile_definitions(MariaDBConnectorCpp PUBLIC -D_CRT_SECURE_NO_WARNINGS)
        endif(MSVC)
    else()
        include_directories(${PROJECT_SOURCE_DIR})

        if(MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        endif(MSVC)
    endif()


    # export targets for find_package config mode
    export(TARGETS MariaDBConnectorCpp
            FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)

    install(TARGETS MariaDBConnectorCpp
            EXPORT ${CMAKE_PROJECT_NAME}Targets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()



if(MARIADB_CONNECTOR_CPP_BUILD_STATIC_LIBS)
    message(STATUS "-----------------------")
    message(STATUS "building static lib for mariadb_connector_cpp")
    message(STATUS "-----------------------")


    add_library(MariaDBConnectorCppStatic STATIC ${SOURCE_FILES})

    set_target_properties(MariaDBConnectorCppStatic PROPERTIES
            COMPILE_DEFINITONS "MARIADB_CONNECTOR_CPP_EXPORT"
            VERSION "${GENERIC_LIB_VERSION}"
            SOVERSION "${GENERIC_LIB_SOVERSION}")

    set_target_properties( MariaDBConnectorCppStatic PROPERTIES OUTPUT_NAME mariadb_connector_cpp )

    target_link_libraries(MariaDBConnectorCppStatic ${MARIADB_LIBRARY})

    export(TARGETS MariaDBConnectorCppStatic
            FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)

    install(TARGETS MariaDBConnectorCppStatic
            EXPORT ${CMAKE_PROJECT_NAME}Targets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            )

    if(DEFINED CMAKE_VERSION AND NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.11")
        target_include_directories(MariaDBConnectorCppStatic PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

        if(MSVC)
            target_compile_definitions(MariaDBConnectorCppStatic PUBLIC -D_CRT_SECURE_NO_WARNINGS)
        endif(MSVC)
    else()
        include_directories(${PROJECT_SOURCE_DIR})

        if(MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        endif(MSVC)
    endif()
endif()

install(FILES ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

foreach(p LIB INCLUDE)
    set(var CMAKE_INSTALL_${p}DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()


file(WRITE
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        "include(\${CMAKE_CURRENT_LIST_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)\n")

install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION lib/cmake/${CMAKE_PROJECT_NAME})

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
        DESTINATION lib/cmake/${CMAKE_PROJECT_NAME})

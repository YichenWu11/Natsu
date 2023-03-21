include(CMakeParseArguments)

function(GlobCXXHeaders AbsPath OutFiles)
    file(GLOB_RECURSE
        GlobFiles
        CONFIGURE_DEPENDS
        ${AbsPath}/*.h
        ${AbsPath}/*.hh
        ${AbsPath}/*.hpp
        ${AbsPath}/*.hxx
        ${AbsPath}/*.inl
    )
    set(${OutFiles} ${GlobFiles} PARENT_SCOPE)
endfunction()

function(GlobCXXFiles AbsPath OutFiles)
    file(GLOB_RECURSE
        GlobFiles
        CONFIGURE_DEPENDS
        ${AbsPath}/*.m
        ${AbsPath}/*.mm
        ${AbsPath}/*.cc
        ${AbsPath}/*.cpp
        ${AbsPath}/*.c
        ${AbsPath}/*.cxx
        ${AbsPath}/*.h
        ${AbsPath}/*.hh
        ${AbsPath}/*.hpp
        ${AbsPath}/*.hxx
        ${AbsPath}/*.inl

        ${AbsPath}/*.natvis
    )
    set(${OutFiles} ${GlobFiles} PARENT_SCOPE)
endfunction()

function(Module)
    CMAKE_PARSE_ARGUMENTS(
        MODULE "" "NAME;TYPE;"
        "DEPS;DEPS_PUBLIC;INCLUDES_PUBLIC;INCLUDES;SRC_PATH;LINKS;LINKS_PUBLIC"
        ${ARGN}
    )

    get_property(CurrentScope GLOBAL PROPERTY Scope)

    list(LENGTH MODULE_SRC_PATH _length)

    if(${_length} LESS 1)
        set(MODULE_SRC_PATH src)
    endif()

    list(LENGTH MODULE_INCLUDES_PUBLIC _length2)

    if(${_length2} LESS 1)
        set(MODULE_INCLUDES_PUBLIC include)
    endif()

    list(LENGTH MODULE_INCLUDES _length3)

    if(${_length3} LESS 1)
        set(MODULE_INCLUDES include)
    endif()

    get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    foreach(p ${MODULE_SRC_PATH})
        GlobCXXFiles(${CMAKE_CURRENT_SOURCE_DIR}/${p} MODULE_SRC_T)
        set(MODULE_SRC ${MODULE_SRC} ${MODULE_SRC_T})
    endforeach()

    foreach(p ${MODULE_INCLUDES_PUBLIC})
        GlobCXXHeaders(${CMAKE_CURRENT_SOURCE_DIR}/${p} MODULE_INC_T)

        set(MODULE_REL_HEADER_DIR ${MODULE_REL_HEADER_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${p})
        set(MODULE_INC ${MODULE_INC} ${MODULE_INC_T})
    endforeach()

    if(${MODULE_TYPE} STREQUAL "Library")
        if(FULL_STATIC)
            add_library(${MODULE_NAME} STATIC ${MODULE_SRC})
        else()
            add_library(${MODULE_NAME} SHARED ${MODULE_SRC})
        endif(FULL_STATIC)

        add_library(Natsu::${MODULE_NAME} ALIAS ${MODULE_NAME})
    elseif(${MODULE_TYPE} STREQUAL "StaticLibrary")
        add_library(${MODULE_NAME} STATIC ${MODULE_SRC})
        add_library(Natsu::${MODULE_NAME} ALIAS ${MODULE_NAME})
    elseif(${MODULE_TYPE} STREQUAL "Executable")
        add_executable(${MODULE_NAME} ${MODULE_SRC})
        add_executable(Natsu::${MODULE_NAME} ALIAS ${MODULE_NAME})
    elseif(${MODULE_TYPE} STREQUAL "Test")
        add_executable(${MODULE_NAME} ${MODULE_SRC})
        add_executable(Natsu::${MODULE_NAME} ALIAS ${MODULE_NAME})
    endif(${MODULE_TYPE} STREQUAL "Library")

    target_include_directories(${MODULE_NAME}
        PUBLIC
        ${MODULE_INCLUDES}
        PRIVATE
        ${MODULE_INCLUDES}
    )

    # message(${MODULE_NAME})
    # message(${MODULE_INCLUDES})
    # message(${MODULE_NAME})
    target_link_libraries(${MODULE_NAME}
        PUBLIC
        ${MODULE_DEPS_PUBLIC}
        ${MODULE_LINKS_PUBLIC}
        PRIVATE
        ${MODULE_DEPS}
        ${MODULE_LINKS}
    )
endfunction(Module)

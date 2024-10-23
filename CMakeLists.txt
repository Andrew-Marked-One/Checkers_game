cmake_minimum_required (VERSION 3.8)

project("checkers_game")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED TRUE)

include(FetchContent)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodule during build" ON)
    if(GIT_SUBMODULE)
        FetchContent_Declare(SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 2.6.1
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/sfml
        )
        FetchContent_MakeAvailable(SFML)
    endif()
else()
    message(FATAL_ERROR
        "Git is required but not found or this is not a Git repository.\n"
        "Make sure git is installed and initialized in this directory."
    )
endif()

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

file(GLOB SOURCES "src/*.cpp" )

add_executable (${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-system
    sfml-graphics
    sfml-window
    opengl32
    winmm
    gdi32
)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )
endif()
find_package(Threads)

if (MSVC)
    include(OptionsMSVC)
endif ()

add_definitions(-DBUILDING_JSCONLY__)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_MICRO 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_MICRO})

WEBKIT_OPTION_BEGIN()
WEBKIT_OPTION_DEFINE(ENABLE_STATIC_JSC "Whether to build JavaScriptCore as a static library." PUBLIC OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_REMOTE_INSPECTOR PRIVATE OFF)
if (WIN32)
    # FIXME: Enable FTL on Windows. https://bugs.webkit.org/show_bug.cgi?id=145366
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_FTL_JIT PRIVATE OFF)
    # FIXME: Port bmalloc to Windows. https://bugs.webkit.org/show_bug.cgi?id=143310
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(USE_SYSTEM_MALLOC PRIVATE ON)
endif ()

WEBKIT_OPTION_END()

set(ALL_EVENT_LOOP_TYPES
    GLib
    Generic
)

set(DEFAULT_EVENT_LOOP_TYPE "Generic")

set(EVENT_LOOP_TYPE ${DEFAULT_EVENT_LOOP_TYPE} CACHE STRING "Implementation of event loop to be used in JavaScriptCore (one of ${ALL_EVENT_LOOP_TYPES})")

set(ENABLE_WEBCORE OFF)
set(ENABLE_WEBKIT_LEGACY OFF)
set(ENABLE_WEBKIT OFF)
set(ENABLE_API_TESTS OFF)

if (WTF_CPU_ARM OR WTF_CPU_MIPS)
    SET_AND_EXPOSE_TO_BUILD(USE_CAPSTONE FALSE)
endif ()

# FIXME: JSCOnly on WIN32 seems to only work with fully static build
# https://bugs.webkit.org/show_bug.cgi?id=172862
if (NOT ENABLE_STATIC_JSC AND NOT WIN32)
    set(JavaScriptCore_LIBRARY_TYPE SHARED)
endif ()

if (WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_WINDOWS -DWINVER=0x601 -D_WIN32_WINNT=0x601)
    add_definitions(-DUNICODE -D_UNICODE)

    if (NOT WEBKIT_LIBRARIES_DIR)
        if (DEFINED ENV{WEBKIT_LIBRARIES})
            set(WEBKIT_LIBRARIES_DIR "$ENV{WEBKIT_LIBRARIES}")
        else ()
            set(WEBKIT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/WebKitLibraries/win")
        endif ()
    endif ()

    set(CMAKE_PREFIX_PATH ${WEBKIT_LIBRARIES_DIR})

    if (WTF_CPU_X86_64)
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS OFF)
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)

        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin64)
    endif ()

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif ()

string(TOLOWER ${EVENT_LOOP_TYPE} LOWERCASE_EVENT_LOOP_TYPE)
if (LOWERCASE_EVENT_LOOP_TYPE STREQUAL "glib")
    find_package(GLIB 2.36 REQUIRED COMPONENTS gio gobject)
    SET_AND_EXPOSE_TO_BUILD(USE_GLIB 1)
    SET_AND_EXPOSE_TO_BUILD(USE_GLIB_EVENT_LOOP 1)
    SET_AND_EXPOSE_TO_BUILD(WTF_DEFAULT_EVENT_LOOP 0)
else ()
    SET_AND_EXPOSE_TO_BUILD(USE_GENERIC_EVENT_LOOP 1)
    SET_AND_EXPOSE_TO_BUILD(WTF_DEFAULT_EVENT_LOOP 0)
endif ()

hunter_add_package(ICU)
find_package(ICU CONFIG REQUIRED)
set(ICU_INCLUDE_DIRS "")
set(ICU_DATA_LIBRARIES ICU::data)
if (NOT WEBOS AND NOT BLUESKY)
   set(ICU_I18N_LIBRARIES ICU::i18n)
   set(ICU_LIBRARIES ICU::uc)
endif()

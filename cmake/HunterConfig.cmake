
set(ICU_CONFIGURATION_TYPE "Release")
if (TIZEN_NACL OR PS4 OR BLUESKY OR WEBOS)
    set(ICU_DATA_MODE_ARG "ICU_DATA_ARCHIVE_MODE=ON")
    set(ICU_USE_EXTERNAL_DATA_ARCHIVE "ICU_USE_EXTERNAL_DATA_ARCHIVE=ON")
endif()
if (VS2017 OR UWP)
    set(ICU_CONFIGURATION_TYPE "Release;Debug")
endif()

hunter_config(ICU
        URL "https://github.com/YOU-i-Labs/icu/archive/55.1-youi6.zip"
        VERSION "55.1-youi6"
        SHA1 "311cfcc8c0d70f4d29407fab5ae6b9ad72b33b2d"
        CMAKE_ARGS BUILD_SHARED_LIBS=OFF
                ${ICU_DATA_MODE_ARG}
                ${ICU_USE_EXTERNAL_DATA_ARCHIVE}
        CONFIGURATION_TYPES ${ICU_CONFIGURATION_TYPE})

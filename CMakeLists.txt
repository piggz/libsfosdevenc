cmake_minimum_required(VERSION 3.5)

project(libsfosdevenc
    LANGUAGES CXX
    VERSION 0.1
    DESCRIPTION "Library used by Sailfish OS Device Encryption"
)

find_package (Qt5 COMPONENTS Core REQUIRED)

include(FindPkgConfig)
include(GNUInstallDirs)
pkg_search_module(CRYPTSETUP libcryptsetup REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CONFIG_DIR "/etc/sailfish-device-encryption-community" CACHE STRING "Full path for system configuration directory")
set(RECOVERY_PASSWORD_FILE ".sailfish-device-encryption-community/recovery-password" CACHE STRING "Relative path for recovery password")
set(DECRYPT_CMD "/usr/libexec/sailfish-device-encryption-community/decrypt" CACHE STRING "Device decryption command")

# sources

## C++ sources
file(GLOB CXX_SRC src/*.cpp)
file(GLOB CXX_HEADERS src/*.h)

add_custom_target(Sources SOURCES
    ${CXX_SRC} ${CXX_HEADERS} src/test/test.cpp
    rpm/libsfosdevenc.spec
    )

# compilation
add_compile_options(
    -DCONFIG_DIR="${CONFIG_DIR}"
    -DRECOVERY_PASSWORD_FILE="${RECOVERY_PASSWORD_FILE}"
    -DDECRYPT_CMD="${DECRYPT_CMD}"
)

add_library(sfosdevenc SHARED ${CXX_SRC} ${CXX_HEADERS})

target_link_libraries(sfosdevenc
     Qt5::Core
     ${CRYPTSETUP_LIBRARIES}
)

set_target_properties(sfosdevenc
    PROPERTIES VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_executable(test src/test/test.cpp)
target_link_libraries(test
    sfosdevenc
    Qt5::Core
)
target_include_directories(test PRIVATE src)

# install library and headers
install(
    TARGETS sfosdevenc
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shared
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT development
)

install(
    FILES ${CXX_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/devenc"
    COMPONENT development
)

# install(
#     FILES
#         platform/qt/include/QMapbox
#         platform/qt/include/QMapboxGL
#         platform/qt/include/qmapbox.hpp
#         platform/qt/include/qmapboxgl.hpp
#     DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/qt5"
#     COMPONENT development
# )

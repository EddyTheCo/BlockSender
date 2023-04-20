cmake_minimum_required(VERSION 3.16)
project(IOTA VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

qt6_add_qml_module(blockSender
    URI  blockSender
    VERSION 1.0
    SOURCES
    src/blocksender.cpp include/blocksender.hpp 
    RESOURCE_PREFIX
   "/esterVtech.com/imports"
    OUTPUT_TARGETS out_targets_var
    IMPORTS nodeConection
    IMPORT_PATH
    ${CMAKE_BINARY_DIR}/_deps/nodeconection-build
    OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/blockSender
)

target_link_libraries(blockSender PUBLIC Qt6::Gui Qt6::Quick)

target_include_directories(blockSender PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        "$<INSTALL_INTERFACE:include>")

include(FetchContent)
FetchContent_Declare(
    nodeConection
    GIT_REPOSITORY git@github.com:EddyTheCo/ConectionSettings.git
    GIT_TAG main
)
FetchContent_MakeAvailable(nodeConection)

FetchContent_Declare(
    qaddr_bundle
    GIT_REPOSITORY git@github.com:EddyTheCo/QAddrBundle.git
    GIT_TAG main
)
FetchContent_MakeAvailable(qaddr_bundle)

target_link_libraries(blockSender PUBLIC qaddr_bundle nodeConection)

target_compile_definitions(blockSender
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

install(TARGETS blockSender ${out_targets_var} EXPORT blockSenderTargets DESTINATION bin )
install(DIRECTORY include/ DESTINATION include/)

install(EXPORT blockSenderTargets
    FILE blockSenderTargets.cmake
    DESTINATION lib/cmake/blockSender
)
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/blockSenderConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/blockSender"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/blockSenderConfigVersion.cmake"
    VERSION "0.1.1.0"
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/blockSenderConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/blockSenderConfigVersion.cmake
    DESTINATION lib/cmake/blockSender
)
export(EXPORT blockSenderTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/blockSenderTargets.cmake"
)
list(APPEND QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}/)
set(QML_IMPORT_PATH ${QML_IMPORT_PATH}  CACHE STRING "" FORCE)


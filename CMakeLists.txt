cmake_minimum_required(VERSION 3.22)
project(BoxingMod)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QPM が生成するファイルを読み込む (qpm restore 後に生成される)
include(${CMAKE_CURRENT_LIST_DIR}/qpm_defines.cmake   OPTIONAL)
include(${CMAKE_CURRENT_LIST_DIR}/extern.cmake         OPTIONAL)

# Android NDK ツールチェーンが設定済みであることを前提とする
# (cmake -DCMAKE_TOOLCHAIN_FILE=... で渡される)

add_library(BoxingMod SHARED
    src/main.cpp
    src/BoxingConfig.cpp
    src/BoxingScoreController.cpp
    src/GloveManager.cpp
    src/UIManager.cpp
    src/NoArrowEnforcer.cpp
)

target_include_directories(BoxingMod PRIVATE
    include
    ${EXTERN_INC_DIR}   # QPM が設定する extern/includes
)

target_compile_options(BoxingMod PRIVATE
    -Wall
    -O2
    -fPIC
    -DMOD_ID="${MOD_ID}"
    -DVERSION="${VERSION}"
)

target_link_libraries(BoxingMod
    ${EXTERN_LIB_DIR}/libbeatsaber-hook.so
    log
    android
)


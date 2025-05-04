# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

set(PTUNE4_VERSION "00.07.0000")
cmake_minimum_required(VERSION 3.15)
project(Ptune4 LANGUAGES ASM C VERSION ${PTUNE4_VERSION})

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)
find_package(Gint 2.11 REQUIRED)
find_package(LibProf 2.1 REQUIRED)
find_package(Azur 0.1)
find_package(Git REQUIRED)

string(SUBSTRING ${PTUNE4_VERSION} 1 4 VERSTR)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
  OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
string(SUBSTRING ${COMMIT_ID} 0 7 COMMIT_ID_SHORT)

set(SOURCES
  src/main.c
  src/validate.c
  src/util.c
  src/menu/express.c
  src/menu/settings.c
  src/menu/bsc.c
  src/menu/benchmark.c
  src/menu/mem_data.c
  src/benchmark/dhrystone.c
  src/benchmark/whetstone.c
  src/benchmark/mem_bench.s
  src/benchmark/tick_count.s
  src/memtest/rom_test.c
  src/memtest/sram_test.c
  src/memtest/sdram_test.c
  src/memtest/test_address.c
  src/memtest/mem_read.s
  src/memtest/mem_write.s
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_fx
  # ...
)
set(ASSETS_cg
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

configure_file(config.h.in include/config.h)
add_executable(Ptune4 ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_include_directories(Ptune4 PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_options(Ptune4 PRIVATE -Wall -Wextra -Os -g -flto)
target_link_options(Ptune4 PRIVATE -flto=auto -Wl,-Map=map -Wl,--print-memory-usage)
target_link_libraries(Ptune4 Gint::Gint LibProf::LibProf #[[Azur::Azur]])

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET Ptune4 VERSION ${PTUNE4_VERSION} OUTPUT "Ptune4.g1a"
    NAME "Ptune4" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET Ptune4 VERSION ${PTUNE4_VERSION} OUTPUT "Ptune4.g3a"
    NAME "Ptune4" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET Ptune4 VERSION ${PTUNE4_VERSION} OUTPUT "Ptune4-fx.g3a"
    NAME "Ptune4-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET Ptune4 VERSION ${PTUNE4_VERSION} OUTPUT "Ptune4-hh2.bin")
endif()

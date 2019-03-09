set(CMAKE_SYSTEM_NAME Connected_Launchpad)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR})

# Ideally this would be in the binary directory, however
# when it runs tests it makes a mess of that
set(TOOLCHAIN_DIR ${PROJECT_SOURCE_DIR}/toolchain/${CMAKE_SYSTEM_NAME})

set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++)

include_directories(${CMAKE_CURRENT_LIST_DIR}/tivac-core/variants/EK-TM4C1294XL)

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/tivac-core/variants/EK-TM4C1294XL/lm4fcpp_snowflake.ld)

set(DSLITE_CONFIG ${PROJECT_SOURCE_DIR}/toolchain/DSLite/EK-TM4C1294XL.ccxml)
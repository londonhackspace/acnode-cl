
set(COMMON_FLAGS "-g -Os -w -ffunction-sections -fdata-sections -nostdlib -fno-threadsafe-statics --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -Dprintf=iprintf -MMD -mcpu=cortex-m4 -mthumb -DF_CPU=120000000L -DARDUINO=10610 -DENERGIA=10610 -DENERGIA_EK_TM4C1294XL -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mabi=aapcs")

set(CMAKE_C_FLAGS "${COMMON_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}"  CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -nostdlib -T ${LINKER_SCRIPT} -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=ResetISR -Wl,--unresolved-symbols=report-all -Wl,--warn-common -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wl,--start-group -lc -lm -lgcc" CACHE STRING "")

set(ENERGIA_CORE ${CMAKE_SOURCE_DIR}/targets/tivac-launchpad/tivac-core)
set(TIVA_SYSTEMLIB ${CMAKE_SOURCE_DIR}/targets/tivac-launchpad/tivac-core/system/driverlib/libdriverlib.a)
include_directories(${CMAKE_SOURCE_DIR}/targets/tivac-launchpad/tivac-core/system/inc)
include_directories(${CMAKE_SOURCE_DIR}/targets/tivac-launchpad/tivac-core/system)
include_directories(${CMAKE_SOURCE_DIR}/targets/tivac-launchpad/tivac-core/cores/tivac)

set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(SYSTEM_LIBRARY ConnectedLaunchpad::Core)

set(CONNECTED_LAUNCHPAD 1)

function(makeFlash TARGET)
    add_custom_target(flash 
        COMMAND ${CMAKE_SOURCE_DIR}/toolchain/DSLite/DebugServer/bin/DSLite flash -c ${DSLITE_CONFIG} -f $<TARGET_FILE:${TARGET}>)
endfunction(makeFlash)
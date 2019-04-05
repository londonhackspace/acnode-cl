# build for core Energia libraries

AddVendorLibrary(
    NAMESPACE ConnectedLaunchpad
    TARGET Core
    FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/cores/tivac/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/cores/tivac/avr/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/cores/tivac/*.cpp
    DEPENDENCIES
        ${TIVA_SYSTEMLIB}
)

AddVendorLibrary(
    NAMESPACE ConnectedLaunchpad
    TARGET Ethernet
    FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/Ethernet/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/Ethernet/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/Ethernet/utility/*.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/Ethernet
    DEPENDENCIES
        ConnectedLaunchpad::Core
)

AddVendorLibrary(
    NAMESPACE ConnectedLaunchpad
    TARGET EEPROM
    FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/EEPROM/*.cpp
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/EEPROM
    DEPENDENCIES
        ConnectedLaunchpad::Core
)

AddVendorLibrary(
    NAMESPACE ConnectedLaunchpad
    TARGET wire
    FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/Wire/*.cpp
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/Wire
    DEPENDENCIES
        ConnectedLaunchpad::Core
)

AddVendorLibrary(
    NAMESPACE ConnectedLaunchpad
    TARGET spi
    FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/SPI/*.cpp
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/tivac-core/libraries/SPI
    DEPENDENCIES
        ConnectedLaunchpad::Core
)
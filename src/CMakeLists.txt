
# Globs are actually kinda nasty, but it'll do for now
file(GLOB ACNODECL_SOURCES      ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                ${CMAKE_CURRENT_SOURCE_DIR}/ACServer/*.cpp
                                ${CMAKE_CURRENT_SOURCE_DIR}/CardReader/*.cpp
                                ${CMAKE_CURRENT_SOURCE_DIR}/CardReader/*.c)


set_source_files_properties(${ACNODECL_SOURCES} PROPERTIES COMPILE_FLAGS -Wall)

add_executable(ACNodeCL ${ACNODECL_SOURCES})
target_compile_definitions(ACNodeCL PRIVATE -DCOMMS_HOST_MODE=1)

target_link_libraries(ACNodeCL  ${SYSTEM_LIBRARY}
                                ConnectedLaunchpad::Ethernet
                                syslog
                                PN532_HSU
                                httpclient
                                SD
                                OneMsTaskTimer
                                MQTTClient
                                aJson
                                littlefs)
makeFlash(ACNodeCL)
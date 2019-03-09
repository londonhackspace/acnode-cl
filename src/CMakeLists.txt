
# Globs are actually kinda nasty, but it'll do for now
file(GLOB ACNODECL_SOURCES    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(ACNodeCL ${ACNODECL_SOURCES})

target_link_libraries(ACNodeCL  ${SYSTEM_LIBRARY}
                                ConnectedLaunchpad::Ethernet
                                syslog
                                PN532_HSU
                                httpclient
                                SD
                                OneMsTaskTimer
                                MQTTClient
                                aJson)
makeFlash(ACNodeCL)
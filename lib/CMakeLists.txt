
file(GLOB PN532_SOURCES    ${CMAKE_CURRENT_SOURCE_DIR}/PN532/PN532/*.cpp)

add_library(PN532 ${PN532_SOURCES})
target_include_directories(PN532 PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/PN532/PN532/)

file(GLOB PN532_HSU_SOURCES    ${CMAKE_CURRENT_SOURCE_DIR}/PN532/PN532_HSU/*.cpp)

add_library(PN532_HSU ${PN532_HSU_SOURCES})
target_include_directories(PN532_HSU PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/PN532/PN532_HSU/)
target_link_libraries(PN532_HSU PN532)

file(GLOB HTTPCLIENT_SOURCES 
                       ${CMAKE_CURRENT_SOURCE_DIR}/HttpClient/*.cpp)

add_library(httpclient ${HTTPCLIENT_SOURCES})
target_include_directories(httpclient PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/HttpClient)

file(GLOB SYSLOG_SOURCES 
                       ${CMAKE_CURRENT_SOURCE_DIR}/Syslog/*.cpp)

add_library(syslog ${SYSLOG_SOURCES})
target_include_directories(syslog PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/Syslog)
target_link_libraries(syslog ConnectedLaunchpad::Ethernet)

file(GLOB SD_SOURCES 
                       ${CMAKE_CURRENT_SOURCE_DIR}/SD/*.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/SD/utility/*.cpp)

add_library(SD ${SD_SOURCES})
target_include_directories(SD PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/SD)
target_link_libraries(SD ConnectedLaunchpad::spi)

file(GLOB ONEMSTIMER_SOURCES 
                       ${CMAKE_CURRENT_SOURCE_DIR}/OneMsTaskTimer/*.cpp)

add_library(OneMsTaskTimer ${ONEMSTIMER_SOURCES})
target_include_directories(OneMsTaskTimer PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/OneMsTaskTimer)

file(GLOB MQTTCLIENT_SOURCES 
                       ${CMAKE_CURRENT_SOURCE_DIR}/MQTTClient/*.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/MQTTClient/*.c)

add_library(MQTTClient ${MQTTCLIENT_SOURCES})
target_include_directories(MQTTClient PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/MQTTClient)

file(GLOB AJSON_SOURCES 
                       ${CMAKE_CURRENT_SOURCE_DIR}/aJson/*.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/aJson/utility/*.c)

add_library(aJson ${AJSON_SOURCES})
target_include_directories(aJson PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/aJson)

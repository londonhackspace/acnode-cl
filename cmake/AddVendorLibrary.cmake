
function(AddVendorLibrary)
    set(options ALWAYS)
    set(multiValueArgs FILES INCLUDES DEPENDENCIES)
    set(oneValueArgs NAMESPACE TARGET)
    cmake_parse_arguments(AddVendorLibrary "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    file(GLOB SOURCES    ${AddVendorLibrary_FILES})

    if (SOURCES STREQUAL "")
        message("Adding ${AddVendorLibrary_NAMESPACE}::${AddVendorLibrary_TARGET} as interface")
        add_library(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET}
                INTERFACE)
        target_include_directories(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET} INTERFACE
                                        ${AddVendorLibrary_INCLUDES})
        target_link_libraries(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET} INTERFACE
                             ${AddVendorLibrary_DEPENDENCIES})
    else()
        if(AddVendorLibrary_ALWAYS)
            add_library(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET}
                    ${SOURCES})
        else()
            add_library(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET} EXCLUDE_FROM_ALL
                    ${SOURCES})
        endif()
        target_include_directories(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET} PUBLIC 
                                        ${AddVendorLibrary_INCLUDES})
        target_link_libraries(${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET}
                             ${AddVendorLibrary_DEPENDENCIES})
    endif()

    add_library(${AddVendorLibrary_NAMESPACE}::${AddVendorLibrary_TARGET} 
                ALIAS ${AddVendorLibrary_NAMESPACE}_${AddVendorLibrary_TARGET})
endfunction()